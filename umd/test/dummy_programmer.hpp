#ifndef DUMMY_PROGRAMMER_HPP
#define DUMMY_PROGRAMMER_HPP

#include <cstdlib>  // getenv
#include "flash_database.hpp"
#include "programmer.hpp"

using FlashDatabase = dp::FlashDatabase;
using FlashInfo = dp::FlashInfo;

namespace dp
{
class DummyProgrammerInterface : public ProgrammerInterface
{
   public:
    DummyProgrammerInterface(void *handler) : ProgrammerInterface(handler), cs_(kCsHigh)
    {
        const char *dummy_id = getenv("DUMMY_ID");
        jedec_id_ = std::stoi(dummy_id ? dummy_id : "0", nullptr, 16);
    }

    DpError PowerOn() noexcept override
    {
        DP_LOG(INFO) << "" << vdd_ << "mV";
        return kSc;
    }
    DpError PowerOff() noexcept override
    {
        DP_LOG(INFO) << vdd_ << "mV";
        return kSc;
    }
    int getPowerConfig(DevPowerChan chan) noexcept override
    {
        DP_LOG(INFO) << "Chan" << chan << "->" << ((chan == kPwrVcc) ? vdd_ : 0) << "mV";
        if (chan == kPwrVcc) return vdd_;
        return 0;
    }
    DpError PowerConfig(DevPowerChan chan, int mvolt) noexcept override
    {
        DP_LOG(INFO) << "Chan" << chan << "<-" << mvolt << "mV";
        if (chan == kPwrVcc) vdd_ = mvolt;
        return kSc;
    }
    DpError TransferIn(uint8_t *data, size_t size, cs_pin_state_e cs = kCsHigh) noexcept
    {
        if (0x9F == last_cmd_code_) memcpy(data, &jedec_id_, size);
        cs_ = cs;
        if (cs_ == kCsHigh) last_cmd_code_ = 0;
        return kSc;
    }
    DpError TransferOut(const uint8_t *data, size_t size, cs_pin_state_e cs = kCsHigh) noexcept
    {
        if (cs_ == kCsHigh && size == 1 && cs == kCsKeepLow)
            last_cmd_code_ = *data;
        else
            last_cmd_code_ = 0;
        cs_ = cs;
        return kSc;
    }

   private:
    uint8_t last_cmd_code_;
    cs_pin_state_e cs_;
    uint32_t jedec_id_;
    uint32_t vdd_;
};

class DummyProgrammer : public Programmer
{
   public:
    DummyProgrammer(const std::string &db_file) : Programmer(db_file)
    {
        prog_interface_ = std::shared_ptr<DummyProgrammerInterface>(new DummyProgrammerInterface(nullptr));
    }

   private:
    uint32_t CommonIdentifier(uint8_t cmd, uint8_t size) override
    {
        if (prog_interface_->getPowerConfig(kPwrVcc) < 3000) return 0;
        return Programmer::CommonIdentifier(cmd, size);
    }
};
}  // namespace dp

#endif  // #ifndef PROGRAMMER_HPP