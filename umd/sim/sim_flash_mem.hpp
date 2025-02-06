#ifndef SIM_FLASH_MEM_HPP
#define SIM_FLASH_MEM_HPP

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "dp_config.h"
#include "dp_error.h"
#include "dp_spi_flash_command_set.h"
#include "dp_type.h"
#include "flash_info.hpp"
#include "hal_proto.pb.h"
#include "hal_skt_device.hpp"

namespace sim
{
using protect_range_t = std::pair<uint32_t, uint32_t>;

class SimFlash
{
   public:
    enum state_e
    {
        kPowerOff = 0,
        kDeepPowerDown,
        kReady,
        kError,
    };
    SimFlash(const dp::FlashInfo *info) : info_(info)
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
        status_reg_ = 0;
        message_.clear();
        id_.clear();
        cmd_cache_.clear();
        rsp_cache_.clear();
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
    virtual int BackdoorWrite(uint64_t address, const uint8_t *data, size_t size, bool force = false) = 0;
    virtual int BackdoorRead(uint64_t address, uint8_t *data, size_t size) = 0;

    int Transfer(const std::vector<uint8_t> &payload, std::vector<uint8_t> &resp, bool cs_high = true)
    {
        resp.clear();
        for (auto x : payload)
        {
            uint8_t data = getDefaultPattern();
            cmd_cache_.push_back(x);
            if (cmd_cache_.size() > 1) data = Handle();
            rsp_cache_.push_back(data);
            resp.push_back(data);
        }
        if (cs_high)
        {
            Execution();
            cmd_cache_.clear();
            rsp_cache_.clear();
            std::cerr << getMessage() << std::endl;
            clearMessage();
        }
        else
        {
            if (Check()) return -1;
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
    void setVpp(int vpp) { power_vpp_ = vpp; }
    void setClock(int clock) { clock_mhz_ = clock; }
    state_e getState() const { return state_; }
    const dp::FlashInfo *getInfo() const { return info_; }
    // void addMessage(const char *s) { message_ += s; }
    void addMessage(const std::string s) { message_ += s; }
    // void addMessage(std::stringstream &ss) { message_ += ss.str(); }
    void clearMessage() { message_.clear(); }
    const std::string &getMessage() const { return message_; }

   protected:
    virtual bool isStatusWriteEnable() { return true; }
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
    virtual int Execution()
    {
        std::cout << "SimFlashMem(0x" << std::hex << static_cast<int>(cmd_cache_[0]) << "):mosi/miso" << std::endl;
        for (size_t i = 0; i < chip_size_; ++i)
        {
            std::cout << std::hex << static_cast<int>(cmd_cache_[i]) << "/" << static_cast<int>(cmd_cache_[i]) << " ";
            if (i && (i % 8 == 0))
            {
                std::cout << std::endl;
            }
        }
        return 0;
    }
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
    int Check(uint64_t address = 0, size_t size = 0)
    {
        if (state_ != kReady)
        {
            if (!(state_ == kDeepPowerDown && cmd_cache_.size() && cmd_cache_[0] == getReleaseDeepPowerDownCmd()))
            {
                std::cerr << "SimFlashMem::Check: state@" << state_ << std::endl;
                return -1;
            }
        }
        if (address + size > chip_size_)
        {
            std::cout << "SimFlashMem::Check: size@" << address + size << std::endl;
            return -1;
        }
        if (power_vcc_ != info_->getInfo().Voltage)
        {
            std::cout << "SimFlashMem::Check: vcc@" << power_vcc_ << std::endl;
            return -1;
        }
        if (clock_mhz_ && clock_mhz_ <= info_->getInfo().Clock)
        {
            std::cout << "SimFlashMem::Check: clock@" << clock_mhz_ << std::endl;
            return -1;
        }
        return 0;
    }
    const dp::FlashInfo *info_;
    state_e state_;
    int power_vcc_;
    int power_vpp_;
    int clock_mhz_;
    uint32_t page_offset_;
    uint32_t page_index_;
    uint64_t address_;
    uint16_t status_reg_;
    size_t chip_size_;
    size_t sector_size_;
    size_t page_size_;
    std::vector<uint8_t> id_;
    std::vector<uint8_t> cmd_cache_;
    std::vector<uint8_t> rsp_cache_;
    std::string message_;
};

template <size_t kPageSize>
class SimFlashMem : public SimFlash
{
   public:
    SimFlashMem(const dp::FlashInfo *info) : SimFlash(info) {}
    void Reset() override
    {
        SimFlash::Reset();
        data_.clear();
    }
    int BackdoorWrite(uint64_t address, const uint8_t *data, size_t size, bool force = false) override
    {
        if (Check(address, size)) return -1;
        if (!force)
        {
            for (size_t i = 0; i < size; i++) data_[address + i] = data[i];
        }
        else
        {
            for (size_t i = 0; i < size; i++) data_[address + i] &= data[i];
        }
        return 0;
    }
    int BackdoorRead(uint64_t address, uint8_t *data, size_t size) override
    {
        if (Check(address, size)) return -1;
        for (size_t i = 0; i < size; i++) data[i] = data_[address + i];
        return 0;
    }

   protected:
    std::map<uint64_t, std::array<kPageSize>> data_;
    std::array<kPageSize> page_buffer_;
};

class SimM25Pxx : public SimFlashMem<256>
{
   public:
    SimM25Pxx(const dp::FlashInfo *info);

   protected:
    bool isStatusWriteEnable() override;
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
    int ExeBlockErase();
    int ExeChipErase();
    int ExeDeepPowerDown();
    int ExeReleaseDeepPowerDown();

    static const size_t kMaxIdCount_;
    std::map<uint8_t, uint8_t (SimM25Pxx::*)()> handle_map_;
    std::map<uint8_t, int (SimM25Pxx::*)()> execution_map_;
    std::map<uint16_t, protect_range_t> protect_sector_map_;
};

SimFlash *SimFlashFactory(const dp::FlashInfo *info);

}  // namespace sim

#endif  // SIM_FLASH_MEM_HPP