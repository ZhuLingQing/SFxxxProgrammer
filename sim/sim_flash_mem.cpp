#include "sim_flash_mem.hpp"

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

const long SimSpiFlash::kPageProgramMs_ = CONFIG_PAGE_PROGRAM_MS;
const long SimSpiFlash::kSectorEraseMs_ = CONFIG_SECTOR_ERASE_MS;
const long SimSpiFlash::kChipEraseMs_ = CONFIG_CHIP_ERASE_MS;

const size_t SimM25Pxx::kMaxIdCount_ = 16;

SimM25Pxx::SimM25Pxx(const dp::FlashInfo *info) : SimFlashMem<uint8_t, 256>(info)
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
        {kCmdBlockErase, &SimM25Pxx::ExeSectorErase},
        {kCmdChipErase, &SimM25Pxx::ExeChipErase},
        {kCmdPowerDown, &SimM25Pxx::ExeDeepPowerDown},
        {kCmdReleasePowerDown, &SimM25Pxx::ExeReleaseDeepPowerDown},
    };
    id_.push_back(static_cast<uint8_t>(kMaxIdCount_));
    for (int i = 0; i < kMaxIdCount_; ++i) id_.push_back(static_cast<uint8_t>(i));

    uint8_t bp_code = 1;
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

bool SimM25Pxx::isStatusWriteDisabled() { return (status_reg_.get() & kStatusSRWD) ? true : false; }

bool SimM25Pxx::isWriteEnabled(uint32_t sector_index, uint32_t sector_num)
{
    if ((status_reg_.get() & kStatusWEL) == 0) return false;
    auto range = protect_sector_map_.find(status_reg_.get() & kProtectMask);
    if (range == protect_sector_map_.end()) return true;
    auto sector_first = sector_index;
    auto sector_last = sector_index + sector_num - 1;
    auto protect_first = range->second.first;
    auto protect_last = range->second.second;
    if (sector_first > protect_last || sector_last < protect_first) return true;
    addMessage("StatusRegister: @" + to_hex_string(status_reg_.get()));
    addMessage("Protected: @" + std::to_string(protect_first) + " ~ " + std::to_string(protect_last) + ". Access: @" +
               std::to_string(sector_first) + " ~ " + std::to_string(sector_last));
    return false;
}

bool SimM25Pxx::isWriteInProgress()
{
    if (status_reg_.get() & kStatusWIP) return true;
    if (exe_thread_.joinable()) exe_thread_.join();
    return false;
}

uint8_t SimM25Pxx::Handle()
{
    auto h = handle_map_.find(cmd_cache_[0]);
    if (h != handle_map_.end()) return (this->*(h->second))();
    return getDefaultPattern();
}

int SimM25Pxx::Execution()
{
    int r = 0;
    auto e = execution_map_.find(cmd_cache_[0]);
    if (e != execution_map_.end()) r = (this->*(e->second))();
    // else if (handle_map_.find(cmd_cache_[0]) == handle_map_.end())
    else if (0 == handle_map_.count(cmd_cache_[0]))
    {
        addMessage("Execution: unimplemented cmd@" + to_hex_string(cmd_cache_[0]));
    }
    SimSpiFlash::Dump();
    return r;
}

uint8_t SimM25Pxx::HndReadIdentification()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset <= id_.size()) return id_[offset - 1];
    addMessage("HndReadIdentification: offset@" + std::to_string(offset) + " is out of range");
    return getDefaultPattern();
}

uint8_t SimM25Pxx::HndReadStatusRegister()
{
    size_t offset = cmd_cache_.size() - 1;
    return static_cast<uint8_t>(status_reg_.get());
}

uint8_t SimM25Pxx::HndRead(size_t dummy)
{
    size_t offset = cmd_cache_.size() - 1;
    auto addr_width = info_->getInfo().AddrWidth + dummy;
    if (offset < addr_width)
        return getDefaultPattern();
    else if (offset == addr_width)
    {
        AddressConvert();
        page_mem_ = getPage(page_index_);
    }
    else
    {
        uint8_t data = page_mem_[page_offset_];
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
            page_mem_ = getPage(page_index_);
        }
        return data;
    }
    return getDefaultPattern();
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
    else if (isStatusWriteDisabled())
    {
        addMessage("ExeWriteEnable: Not Write Enabled");
        return 0;
    }
    else
    {
        status_reg_.set(kStatusWEL);
        return 0;
    }
}

int SimM25Pxx::ExeWriteDisable()
{
    size_t offset = cmd_cache_.size() - 1;
    if (offset)
    {
        addMessage("ExeWriteDisable: offset@" + std::to_string(offset) + " is out of range");
        return -1;
    }
    else if (isStatusWriteDisabled())
    {
        addMessage("ExeWriteEnable: Not Write Enabled");
        return 0;
    }
    else
    {
        status_reg_.clear(kStatusWEL);
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
    else if (isStatusWriteDisabled())
    {
        addMessage("ExeWriteEnable: Not Write Enabled");
        return 0;
    }
    else
    {
        status_reg_.write(cmd_cache_[1] & kProtectMask);
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
    page_mem_ = getPage(page_index_);
    std::copy(cmd_cache_.begin() + 1 + addr_width, cmd_cache_.end(), page_mem_.begin() + page_offset_);
    status_reg_.set(kStatusWIP);
    exe_thread_ = std::thread(&SimM25Pxx::executionPageProgram, this, page_index_);
    return 0;
}

int SimM25Pxx::ExeSectorErase()
{
    size_t offset = cmd_cache_.size() - 1;
    auto addr_width = info_->getInfo().AddrWidth;
    if (offset != addr_width)
    {
        addMessage("ExeSectorErase: offset@" + std::to_string(offset) + " too short/long");
        return 0;
    }
    AddressConvert();
    if (!isWriteEnabled(address_ / sector_size_))
    {
        addMessage("ExeSectorErase: Not Write Enabled");
        return 0;
    }
    else if (isWriteInProgress())
    {
        addMessage("ExeSectorErase: isWriteInProgress");
        return -1;
    }
    status_reg_.set(kStatusWIP);
    exe_thread_ = std::thread(&SimM25Pxx::executionSectorErase, this, address_ / sector_size_);
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
    status_reg_.set(kStatusWIP);
    exe_thread_ = std::thread(&SimM25Pxx::executionChipErase, this);
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

void SimM25Pxx::executionChipErase()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(kPageProgramMs_));
    if (!mem_.empty())
    {
        std::this_thread::sleep_for(std::chrono::seconds(kChipEraseMs_));
    }
    mem_.clear();
    status_reg_.clear(kStatusWIP | kStatusWEL);
}

void SimM25Pxx::executionSectorErase(uint32_t sector_index)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(kPageProgramMs_));
    auto sector = mem_.find(sector_index);
    if (sector != mem_.end())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(kSectorEraseMs_));
        mem_.erase(sector);
    }
    status_reg_.clear(kStatusWIP | kStatusWEL);
}

void SimM25Pxx::executionPageProgram(uint32_t page_index)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(kPageProgramMs_));
    if (!isBlankPage(page_mem_)) PageDataReplace(page_index, page_mem_);
    status_reg_.clear(kStatusWIP | kStatusWEL);
}

SimSpiFlash *SimFlashFactory(const dp::FlashInfo *info)
{
    if (info->getClass() == dp::kClass_M25Pxx) return new SimM25Pxx(info);
    std::cerr << "SimFlashFactory::" << info->getName() << "::Unsupported class@" << info->getInfo().Class << std::endl;
    return nullptr;
}

}  // namespace sim