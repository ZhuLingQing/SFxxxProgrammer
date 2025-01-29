#ifndef PROGRAMMER_INTERFACE_HPP
#define PROGRAMMER_INTERFACE_HPP

#include "dp_config.h"
#include "dp_error.h"
#include "dp_logging.hpp"
#include "dp_type.h"

#include <map>
#include <memory>

namespace dp
{
class ProgrammerHal;

class ProgrammerInterface
{
    friend class Programmer;

   public:
    enum programmer_type_e
    {
        kProgUnknown,
        kProgSimulator,
        kProgSF100,
        kProgSF600,
        kProgSF700,
        kProgSF600G2
    };
    ProgrammerInterface(std::shared_ptr<ProgrammerHal> hal) : hal_(hal), prog_type_(kProgUnknown)
    {
        memset(power_val_, 0, sizeof(power_val_));
        flash_series_ = kFlashSeries25;
        bus_clock_ = CONFIG_DEFAULT_BUS_CLOCK;
    }
    ~ProgrammerInterface() { Shutdown(); }
    virtual DpError Init(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT);
    virtual DpError Shutdown();
    virtual DpError Polling(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT);
    virtual DpError TransceiveIn(uint8_t *data, size_t size, bool result_in = false);
    DpError TransceiveIn(uint8_t &data, bool result_in = false) { return TransceiveIn(&data, 1, result_in); }
    virtual DpError TransceiveOut(uint8_t *data, size_t size, bool result_in = false);
    DpError TransceiveOut(uint8_t data, bool result_in = false) { return TransceiveOut(&data, 1, result_in); }
    virtual DpError PowerOn(DevPowerChan chan);
    virtual DpError PowerOff(DevPowerChan chan);
    virtual DpError setPowerConfig(DevPowerChan chan, int mvolt);
    int getPowerConfig(DevPowerChan chan) const
    {
        assert(chan < kPowerChanMax);
        return power_val_[chan];
    }
    void setFlashSeries(DevFlashSeries series) { flash_series_ = series; }
    DevFlashSeries getFlashSeries() const { return flash_series_; }
    virtual DpError setBusClock(int freq);
    int getBusClock() const { return bus_clock_; }
    virtual DpError setIO(uint32_t io_state) { return kSc; }    // TODO
    virtual uint32_t getIO() { return 0; }                      // TODO
    virtual DpError setLED(uint32_t led_state) { return kSc; }  // TODO
    virtual uint32_t getLED() { return 0; }                     // TODO

   protected:
    static uint32_t FirmwareVersionConvert(uint32_t h, uint32_t m, uint32_t l) { return (h << 24) | (m << 16) | l; }
    // int StartAppli();
    // int AssignProg();
    // int LeaveStandaloneMode();
    // int QueryBoard();
    // int CheckProgrammerInfo();
   private:
    std::shared_ptr<ProgrammerHal> hal_;
    programmer_type_e prog_type_;
    std::string hardware_id_;
    uint32_t firmware_ver_;
    uint32_t io1_selection_;
    uint32_t io4_selection_;
    uint32_t is_new_command;

    int power_val_[kPowerChanMax];
    int bus_clock_;

    DevFlashSeries flash_series_;

    static const std::map<int, DevPowerVccSet> power_vcc_map_;
    static const std::map<int, DevBusClockSet> bus_clock_map_;
};  // class ProgrammerInterface

}  // namespace dp

#endif  // PROGRAMMER_INTERFACE_HPP