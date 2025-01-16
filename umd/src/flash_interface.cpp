#include "flash_interface.hpp"
#include "dp_logging.hpp"
#include "flash_database.hpp"
#include "flash_info.hpp"
#include "programmer.hpp"

#include <algorithm>
#include <chrono>
#include <thread>

namespace dp
{
/* static */ const uint32_t FlashInterface::kSleepResolutionUsec_ = 100;

// FlashInterface::FlashInterface(ProgrammerInterface &prog_interface, const FlashInfo *flash_info, int site_index)
//     : prog_interface_(prog_interface), flash_info_(flash_info), site_index_(site_index)
// {
// }
FlashInterface::FlashInterface(Programmer *prog_, int site_index)
    : prog_interface_(prog_->prog_interface_), flash_info_(prog_->flash_info_), site_index_(site_index)
{
}

DpError FlashInterface::setAttribute(flash_attr_e attr, long value)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    DP_LOG(WARNING) << "not implemented";
    return kNotImplemented;
}

DpError FlashInterface::getAttribute(flash_attr_e attr, long &value)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    DP_LOG(WARNING) << "not implemented";
    return kNotImplemented;
}

long FlashInterface::getAttribute(flash_attr_e attr)
{
    long r;

    getAttribute(attr, r);
    return r;
}

bool FlashInterface::Identify()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (flash_info_->getInfo().IDNumber == 0) return true;
    auto id = ReadId() & FLASH_ID_MASK(flash_info_->getInfo().IDNumber);
    if (id == flash_info_->getInfo().JedecDeviceID) return true;
    return false;
}

uint8_t FlashInterface::ReadStatus()
{
    DpError rc;
    uint8_t status;

    if ((rc = ReadStatus(status)) != kSc) return 0;
    return status;
}

DpError FlashInterface::ReadStatus(uint8_t &status)
{
    DpError rc;
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if ((rc = prog_interface_->TransferOut(kCmdReadStatus, kCsKeepLow)) != kSc) return rc;
    if ((rc = prog_interface_->TransferIn(status)) != kSc) return rc;
    return kSc;
}

DpError FlashInterface::WriteEnable()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return prog_interface_->TransferOut(kCmdWriteEnable);
}

DpError FlashInterface::WriteDisable()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return prog_interface_->TransferOut(kCmdWriteDisable);
}

DpError FlashInterface::Unprotect(uint32_t retry)
{
    DpError rc;
    uint8_t status;
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    while (retry--)
    {
        if (!isProtected()) return kSc;
        if ((rc = pollingWEL()) != kSc) return rc;
        uint8_t status[2] = {kCmdWriteStatus, 0};
        if ((rc = prog_interface_->TransferOut(status, sizeof(status))) != kSc) return rc;
        if ((rc = pollingWIP()) != kSc) return rc;
    }
    return kEpollErr;
}

bool FlashInterface::isProtected()
{
    DpError rc;
    uint8_t status;
    if ((rc = ReadStatus(status)) != kSc) return true;
    return (status & kStatusBP) ? true : false;
}

uint32_t FlashInterface::ReadId()
{
    DpError rc;
    uint32_t id = 0;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = prog_interface_->TransferOut(flash_info_->getInfo().RDIDCommand, kCsKeepLow)) != kSc) return 0;
    if ((rc = prog_interface_->TransferIn(reinterpret_cast<uint8_t *>(&id), flash_info_->getInfo().AddrWidth)) != kSc)
        return 0;
    return id;
}

DpError FlashInterface::ChipErase()
{
    DpError rc;
    uint8_t status;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((rc = pollingWEL()) != kSc) return rc;
    if ((rc = prog_interface_->TransferOut(kCmdChipErase)) != kSc) return rc;
    if ((rc = pollingWIP(flash_info_->getInfo().Timeout)) != kSc) return rc;
    return kSc;
}

DpError FlashInterface::BlockErase(uint32_t block_index, size_t count)
{
    DpError rc;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    while (count--)
    {
        uint8_t status;
        if ((rc = pollingWEL()) != kSc) return rc;
        if ((status & kStatusWEL) == 0) return kInstrTimeout;
        if ((rc = prog_interface_->TransferOut(kCmdBlockErase, kCsKeepLow)) != kSc) return rc;
        uint32_t address = block_index * flash_info_->getInfo().SectorSizeInByte;
        if ((rc = prog_interface_->TransferOut(reinterpret_cast<uint8_t *>(&address),
                                               flash_info_->getInfo().AddrWidth)) != kSc)
            return rc;
        if ((rc = pollingWIP(flash_info_->getInfo().Timeout)) != kSc) return rc;
        block_index++;
    }
    return kSc;
}

DpError FlashInterface::PageProgram(uint32_t page_index, const void *buf, size_t size)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface::PageRead(uint32_t page_index, void *buf, size_t size)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kReadErr;
}

DpError FlashInterface::Write(const void *buf, size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface::Read(void *buf, size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface::Verify(const void *buf, size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface::BlankCheck(size_t size, uint64_t address)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DP_LOG(ERROR) << ":not implemented";
    return kNotImplemented;
}

DpError FlashInterface::pollingWEL(uint32_t retry)
{
    DpError rc;
    uint8_t status = 0;
    while (retry--)
    {
        if ((rc = WriteEnable()) != kSc) return rc;
        if ((status = ReadStatus()) & kStatusWEL) break;
    }
    return (status & kStatusWEL) ? kSc : kEpollErr;
}

DpError FlashInterface::pollingWIP(uint32_t timeout_usec)
{
    do
    {
        if ((ReadStatus() & kStatusWIP) == 0) return kSc;
        std::this_thread::sleep_for(std::chrono::microseconds(std::min(timeout_usec, kSleepResolutionUsec_)));
        timeout_usec -= std::min(timeout_usec, kSleepResolutionUsec_);
    } while (timeout_usec);
    return kTimeout;
}

}  // namespace dp
