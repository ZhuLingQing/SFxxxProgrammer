#ifndef DUMMY_PROGRAMMER_HPP
#define DUMMY_PROGRAMMER_HPP

#include <cstdlib>  // getenv
#include "dp_config.h"
#include "flash_database.hpp"
#include "programmer.hpp"
#include "programmer_hal.hpp"
#include "programmer_interface.hpp"

namespace dp
{
class DummyHal : public ProgrammerHal
{
   public:
    DummyHal() : ProgrammerHal() {}
    int Control(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
                uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        return 0;
    }
    int ControlOut(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf, uint32_t size,
                   uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        return 0;
    }
    int ControlIn(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf, uint32_t size,
                  uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        return 0;
    }
    int BulkOut(void *buf, uint32_t size = 0, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) override
    {
        return 0;
    }
    int BulkIn(void *buf, uint32_t size = 0, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) override
    {
        return 0;
    }
};

class DummyProgInterface : public ProgrammerInterface
{
   public:
    DummyProgInterface(std::shared_ptr<ProgrammerHal> hal) : ProgrammerInterface(hal), result_in_(false)
    {
        const char *dummy_id = getenv("DUMMY_ID");
        jedec_id_ = std::stoi(dummy_id ? dummy_id : "0", nullptr, 16);
    }
    DpError PowerOn(DevPowerChan chan) override
    {
        DP_LOG(INFO) << getPowerConfig(chan) << "mV";
        return ProgrammerInterface::PowerOn(chan);
    }
    DpError PowerOff(DevPowerChan chan) override
    {
        DP_LOG(INFO) << getPowerConfig(chan) << "mV";
        return ProgrammerInterface::PowerOff(chan);
    }
    // DpError setPowerConfig(DevPowerChan chan, int mvolt) override
    // {
    //     DP_LOG(INFO) << "Chan" << chan << "<-" << mvolt << "mV";
    //     if (chan == kPowerVcc) vdd_ = mvolt;
    //     return ProgrammerInterface::setPowerConfig(chan, mvolt);
    // }
    DpError TransceiveIn(uint8_t *data, size_t size, bool result_in = false) override
    {
        if (0x9F == last_cmd_code_)
        {
            memcpy(data, &jedec_id_, size);
            DP_LOG(INFO) << "TransData: " << std::hex << (int)jedec_id_;
        }
        else
            DP_LOG(INFO) << "TransData: NONE";
        result_in_ = result_in;
        if (false == result_in) last_cmd_code_ = 0;
        return kSc;
    }
    DpError TransceiveOut(uint8_t *data, size_t size, bool result_in = false) override
    {
        if (result_in_ == false && size == 1 && result_in == true)
        {
            last_cmd_code_ = *data;
            DP_LOG(INFO) << "TransCmd: " << std::hex << (int)last_cmd_code_;
        }
        else
            last_cmd_code_ = 0;
        result_in_ = result_in;
        return kSc;
    }

   private:
    uint8_t last_cmd_code_;
    bool result_in_;
    uint32_t jedec_id_;
    uint32_t vdd_;
};

class DummyProgrammer : public Programmer
{
   public:
    DummyProgrammer(const std::string &db_file, std::shared_ptr<ProgrammerInterface> interface)
        : Programmer(db_file, interface)
    {
    }

   private:
    uint32_t CommonIdentifier(uint8_t cmd, uint8_t size) override
    {
        if (prog_interface_->getPowerConfig(kPowerVcc) < 3000) return 0;
        return Programmer::CommonIdentifier(cmd, size);
    }
};

}  // namespace dp

#endif  // #ifndef PROGRAMMER_HPP