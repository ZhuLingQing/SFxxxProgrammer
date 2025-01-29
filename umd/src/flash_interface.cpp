#include "flash_interface.hpp"
#include "dp_logging.hpp"
#include "flash_database.hpp"
#include "flash_info.hpp"
#include "programmer_interface.hpp"

#include <algorithm>
#include <chrono>
#include <thread>

namespace dp
{
/* static */ const uint32_t FlashInterface::kSleepResolutionUsec_ = 100;

FlashInterface::FlashInterface(std::shared_ptr<ProgrammerInterface> interface, const FlashInfo *flash_info,
                               int site_index)
    : interface_(interface), flash_info_(flash_info), site_index_(site_index)
{
}

// DpError FlashInterface::setAttribute(flash_attr_e attr, long value)
// {
//     std::lock_guard<std::recursive_mutex> lock(mutex_);

//     DP_LOG(WARNING) << "not implemented";
//     return kNotImplemented;
// }

// DpError FlashInterface::getAttribute(flash_attr_e attr, long &value)
// {
//     std::lock_guard<std::recursive_mutex> lock(mutex_);

//     DP_LOG(WARNING) << "not implemented";
//     return kNotImplemented;
// }

// long FlashInterface::getAttribute(flash_attr_e attr)
// {
//     long r;

//     getAttribute(attr, r);
//     return r;
// }

bool FlashInterface::Identify()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (flash_info_->getInfo().IDNumber == 0) return true;
    auto id = ReadId() & FLASH_ID_MASK(flash_info_->getInfo().IDNumber);
    if (id == flash_info_->getInfo().JedecDeviceID) return true;
    return false;
}

uint32_t FlashInterface::ReadId()
{
    DpError rc;
    uint32_t id = 0;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = interface_->TransceiveOut(flash_info_->getInfo().RDIDCommand, true)) != kSc) return 0;
    if ((rc = interface_->TransceiveIn(reinterpret_cast<uint8_t *>(&id), flash_info_->getInfo().AddrWidth)) != kSc)
        return 0;
    return id;
}

DpError FlashInterface25::ReadStatus(uint16_t &status)
{
    DpError rc;
    uint8_t sr;
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if ((rc = interface_->TransceiveOut((uint8_t)kCmdReadStatus, true)) != kSc) return rc;
    if ((rc = interface_->TransceiveIn(sr)) != kSc) return rc;
    status = static_cast<uint16_t>(sr);
    return kSc;
}

DpError FlashInterface25::WriteEnable()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return interface_->TransceiveOut(kCmdWriteEnable);
}

DpError FlashInterface25::WriteDisable()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return interface_->TransceiveOut(kCmdWriteDisable);
}

DpError FlashInterface25::Unprotect(uint32_t retry)
{
    DpError rc;
    uint8_t status;
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    while (retry--)
    {
        if (!isProtected()) return kSc;
        if ((rc = WEL()) != kSc) return rc;
        uint8_t status[2] = {kCmdWriteStatus, 0};
        if ((rc = interface_->TransceiveOut(status, sizeof(status))) != kSc) return rc;
        if ((rc = WIP()) != kSc) return rc;
    }
    return kEpollErr;
}

bool FlashInterface25::isProtected()
{
    DpError rc;
    uint16_t status;
    if ((rc = ReadStatus(status)) != kSc) return true;
    return (status & kStatusBP) ? true : false;
}

DpError FlashInterface25::ChipErase()
{
    DpError rc;
    uint8_t status;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = WEL()) != kSc) return rc;
    if ((rc = interface_->TransceiveOut(kCmdChipErase)) != kSc) return rc;
    if ((rc = WIP(flash_info_->getInfo().Timeout)) != kSc) return rc;
    return kSc;
}

DpError FlashInterface25::BlockErase(uint32_t block_index, size_t count)
{
    DpError rc;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    while (count--)
    {
        uint8_t status;
        if ((rc = WEL()) != kSc) return rc;
        if ((status & kStatusWEL) == 0) return kInstrTimeout;
        if ((rc = interface_->TransceiveOut(kCmdBlockErase, true)) != kSc) return rc;
        uint32_t address = block_index * flash_info_->getInfo().SectorSizeInByte;
        if ((rc = interface_->TransceiveOut(reinterpret_cast<uint8_t *>(&address), flash_info_->getInfo().AddrWidth)) !=
            kSc)
            return rc;
        if ((rc = WIP(flash_info_->getInfo().Timeout)) != kSc) return rc;
        block_index++;
    }
    return kSc;
}

DpError FlashInterface25::PageProgram(uint32_t page_index, const void *buf, size_t size)
{
    DpError rc;
    assert(size <= flash_info_->getInfo().PageSizeInByte);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = WEL()) != kSc) return rc;
    // interface_->BulkOut();
    if ((rc = WIP()) != kSc) return rc;
    return kSc;
}

DpError FlashInterface25::PageRead(uint32_t page_index, void *buf, size_t size)
{
    DpError rc;
    assert(size + page_index * flash_info_->getInfo().PageSizeInByte <=
           flash_info_->getInfo().ChipSizeInKByte * 1024LL);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = WEL()) != kSc) return rc;
    // interface_->BulkIn();
    if ((rc = WIP()) != kSc) return rc;
    return kSc;
}

DpError FlashInterface25::Write(const void *buf, size_t size, uint64_t address)
{
    DpError rc;
    const uint8_t *p = static_cast<const uint8_t *>(buf);
    size_t remainder = flash_info_->getInfo().PageSizeInByte - size % flash_info_->getInfo().PageSizeInByte;
    uint32_t page_index = address / flash_info_->getInfo().PageSizeInByte;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = PreBatchOperation()) != kSc) return rc;
    while (size)
    {
        if ((rc = PageProgram(page_index, p, remainder)) != kSc) return rc;
        page_index++;
        p += remainder;
        size -= remainder;
        remainder = std::min(static_cast<size_t>(flash_info_->getInfo().PageSizeInByte), size);
    }
    if ((rc = PostBatchOperation()) != kSc) return rc;
    return kSc;
}

DpError FlashInterface25::Read(void *buf, size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface25::Verify(const void *buf, size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface25::BlankCheck(size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface25::WEL(uint32_t retry)
{
    DpError rc;
    uint16_t status = 0;
    while (retry--)
    {
        if ((rc = WriteEnable()) != kSc) return rc;
        if ((rc = ReadStatus(status)) != kSc) return rc;
        if (status & kStatusWEL) break;
    }
    return (status & kStatusWEL) ? kSc : kEpollErr;
}

DpError FlashInterface25::WIP(uint32_t timeout_usec)
{
    DpError rc;
    uint16_t status = 0;
    do
    {
        if ((rc = ReadStatus(status)) != kSc) return rc;
        if ((status & kStatusWIP) == 0) return kSc;
        std::this_thread::sleep_for(std::chrono::microseconds(std::min(timeout_usec, kSleepResolutionUsec_)));
        timeout_usec -= std::min(timeout_usec, kSleepResolutionUsec_);
    } while (timeout_usec);
    return kTimeout;
}

}  // namespace dp
