#ifndef SIM_FLASH_MEM_HPP
#define SIM_FLASH_MEM_HPP

#include <array>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "dp_config.h"
#include "dp_error.h"
#include "dp_spi_flash_command_set.h"
#include "dp_type.h"
#include "flash_info.hpp"
#include "hal_proto.pb.h"
#include "hal_skt_device.hpp"

static inline std::string to_hex_string(uint8_t value)
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
using protect_range_t = std::pair<uint32_t, uint32_t>;

template <typename T>
class SimStatusRegister
{
   public:
    SimStatusRegister(T value = 0) : value_(value) {}
    T get() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }
    void set(T value)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ |= value;
    }
    void clear(T value)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ &= ~value;
    }
    void write(T value)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ = value;
    }

   private:
    mutable std::shared_mutex mutex_;
    T value_;
};

class SimSpiFlash
{
   public:
    enum state_e
    {
        kPowerOff = 0,
        kDeepPowerDown,
        kReady,
        kError,
    };
    SimSpiFlash(const dp::FlashInfo *info) : info_(info), enable_dump_(false)
    {
        assert(info_);
        Reset();
    }
    virtual void Reset()
    {
        state_ = kPowerOff;
        power_vcc_ = 0;
        power_vpp_ = 0;
        clock_mhz_ = 12;
        address_ = 0;
        message_.clear();
        id_.clear();
        cmd_cache_.clear();
        rsp_cache_.clear();
        if (exe_thread_.joinable()) exe_thread_.join();
        if (info_)
        {
            auto info = info_->getInfo();
            chip_size_ = info_->getInfo().ChipSizeInKByte * 1024LL;
            sector_size_ = info_->getInfo().SectorSizeInByte;
            page_size_ = info_->getInfo().PageSizeInByte;
            if (info.IDNumber)
            {
                for (int shift = info.IDNumber - 1; shift >= 0; shift--)
                    id_.push_back(static_cast<uint8_t>(info.JedecDeviceID >> (8 * shift)));
            }
        }
    }
    virtual int BackdoorWrite(uint64_t address, const uint8_t *data, size_t size) = 0;
    virtual int BackdoorRead(uint64_t address, uint8_t *data, size_t size) = 0;
    virtual int BackdoorBlankCheck(uint64_t address, size_t size) = 0;
    virtual int BackdoorErase(uint32_t sector_index, size_t sector_num) = 0;

    int Transfer(size_t dummy, std::vector<uint8_t> &resp, bool cs_high = true)
    {
        resp.clear();
        for (size_t i = 0; i < dummy; i++)
        {
            resp.push_back(Transceive(getDefaultPattern()));
        }
        if (cs_high)
        {
            return CsRisingEdge();
        }
        else
        {
            if (HardwareCheck()) return -1;
        }
        return 0;
    }
    int Transfer(const std::vector<uint8_t> &payload, std::vector<uint8_t> &resp, bool cs_high = true)
    {
        resp.clear();
        for (auto x : payload)
        {
            resp.push_back(Transceive(x));
        }
        if (cs_high)
        {
            return CsRisingEdge();
        }
        else
        {
            if (HardwareCheck()) return -1;
        }
        return 0;
    }
    void setVcc(int vcc)
    {
        power_vcc_ = vcc;
        if (0 == vcc)
            state_ = kPowerOff;
        else if (state_ == kPowerOff || state_ == kError)
        {
            state_ = kReady;
        }
        else
        {
            std::cout << "SimFlashMem::setVcc: @" << state_ << std::endl;
        }
    }
    virtual void setWP(bool wp) {}
    void setVpp(int vpp) { power_vpp_ = vpp; }
    void setClock(int clock) { clock_mhz_ = clock; }
    state_e getState() const { return state_; }
    const dp::FlashInfo *getInfo() const { return info_; }
    void addMessage(const std::string s) { message_ += "[SIM] " + s + "\n"; }
    void clearMessage() { message_.clear(); }
    const std::string &getMessage() const { return message_; }

   private:
    uint8_t Transceive(uint8_t data)
    {
        uint8_t rdata = getDefaultPattern();
        cmd_cache_.push_back(data);
        if (cmd_cache_.size() > 1) rdata = Handle();
        rsp_cache_.push_back(rdata);
        return rdata;
    }
    int CsRisingEdge()
    {
        Execution();
        cmd_cache_.clear();
        rsp_cache_.clear();
        if (getMessage().size())
        {
            std::cerr << getMessage();
            clearMessage();
        }
        return 0;
    }

   protected:
    virtual bool isStatusWriteDisabled() { return false; }
    virtual bool isWriteEnabled(uint32_t sector_index, uint32_t sector_num = 1) { return false; }
    virtual bool isWriteInProgress() { return false; }
    virtual uint8_t getDefaultPattern() const { return 0; }
    virtual uint8_t getBlankPattern() const { return 0xFF; }
    virtual uint8_t getReleaseDeepPowerDownCmd() const { return kCmdReleasePowerDown; }
    virtual uint8_t Handle()
    {
        std::cerr << "Unimplemented Transfer" << std::endl;
        return 0;
    }
    virtual int Execution() = 0;
    void enableDump(bool enable) { enable_dump_ = enable; }
    void Dump()
    {
        if (!enable_dump_) return;
        std::cout << "SimFlashMem(0x" << to_hex_string(cmd_cache_[0]) << "):mosi/miso";
        for (size_t i = 0; i < cmd_cache_.size(); ++i)
        {
            if (i % 8 == 0) std::cout << std::endl << "\t";
            std::cout << to_hex_string(cmd_cache_[i]) << "/" << to_hex_string(rsp_cache_[i]) << " ";
        }
        std::cout << std::endl;
    }
    uint32_t getPageIndex(uint64_t address) const { return address / page_size_; }
    uint32_t getSectorIndex(uint64_t address) const { return address / sector_size_; }
    void AddressConvert()
    {
        address_ = 0;
        for (size_t i = 0; i < info_->getInfo().AddrWidth; ++i)
        {
            address_ <<= 8;
            address_ |= cmd_cache_[i + 1];
        }
        page_offset_ = address_ % page_size_;
        page_index_ = address_ / page_size_;
    }
    int HardwareCheck(uint64_t address = 0, size_t size = 0)
    {
        if (state_ != kReady)
        {
            if (!(state_ == kDeepPowerDown && cmd_cache_.size() && cmd_cache_[0] == getReleaseDeepPowerDownCmd()))
            {
                std::cerr << "SimFlashMem::HardwareCheck: state@" << state_ << std::endl;
                return -1;
            }
        }
        if (address + size > chip_size_)
        {
            std::cerr << "SimFlashMem::HardwareCheck: size@" << address + size << std::endl;
            return -1;
        }
        if (power_vcc_ != info_->getInfo().Voltage)
        {
            std::cerr << "SimFlashMem::HardwareCheck: vcc@" << power_vcc_ << std::endl;
            return -1;
        }
        if (clock_mhz_ == 0 || clock_mhz_ > info_->getInfo().Clock)
        {
            std::cerr << "SimFlashMem::HardwareCheck: clock@" << clock_mhz_ << std::endl;
            return -1;
        }
        return 0;
    }
    const dp::FlashInfo *info_;
    bool enable_dump_;
    state_e state_;
    int power_vcc_;
    int power_vpp_;
    int clock_mhz_;
    uint32_t page_offset_;
    uint32_t page_index_;
    uint64_t address_;
    size_t chip_size_;
    size_t sector_size_;
    size_t page_size_;
    std::vector<uint8_t> id_;
    std::vector<uint8_t> cmd_cache_;
    std::vector<uint8_t> rsp_cache_;
    std::string message_;

    virtual void executionChipErase() {}
    virtual void executionSectorErase(uint32_t sector_index) {}
    virtual void executionPageProgram(uint32_t page_index) {}
    mutable std::thread exe_thread_;
    static const long kPageProgramMs_;
    static const long kSectorEraseMs_;
    static const long kChipEraseMs_;
};

template <typename SR, size_t kPageSize>
class SimFlashMem : public SimSpiFlash
{
   public:
    using page_mem_t = std::array<uint8_t, kPageSize>;
    using sector_mem_t = std::map<uint32_t, page_mem_t>;
    using chip_mem_t = std::map<uint32_t, sector_mem_t>;
    SimFlashMem(const dp::FlashInfo *info) : SimSpiFlash(info) {}
    void Reset() override
    {
        SimSpiFlash::Reset();
        mem_.clear();
    }
    int BackdoorWrite(uint64_t address, const uint8_t *data, size_t size) override
    {
        if (HardwareCheck(address, size)) return -1;
        if (isWriteInProgress()) return -1;

        auto first_page = getPageIndex(address);
        auto last_page = getPageIndex(address + size - 1);
        auto offset = address % page_size_;
        for (auto page = first_page; page <= last_page; ++page)
        {
            auto in_page_size = std::min(size, page_size_ - offset);
            auto page_mem = getPage(page);
            std::copy(data, data + in_page_size, page_mem.begin() + offset);
            if (!isBlankPage(page_mem)) PageDataReplace(page, page_mem);
            data += in_page_size;
            size -= in_page_size;
            offset = 0;
        }
        return 0;
    }
    int BackdoorRead(uint64_t address, uint8_t *data, size_t size) override
    {
        if (HardwareCheck(address, size)) return -1;
        if (isWriteInProgress()) return -1;

        auto first_page = getPageIndex(address);
        auto last_page = getPageIndex(address + size - 1);
        auto offset = address % page_size_;
        for (auto page = first_page; page <= last_page; ++page)
        {
            auto in_page_size = std::min(size, page_size_ - offset);
            auto mem = getPage(page);
            std::copy(mem.begin() + offset, mem.begin() + offset + in_page_size, data);
            data += in_page_size;
            size -= in_page_size;
            offset = 0;
        }
        return 0;
    }
    int BackdoorBlankCheck(uint64_t address, size_t size) override
    {
        if (HardwareCheck(address, size)) return -1;
        if (isWriteInProgress()) return -1;

        int not_blank_pages = 0;
        auto first_sector = getSectorIndex(address);
        auto last_sector = getPageIndex(address + size - 1);
        for (auto sct = first_sector; sct <= last_sector; ++sct)
        {
            auto sector = mem_.find(sct);
            if (sector != mem_.end()) not_blank_pages += sector->second.size();
        }
        return not_blank_pages;
    }
    int BackdoorErase(uint32_t sector_index, size_t sector_num) override
    {
        if (HardwareCheck(sector_index * sector_size_, sector_num * sector_size_)) return -1;
        if (isWriteInProgress()) return -1;

        while (sector_num--)
        {
            auto sector = mem_.find(sector_index);
            if (sector != mem_.end())
            {
                mem_.erase(sector);
            }
            sector_index++;
        }
        return 0;
    }
    page_mem_t getPage(uint32_t page_index)
    {
        auto sector_index = page_index / (sector_size_ / page_size_);
        // check sector exist
        auto sector = mem_.find(sector_index);
        if (sector == mem_.end()) return page_mem_t({getBlankPattern()});
        // check page exist
        auto page = sector->second.find(page_index);
        if (page == sector->second.end()) return page_mem_t({getBlankPattern()});
        return page->second;
    }
    std::optional<page_mem_t> TryGetPage(uint32_t page_index)
    {
        auto sector_index = page_index / (sector_size_ / page_size_);
        // check sector exist
        auto sector = mem_.find(sector_index);
        if (sector == mem_.end()) return std::nullopt;
        // check page exist
        auto page = sector->second.find(page_index);
        if (page == sector->second.end()) return std::nullopt;
        return page->second;
    }

   protected:
    bool isBlankPage(page_mem_t &page_mem)
    {
        // return std::all_of(page_mem.begin(), page_mem.end(), [](uint8_t x) { return x == 0xFF; });
        return page_mem == page_mem_t({getBlankPattern()});
    }
    void PageDataReplace(uint32_t page_index, page_mem_t &page_mem)
    {
        auto sector_index = page_index / (sector_size_ / page_size_);
        auto sector = mem_.find(sector_index);
        if (sector == mem_.end())
        {  // sector not exist
            sector_mem_t sector_mem;
            sector_mem.insert(std::make_pair(page_index, page_mem));
            mem_.insert(std::make_pair(sector_index, sector_mem));
        }
        else
        {
            auto page = sector->second.find(page_index);
            if (page == sector->second.end())  // page not exist
                sector->second.insert(std::make_pair(page_index, page_mem));
            else
                page->second = page_mem;
        }
    }
    SimStatusRegister<SR> status_reg_;
    chip_mem_t mem_;
    page_mem_t page_mem_;
};

class SimM25Pxx : public SimFlashMem<uint8_t, 256>
{
   public:
    SimM25Pxx(const dp::FlashInfo *info);

    void setWP(bool wp) override { wp ? status_reg_.set(kStatusSRWD) : status_reg_.clear(kStatusSRWD); }

   protected:
    bool isStatusWriteDisabled() override;
    bool isWriteEnabled(uint32_t sector_index, uint32_t sector_num = 1) override;
    bool isWriteInProgress() override;
    uint8_t Handle() override;
    int Execution() override;

    uint8_t HndReadIdentification();
    uint8_t HndReadStatusRegister();
    uint8_t HndReadData();
    uint8_t HndReadDataHighSpeed();
    uint8_t HndRead(size_t dummy = 1);

    int ExeWriteEnable();
    int ExeWriteDisable();
    int ExeWriteStatusRegister();
    int ExePageProgram();
    int ExeSectorErase();
    int ExeChipErase();
    int ExeDeepPowerDown();
    int ExeReleaseDeepPowerDown();

    static const size_t kMaxIdCount_;
    std::map<uint8_t, uint8_t (SimM25Pxx::*)()> handle_map_;
    std::map<uint8_t, int (SimM25Pxx::*)()> execution_map_;
    std::map<uint16_t, protect_range_t> protect_sector_map_;

    void executionChipErase() override;
    void executionSectorErase(uint32_t sector_index) override;
    void executionPageProgram(uint32_t page_index) override;
};

SimSpiFlash *SimFlashFactory(const dp::FlashInfo *info);

}  // namespace sim

#endif  // SIM_FLASH_MEM_HPP