#ifndef PROGRAMMER_HPP
#define PROGRAMMER_HPP

#include <stdint.h>

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "dp_config.h"
#include "dp_error.h"
#include "dp_logging.hpp"
#include "dp_type.h"
#include "flash_database.hpp"
#include "flash_interface.hpp"
#include "programmer_interface.hpp"

namespace dp
{
class Programmer
{
    friend class FlashInterface;

   public:
    Programmer(const std::string &db_file) : db_(FlashDatabase::getInstance(db_file)), flash_info_(nullptr)
    {
        flash_interface_.reset();
        prog_interface_.reset();
        DP_CHECK(db_.isLoaded()) << "invalid flash database file: " << db_file;
        DP_LOG(INFO) << "Flash database has " << db_.getCount() << " flash(es)";
    }
    [[nodiscard]] virtual DpError Detect(std::set<std::string> &flash_name_list) noexcept;
    const FlashInfo *Select(const std::string &flash_name) noexcept;
    [[nodiscard]] bool isSelected() const noexcept { return flash_info_ != nullptr; }
    [[nodiscard]] std::shared_ptr<FlashInterface> getSelectedFlash() const noexcept { return flash_interface_; }

   protected:
    [[nodiscard]] DpError ListFlash(std::pair<uint32_t, uint32_t> readid_code, uint32_t power_vdd, uint32_t jedec_id,
                                    std::set<std::string> &flash_name_list);
    virtual uint32_t CommonIdentifier(uint8_t cmd, uint8_t size);
    FlashDatabase &db_;
    const FlashInfo *flash_info_;
    std::shared_ptr<FlashInterface> flash_interface_;
    std::shared_ptr<ProgrammerInterface> prog_interface_;
};

class PowerControl
{
   public:
    PowerControl(std::shared_ptr<ProgrammerInterface> interface) : interface_(interface)
    {
        for (int chan = 0; chan < kPowerChanMax; ++chan)
        {
            power_config_[chan] = interface_->getPowerConfig((DevPowerChan)chan);
        }
        interface_->PowerOn();
    }
    ~PowerControl()
    {
        interface_->PowerOff();
        for (int chan = 0; chan < kPowerChanMax; ++chan)
        {
            interface_->PowerConfig((DevPowerChan)chan, power_config_[chan]);
        }
    }
    int Config(DevPowerChan chan, int mvolt) { return interface_->PowerConfig(chan, mvolt); }

   private:
    std::shared_ptr<ProgrammerInterface> interface_;
    int power_config_[kPowerChanMax];
};
}  // namespace dp

#endif  // #ifndef PROGRAMMER_HPP