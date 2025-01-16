#ifndef PROGRAMMER_INTERFACE_HPP
#define PROGRAMMER_INTERFACE_HPP

#include "dp_config.h"
#include "dp_error.h"
#include "dp_logging.hpp"
#include "dp_type.h"

namespace dp
{
class ProgrammerInterface
{
   public:
    ProgrammerInterface(void *handler) : handler_(handler) {}
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
    void *handler_;
};  // class ProgrammerInterface
}  // namespace dp

#endif  // PROGRAMMER_INTERFACE_HPP