#ifndef PROGRAMMER_HPP
#define PROGRAMMER_HPP

#include <stdint.h>
#include <stdio.h>

#include <vector>
#include <memory>
#include <iostream>

#include "dp_config.h"
#include "dp_error.h"
#include "dp_type.h"
#include "dp_logging.hpp"

namespace dp::prog
{

    class Programmer
    {
        public:
        Programmer(){};
        [[nodiscard]] virtual DpError Detect(std::set<std::string> &flash_name_list) noexcept {
            DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
            return kSc;
        }
        [[nodiscard]] virtual DpError Polling(uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) noexcept
        {
            DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
            return kSc;
        }
        [[nodiscard]] virtual DpError TransferIn(const uint8_t *data, size_t size) noexcept
        {
            DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
            return kSc;
        }
        [[nodiscard]] virtual DpError TransferOut(uint8_t *data, size_t size) noexcept
        {
            DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
            return kSc;
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
        // std::shared_ptr<TargetFlash> flash_;
    };

    class PowerControl
    {
        public:
        PowerControl(Programmer &prog) : prog_(prog){
            for (int chan = 0; chan < kPowerChanMax; ++chan) {
                power_config_[chan] = prog_.getPowerConfig((DevPowerChan)chan);
            }
            prog_.PowerOn();
        }
        ~PowerControl() {
            prog_.PowerOff();
            for (int chan = 0; chan < kPowerChanMax; ++chan) {
                prog_.PowerConfig((DevPowerChan)chan, power_config_[chan]);
            }
        }
        int Config(DevPowerChan chan, int mvolt) {
            return prog_.PowerConfig(chan, mvolt);
        }
        private:
        Programmer &prog_;
        int power_config_[kPowerChanMax];
    };
} // namespace dp::prog

#endif // #ifndef PROGRAMMER_HPP