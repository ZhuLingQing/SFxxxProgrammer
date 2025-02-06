#include "sim_flash_mem.hpp"

#include <charconv>

std::string to_hex_string(uint8_t value)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << static_cast<int>(value);
    std::string hex_str = ss.str();
    if (hex_str.length() == 1)
    {
        hex_str = "0" + hex_str;  // 如果只有一位，添加前导0
    }
    return hex_str;
}

namespace sim
{
// https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/9/M25P20.pdf
// https://media.digikey.com/pdf/Data%20Sheets/Micron%20Technology%20Inc%20PDFs/DS_557_M25P16.pdf
// https://media.digikey.com/pdf/Data%20Sheets/Micron%20Technology%20Inc%20PDFs/M25P128.pdf
// 8bit status register:
// SRWD, 0, 0, BP2, BP1, BP0, WEL, WIP
// Instructions:
// WREN(0x06), WRDI(0x04), RDSR(0x05), WRSR(0x01), READ(0x03), FAST_READ(0x0B), PP(0x02), SE(0xD8), BE(0xC7),
// DP(0xB9), RDP(0xAB),

const size_t SimM25Pxx::kMaxIdCount_ = 20;

SimM25Pxx::SimM25Pxx(const dp::FlashInfo *info) : SimFlashMem<256>(info)
{
    handle_map_ = {
        {kCmdJedecId, &SimM25Pxx::HndReadIdentification},     {0x9E, &SimM25Pxx::HndReadIdentification},
        {kCmdReadStatus, &SimM25Pxx::HndReadStatusRegister},  {kCmdReadData, &SimM25Pxx::HndReadData},
        {kCmdFastReadData, &SimM25Pxx::HndReadDataHighSpeed},
    };
    execution_map_ = {
        {kCmdWriteEnable, &SimM25Pxx::ExeWriteEnable},
        {kCmdWriteDisable, &SimM25Pxx::ExeWriteDisable},
        {kCmdWriteStatus, &SimM25Pxx::ExeWriteStatusRegister},
        {kCmdPageProgram, &SimM25Pxx::ExePageProgram},
        {kCmdBlockErase, &SimM25Pxx::ExeBlockErase},
        {kCmdChipErase, &SimM25Pxx::ExeChipErase},
        {kCmdPowerDown, &SimM25Pxx::ExeDeepPowerDown},
        {kCmdReleasePowerDown, &SimM25Pxx::ExeReleaseDeepPowerDown},
    };
    id_.push_back(static_cast<uint8_t>(kMaxIdCount_));
    for (int i = 0; i < kMaxIdCount_; ++i) id_.push_back(static_cast<uint8_t>(i));

    uint8_t bp_code = 0;
    uint32_t sector_count = chip_size_ / sector_size_;
    uint32_t protect_sector_count = 1;
    while (protect_sector_count < sector_count)
    {
        protect_sector_map_.emplace(
            bp_code << 2, std::make_pair<uint32_t, uint32_t>(sector_count - protect_sector_count, sector_count - 1));
        protect_sector_count <<= 1;
        bp_code++;
    }
    while (bp_code < 8)
    {
        protect_sector_map_.emplace(bp_code << 2, std::make_pair<uint32_t, uint32_t>(0, sector_count - 1));
        bp_code++;
    }
}

bool SimM25Pxx::isStatusWriteEnable() { return (status_reg_ & kStatusSRWD) ? false : true; }

bool SimM25Pxx::isWriteEnabled(uint32_t sector_index, uint32_t sector_num)
{
    if ((status_reg_ & kStatusWEL) == 0) return false;
    auto range = protect_sector_map_.find(status_reg_ & kProtectMask);
    if (range == protect_sector_map_.end())
    {
        addMessage("isWriteEnabled: status_reg@" + std::to_string(status_reg_) + " out of range");
        return false;
    }
    auto sector_first = sector_index;
    auto sector_last = sector_index + sector_num - 1;
    auto protect_first = range->second.first;
    auto protect_last = range->second.second;
    if (sector_first < protect_first)
    {
        if (sector_last >= protect_first) return false;
    }
    else if (sector_first <= protect_last)
        return false;
    return true;
}

bool SimM25Pxx::isWriteInProgress() { return (status_reg_ & kStatusWIP) ? true : false; }

uint8_t SimM25Pxx::Handle()
{
    auto h = handle_map_.find(cmd_cache_[0]);
    if (h != handle_map_.end()) return (this->*(h->second))();
    return getDefaultPattern();
}

int SimM25Pxx::Execution()
{
    auto e = execution_map_.find(cmd_cache_[0]);
    if (e != execution_map_.end()) return (this->*(e->second))();
    addMessage("Execution: cmd@" + to_hex_string(cmd_cache_[0]) + " is not supported");
    return -1;
}

uint8_t SimM25Pxx::HndReadIdentification()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset < id_.size()) return id_[offset];
    addMessage("HndReadIdentification: offset@" + std::to_string(offset) + " is out of range");
    return 0;
}

uint8_t SimM25Pxx::HndReadStatusRegister()
{
    size_t offset = cmd_cache_.size() - 1;
    return static_cast<uint8_t>(status_reg_);
}

uint8_t SimM25Pxx::HndRead(size_t dummy)
{
    size_t offset = cmd_cache_.size() - 1;
    auto addr_width = info_->getInfo().AddrWidth + dummy;
    if (offset < addr_width)
        return 0;
    else if (offset == addr_width)
    {
        AddressConvert();
    }
    else
    {
        uint8_t data = getBlankPattern();
        auto page = data_.find(page_index_);
        if (page != data_.end())
        {
            data = page->second[page_offset_];
            page_offset_++;
            if (page_size_ == page_offset_)
            {
                page_offset_ = 0;
                page_index_++;
                if (page_index_ * page_size_ == chip_size_)
                {
                    page_index_ = 0;
                    addMessage("HndReadData: wrap");
                }
            }
        }
        return data;
    }
    return 0;
}

uint8_t SimM25Pxx::HndReadData() { return HndRead(0); }

uint8_t SimM25Pxx::HndReadDataHighSpeed() { return HndRead(1); }

int SimM25Pxx::ExeWriteEnable()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset)
    {
        addMessage("ExeWriteEnable: offset@" + std::to_string(offset) + " is out of range");
        return -1;
    }
    else if (!isStatusWriteEnable())
    {
        addMessage("ExeWriteEnable: Not Write Enabled");
        return 0;
    }
    else
    {
        status_reg_ |= kStatusWEL;
        return 0;
    }
}

int SimM25Pxx::ExeWriteDisable()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset == 0)
    {
        addMessage("ExeWriteDisable: offset@" + std::to_string(offset) + " is out of range");
        return -1;
    }
    else if (!isStatusWriteEnable())
    {
        addMessage("ExeWriteEnable: Not Write Enabled");
        return 0;
    }
    else
    {
        status_reg_ &= ~kStatusWEL;
        return 0;
    }
}

int SimM25Pxx::ExeWriteStatusRegister()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset != 1)
    {
        addMessage("ExeWriteDisable: offset@" + std::to_string(offset) + " is out of range");
        return -1;
    }
    else if (!isStatusWriteEnable())
    {
        addMessage("ExeWriteEnable: Not Write Enabled");
        return 0;
    }
    else
    {
        status_reg_ = cmd_cache_[1] & 0x1C;
        return 0;
    }
}

int SimM25Pxx::ExePageProgram()
{
    size_t offset = cmd_cache_.size() - 1;
    auto addr_width = info_->getInfo().AddrWidth;
    if (offset <= addr_width)
    {
        addMessage("ExePageProgram: offset@" + std::to_string(offset) + " too short");
        return 0;
    }
    AddressConvert();
    if (page_offset_ + cmd_cache_.size() - 1 - addr_width > page_size_)
    {
        addMessage("ExePageProgram: offset@" + std::to_string(offset) + " is out of range");
        return -1;
    }
    else if (!isWriteEnabled(address_ / sector_size_))
    {
        addMessage("ExePageProgram: Not Write Enabled");
        return 0;
    }
    else if (isWriteInProgress())
    {
        addMessage("ExePageProgram: isWriteInProgress");
        return -1;
    }
    page_buffer_.fill(getBlankPattern());
    std::copy(cmd_cache_.begin() + 1 + addr_width, cmd_cache_.end(), page_buffer_.begin() + page_offset_);
    status_reg_ |= kStatusWIP;
    return 0;
}
int SimM25Pxx::ExeBlockErase()
{
    size_t offset = cmd_cache_.size() - 1;
    auto addr_width = info_->getInfo().AddrWidth;
    if (offset != addr_width)
    {
        addMessage("ExeBlockErase: offset@" + std::to_string(offset) + " too short/long");
        return 0;
    }
    AddressConvert();
    if (!isWriteEnabled(address_ / sector_size_))
    {
        addMessage("ExeBlockErase: Not Write Enabled");
        return 0;
    }
    else if (isWriteInProgress())
    {
        addMessage("ExeBlockErase: isWriteInProgress");
        return -1;
    }
    status_reg_ |= kStatusWIP;
    return 0;
}
int SimM25Pxx::ExeChipErase()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset)
    {
        addMessage("ExeChipErase: offset@" + std::to_string(offset) + " too short/long");
        return 0;
    }
    if (!isWriteEnabled(0, chip_size_ / sector_size_))
    {
        addMessage("ExeChipErase: Not Write Enabled");
        return 0;
    }
    else if (isWriteInProgress())
    {
        addMessage("ExeChipErase: isWriteInProgress");
        return -1;
    }
    status_reg_ |= kStatusWIP;
    return 0;
}
int SimM25Pxx::ExeDeepPowerDown()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset)
    {
        addMessage("ExeDeepPowerDown: offset@" + std::to_string(offset) + " too short/long");
        return 0;
    }
    if (isWriteInProgress())
    {
        addMessage("ExeDeepPowerDown: isWriteInProgress");
        return -1;
    }
    state_ = kDeepPowerDown;
    return 0;
}
int SimM25Pxx::ExeReleaseDeepPowerDown()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset)
    {
        addMessage("ExeReleaseDeepPowerDown: offset@" + std::to_string(offset) + " too short/long");
        return 0;
    }
    state_ = kReady;
    return 0;
}

SimFlash *SimFlashFactory(const dp::FlashInfo *info)
{
    if (info->getClass() == dp::kClass_M25Pxx) return new SimM25Pxx(info);
    std::cerr << "SimFlashFactory::" << info->getName() << "::Unsupported class@" << info->getInfo().Class << std::endl;
    return nullptr;
}

}  // namespace sim