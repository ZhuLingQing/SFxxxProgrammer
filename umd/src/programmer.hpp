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

namespace dp
{
class ProgrammerHal;
class FlashInterface;
class ProgrammerInterface;

class Programmer
{
    friend class FlashInterface;

   public:
    Programmer(const std::string &db_file, std::shared_ptr<ProgrammerInterface> interface);
    [[nodiscard]] virtual DpError Detect(std::set<std::string> &flash_name_list) noexcept;
    const FlashInfo *Select(const std::string &flash_name) noexcept;
    [[nodiscard]] bool isSelected() const noexcept { return flash_info_ != nullptr; }
    [[nodiscard]] std::shared_ptr<FlashInterface> getSelectedFlash() const noexcept { return flash_interface_; }
    DpError Shutdown();

   protected:
    [[nodiscard]] DpError ListFlash(std::pair<uint32_t, uint32_t> readid_code, uint32_t power_vdd, uint32_t jedec_id,
                                    std::set<std::string> &flash_name_list);
    virtual uint32_t CommonIdentifier(uint8_t cmd, uint8_t size);
    FlashDatabase &db_;
    const FlashInfo *flash_info_;
    std::shared_ptr<ProgrammerInterface> prog_interface_;
    std::shared_ptr<FlashInterface> flash_interface_;
};

class PowerControl
{
   public:
    PowerControl(std::shared_ptr<ProgrammerInterface> interface);
    ~PowerControl();
    int Config(DevPowerChan chan, int mvolt);

   private:
    std::shared_ptr<ProgrammerInterface> interface_;
    int power_config_[kPowerChanMax];
};
}  // namespace dp

#endif  // #ifndef PROGRAMMER_HPP