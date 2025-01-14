#ifndef SFXXX_HPP
#define SFXXX_HPP

#include "programmer.hpp"
#include "flash_database.hpp"
#include <cstdlib> // getenv

using FlashDatabase = dp::sf::FlashDatabase;

namespace dp::prog
{
    class DummyProgrammer : public Programmer
    {
        public:
        DummyProgrammer(const std::string &db_file) : db_(FlashDatabase::getInstance(db_file)) {
            DP_CHECK(db_.isLoaded()) << "invalid flash database file: " << db_file;
            DP_LOG(INFO) << "Flash database has " << db_.getCount() << " flash(es)";
        }
        DpError Detect(std::set<std::string> &flash_name_list) noexcept override
        {
            DpError rc = kSc;
            flash_name_list.clear();
            std::pair<uint32_t, uint32_t> readid_code_;
            auto power_vdd = db_.getPowerVddList();
            auto readid = db_.getReadIdInfoList();

            for (auto power : power_vdd)
            {
                DP_CHECK_EQ(kSc, PowerConfig(kPwrVcc, power));
                PowerControl pwr(*this);
                for (auto id : readid | std::views::reverse)
                {
                    if (id.first == 0 || id.second == 0) continue; // RDIDCommand == 0 or IDNumber == 0
                    auto jedec_id = TryReadId(id.first, id.second);
                    if(jedec_id && kSc == ListFlash(id, power, jedec_id, flash_name_list)) // Found flash
                        return rc;
                }
            }
            DP_LOG(WARNING) << "No flash found";
            return kSc;
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
            if (chan == kPwrVcc)
                vdd_ = mvolt;
            return kSc;
        }
        private:
        uint32_t TryReadId(uint8_t cmd, uint8_t size)
        {
            uint32_t id = 0;
            const char* dummy_id = getenv("DUMMY_ID");
            if (cmd == 0x9F && vdd_ >= 3000)
                id = std::stoi(dummy_id, nullptr, 16);
            id &= ((~0U) >> (32 - size * 8));
            DP_LOG(INFO) << "CMD" << size << ": 0x" << std::hex << static_cast<uint32_t>(cmd) << ": 0x" << id;
            return id;
        }
        DpError ListFlash(std::pair<uint32_t, uint32_t> readid_code, uint32_t power_vdd, uint32_t jedec_id, std::set<std::string> &flash_name_list)
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
        FlashDatabase &db_;
        uint32_t vdd_;
    };
} // namespace dp::prog

#endif // #ifndef PROGRAMMER_HPP