#include "programmer.hpp"
#include "flash_database.hpp"
#include "flash_info.hpp"
#include "flash_interface.hpp"

namespace dp
{
const FlashInfo *Programmer::Select(const std::string &flash_name) noexcept
{
    auto info = db_.getFlashInfo(flash_name);
    if (nullptr == info)
    {
        DP_LOG(ERROR) << "Flash " << flash_name << " is not found in flash database";
        if (flash_interface_) flash_interface_.reset();
        return nullptr;
    }
    if (flash_info_ != info)
    {
        flash_info_ = info;
        flash_interface_.reset();
        flash_interface_ = std::make_unique<FlashInterface>(this);
    }
    return flash_info_;
}

DpError Programmer::Detect(std::set<std::string> &flash_name_list) noexcept
{
    DpError rc = kSc;
    flash_name_list.clear();
    std::pair<uint32_t, uint32_t> readid_code_;
    auto power_vdd = db_.getPowerVddList();
    auto readid = db_.getReadIdInfoList();

    for (auto power : power_vdd)
    {
        DP_CHECK_EQ(kSc, prog_interface_->PowerConfig(kPwrVcc, power));
        PowerControl pwr(prog_interface_);
        for (auto id : readid | std::views::reverse)  // revserse find, need -std=c++20
        {
            if (id.first == 0 || id.second == 0) continue;  // RDIDCommand == 0 or IDNumber == 0
            auto jedec_id = CommonIdentifier(id.first, id.second);
            if (jedec_id && kSc == ListFlash(id, power, jedec_id, flash_name_list))  // Found flash
                return rc;
        }
    }
    DP_LOG(WARNING) << "No flash found";
    return kSc;
}
DpError Programmer::ListFlash(std::pair<uint32_t, uint32_t> readid_code, uint32_t power_vdd, uint32_t jedec_id,
                              std::set<std::string> &flash_name_list)
{
    DpError rc = db_.getFlashNameList(readid_code, power_vdd, jedec_id, flash_name_list);
    if (kSc != rc) return rc;
    if (flash_name_list.size() == 0) return kExist;
    DP_LOG(INFO) << "ID: 0x" << std::hex << jedec_id << ": Found " << flash_name_list.size() << " flash(es)";
    for (auto &name : flash_name_list)
    {
        DP_LOG(INFO) << name;
    }
    return kSc;
}

uint32_t Programmer::CommonIdentifier(uint8_t cmd, uint8_t size)
{
    DpError rc;
    uint32_t id = 0;
    DP_CHECK(size <= 4) << "Invalid size";
    if ((rc = prog_interface_->TransferOut(cmd, kCsKeepLow)) != kSc) return 0;
    if ((rc = prog_interface_->TransferIn(reinterpret_cast<uint8_t *>(&id), size)) != kSc) return 0;
    id &= FLASH_ID_MASK(size);
    return id;
}
}  // namespace dp
