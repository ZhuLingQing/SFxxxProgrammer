#ifndef FLASH_DATABASE_HPP
#define FLASH_DATABASE_HPP

//  -- cpp headers --
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

//  -- local headers --
#include "dp_config.h"
#include "dp_error.h"
#include "flash_info.hpp"

namespace dp
{
struct database_info_t
{
    std::string Description;
    std::string Creator;
    std::string Ver;
    std::string CreateDate;
    std::string Locale;
    std::string PortofolioDescription;
    bool NewDb;

    void Dump() const;
};

class FlashDatabase
{
   public:
    static FlashDatabase &getInstance(const std::string filename = "");
    DpError ReLoad(const std::string &filename);
    bool isLoaded() const { return flash_info_map_.size() ? true : false; }
    size_t getCount() const { return flash_info_map_.size(); }
    const FlashInfo *getFlashInfo(const std::string &name);
    std::set<std::pair<uint32_t, uint32_t>> getReadIdInfoList();
    std::set<uint32_t> getPowerVddList();
    DpError getFlashNameList(const std::pair<uint32_t, uint32_t> &readid, uint32_t power, uint32_t id,
                             std::set<std::string> &flash_name_list);

    int Save(const std::string &filename);

   private:
    FlashDatabase();  // singleton
    static std::string ConvertString(const std::string &str);
    static uint32_t ConvertVoltage(const std::string &str);
    static uint32_t ConvertFrequency(const std::string &str);
    static uint32_t ConvertUint32(const std::string &str);
    static bool ConvertBoolean(const std::string &str);
    static void MakeDieInfo(struct flash_info_t &flash_info);

    std::unordered_map<std::string, FlashInfo> flash_info_map_;
    struct database_info_t database_info_;
};
}  // namespace dp

#endif  // FLASH_DATABASE_HPP