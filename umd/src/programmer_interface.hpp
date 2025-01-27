#ifndef PROGRAMMER_INTERFACE_HPP
#define PROGRAMMER_INTERFACE_HPP

#include "programmer_hal.hpp"
#include "dp_config.h"
#include "dp_error.h"
#include "dp_logging.hpp"
#include "dp_type.h"

#include <memory>

namespace dp
{
class ProgrammerInterface
{
    friend class Programmer;

   public:
    enum programmer_type_e
    {
        kProgUnknown,
        kProgSF100,
        kProgSF600,
        kProgSF700,
        kProgSF600G2
    };
    ProgrammerInterface(const std::shared_ptr<ProgrammerHal> &hal) : hal_(hal), prog_type_(kProgUnknown) {}
    virtual DpError Init(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) noexcept;
    [[nodiscard]] virtual DpError Polling(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    [[nodiscard]] virtual DpError TransferIn(uint8_t *data, size_t size, cs_pin_state_e cs = kCsHigh) noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    [[nodiscard]] DpError TransferIn(uint8_t &data, cs_pin_state_e cs = kCsHigh) noexcept
    {
        return TransferIn(&data, 1, cs);
    }
    [[nodiscard]] virtual DpError TransferOut(const uint8_t *data, size_t size, cs_pin_state_e cs = kCsHigh) noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    [[nodiscard]] virtual DpError TransferOut(uint8_t data, cs_pin_state_e cs = kCsHigh) noexcept
    {
        return TransferOut(&data, 1, cs);
    }
    [[nodiscard]] virtual DpError PowerOn() noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    [[nodiscard]] virtual DpError PowerOff() noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    [[nodiscard]] virtual DpError PowerConfig(DevPowerChan chan, int mvolt) noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    [[nodiscard]] virtual int getPowerConfig(DevPowerChan chan) noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return 0;
    }

   protected:
    [[nodiscard]] virtual DpError ShutDown() noexcept
    {
        DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
        return kSc;
    }
    int StartAppli();
    int AssignProg();
    int LeaveStandaloneMode();
    int QueryBoard();
    int CheckProgrammerInfo();
    private:
    const std::shared_ptr<ProgrammerHal> &hal_;
    programmer_type_e prog_type_;
    std::string hardware_id_;
    uint32_t firmware_ver_;
    uint32_t io1_selection_;
    uint32_t io4_selection_;
    uint32_t is_new_command;
};  // class ProgrammerInterface

}  // namespace dp

#endif  // PROGRAMMER_INTERFACE_HPP