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
    ProgrammerInterface(std::shared_ptr<ProgrammerHal> hal) : hal_(hal), prog_type_(kProgUnknown)
    {
        memset(power_val_, 0, sizeof(power_val_));
        flash_series_ = kFlashSeries25;
        bus_clock_ = CONFIG_DEFAULT_BUS_CLOCK;
    }
    ~ProgrammerInterface() { Shutdown(); }
    virtual DpError Init(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT);
    virtual DpError Shutdown();
    virtual DpError Polling(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT);                // no new
    virtual DpError TransceiveIn(uint8_t *data, size_t size, bool result_in = false);  // override new
    DpError TransceiveIn(uint8_t &data, bool result_in = false) { return TransceiveIn(&data, 1, result_in); }
    virtual DpError TransceiveOut(uint8_t *data, size_t size, bool result_in = false);  // override new
    DpError TransceiveOut(uint8_t data, bool result_in = false) { return TransceiveOut(&data, 1, result_in); }
    virtual DpError PowerOn(DevPowerChan chan);   // override new
    virtual DpError PowerOff(DevPowerChan chan);  // override new
    virtual DpError setPowerConfig(DevPowerChan chan, int mvolt);
    int getPowerConfig(DevPowerChan chan) const
    {
        assert(chan < kPowerChanMax);
        return power_val_[chan];
    }
    void setFlashSeries(DevFlashSeries series) { flash_series_ = series; }
    DevFlashSeries getFlashSeries() const { return flash_series_; }
    virtual DpError setBusClock(int freq);  // no new
    int getBusClock() const { return bus_clock_; }
    virtual DpError setIO(uint32_t io_state);  // override new
    virtual uint32_t getIO() const { return io_state_; }
    virtual DpError setLED(programmer_led_e led_state);
    uint32_t getBoardUID() const { return board_uid_; }
    void Dump();

   protected:
    virtual DpError setCS(uint16_t value);                                 // no new, no called
    virtual DpError setIoMode(uint32_t io_mode = 0);                       // no new, no called
    virtual DpError setTargetFlash(target_flash_e mode);                   // no new
    virtual DpError ReadOnBoardFlash(uint8_t data[16], bool is_read_uid);  // override new
    virtual DpError WriteSF600UID(uint32_t uid, uint8_t manuID);           // no new
    virtual DpError setStandAlone(bool enable);                            // no new
    virtual DpError ReadFpgaVersion();                                     // no new
    virtual DpError ReadBoardUID();

    virtual DpError ReadDeviceInfo();  // no new
    static uint32_t FirmwareVersionConvert(uint32_t h, uint32_t m, uint32_t l) { return (h << 24) | (m << 16) | l; }
    virtual DpError StartAppli();  // no new
    std::shared_ptr<ProgrammerHal> hal_;

    std::string prog_name_;
    programmer_type_e prog_type_;
    std::string hardware_ver_;
    uint32_t firmware_ver_;
    uint32_t fpga_ver_;
    uint32_t board_uid_;
    uint32_t io1_selection_;
    uint32_t io4_selection_;
    bool is_new_command;
    uint32_t io_state_;

    int power_val_[kPowerChanMax];
    int bus_clock_;

    DevFlashSeries flash_series_;

    static const std::map<std::string, programmer_type_e> prog_type_map_;
    static const std::map<int, DevPowerVccSet> power_vcc_map_;
    static const std::map<int, DevBusClockSet> bus_clock_map_;
};  // class ProgrammerInterface

class ProgrammerInterfaceNew : public ProgrammerInterface
{
    friend class Programmer;

   public:
    DpError TransceiveIn(uint8_t *data, size_t size, bool result_in = false) override;
    DpError TransceiveOut(uint8_t *data, size_t size, bool result_in = false) override;
    DpError PowerOn(DevPowerChan chan) override;
    DpError PowerOff(DevPowerChan chan) override;
    DpError setIO(uint32_t io_state) override;

   protected:
    DpError ReadOnBoardFlash(uint8_t data[16], bool is_read_uid) override;
}

}  // namespace dp

#endif  // PROGRAMMER_INTERFACE_HPP