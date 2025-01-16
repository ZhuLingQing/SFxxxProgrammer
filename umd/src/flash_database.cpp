#include "flash_database.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "dp_logging.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace dp
{
#define DumpInfo(name) DP_LOG(INFO) << #name << ": " << name
#if !defined(CONFIG_DATABASE_SAVE_AS_DICT)
#define JsonKeySet(key) j_chip_info[#key] = info.key
#else
#define JsonKeySet(key) database["Portofolio"]["Chip"][std::string_view(type_name_)][#key] = info.key
#endif
#define JsonKeyGetRaw(key, type) info.key = chip.at(#key).get<type>()
#define JsonKeyGet(key, default_value, type) \
    info.key = (chip.find(#key) == chip.end()) ? default_value : Convert##type(chip.at(#key).get<std::string>());

void database_info_t::Dump() const
{
    DumpInfo(Description);
    DumpInfo(Creator);
    DumpInfo(Ver);
    DumpInfo(CreateDate);
    DumpInfo(Locale);
    DumpInfo(PortofolioDescription);
    DumpInfo(NewDb);
}

FlashDatabase::FlashDatabase() { flash_info_map_.clear(); }

FlashDatabase &FlashDatabase::getInstance(const std::string filename)
{
    static FlashDatabase instance;
    if (!instance.isLoaded()) instance.ReLoad(filename);
    return instance;
}
const FlashInfo *FlashDatabase::getFlashInfo(const std::string &name)
{
    auto info = flash_info_map_.find(name);
    if (info == flash_info_map_.end()) return nullptr;
    return &info->second;
}
DpError FlashDatabase::ReLoad(const std::string &filename)
{
    DP_LOG(INFO) << __PRETTY_FUNCTION__;
    flash_info_map_.clear();
    std::ifstream ifs(filename);
    json read_data = json::parse(ifs);
    auto database = read_data["DediProgChipDatabase"];

    database_info_.Description = database.at("Description").get<std::string>();
    database_info_.Creator = database.at("Creator").get<std::string>();
    database_info_.Ver = database.at("Ver").get<std::string>();
    database_info_.CreateDate = database.at("CreateDate").get<std::string>();
    database_info_.Locale = database.at("Locale").get<std::string>();
    database_info_.PortofolioDescription = database["Portofolio"].at("Description").get<std::string>();
    database_info_.NewDb = (database.find("NewDb") == database.end()) ? false : database.at("NewDb").get<bool>();
    database_info_.Dump();

    auto chip_list = database["Portofolio"].at("Chip");
    DP_LOG(INFO) << "ChipList from JSON: " << chip_list.size();
    for (auto chip : chip_list)
    {
        struct flash_info_t info;
        // DP_LOG(INFO) << "Chip: " << chip.at("TypeName").get<std::string>(); //
        // for debugging
        if (flash_info_map_.find(chip.at("TypeName").get<std::string>()) != flash_info_map_.end())
        {
            DP_LOG(WARNING) << "Duplicate chip: " << chip.at("TypeName").get<std::string>();
            continue;
        }
        if (database_info_.NewDb)
        {
            JsonKeyGetRaw(TypeName, std::string);
            JsonKeyGetRaw(ICType, std::string);
            JsonKeyGetRaw(Class, std::string);
            JsonKeyGetRaw(UniqueID, std::string);
            JsonKeyGetRaw(Description, std::string);
            JsonKeyGetRaw(Manufacturer, std::string);
            JsonKeyGetRaw(ManufactureUrl, std::string);
            JsonKeyGetRaw(ProgramIOMethod, std::string);
            JsonKeyGetRaw(MXIC_WPmode, bool);

            JsonKeyGetRaw(Voltage, uint32_t);
            JsonKeyGetRaw(VppSupport, uint32_t);
            JsonKeyGetRaw(Clock, uint32_t);
            JsonKeyGetRaw(Timeout, uint32_t);
            JsonKeyGetRaw(ManufactureID, uint32_t);
            JsonKeyGetRaw(JedecDeviceID, uint32_t);
            JsonKeyGetRaw(AlternativeID, uint32_t);
            JsonKeyGetRaw(DeviceID, uint32_t);

            JsonKeyGetRaw(ChipSizeInKByte, uint32_t);
            JsonKeyGetRaw(SectorSizeInByte, uint32_t);
            JsonKeyGetRaw(PageSizeInByte, uint32_t);
            JsonKeyGetRaw(AddrWidth, uint32_t);
            JsonKeyGetRaw(ReadDummyLen, uint32_t);
            JsonKeyGetRaw(IDNumber, uint32_t);
            JsonKeyGetRaw(RDIDCommand, uint32_t);

            // extra info
            JsonKeyGetRaw(OperationDll, std::string);
            JsonKeyGetRaw(SupportedProduct, uint32_t);
            JsonKeyGetRaw(TopBootID, uint32_t);
            JsonKeyGetRaw(BottomBootID, uint32_t);
            JsonKeyGetRaw(AAIByte, uint32_t);
        }
        else
        {
            JsonKeyGet(TypeName, "", String);
            JsonKeyGet(ICType, "", String);
            JsonKeyGet(Class, "", String);
            JsonKeyGet(UniqueID, "", String);
            JsonKeyGet(Description, "", String);
            JsonKeyGet(Manufacturer, "", String);
            JsonKeyGet(ManufactureUrl, "", String);
            JsonKeyGet(ProgramIOMethod, "SPSD_R", String);
            JsonKeyGet(MXIC_WPmode, false, Boolean);

            JsonKeyGet(Voltage, 1800, Voltage);
            JsonKeyGet(VppSupport, 0, Voltage);
            JsonKeyGet(Clock, 10, Frequency);
            JsonKeyGet(Timeout, 1000, Uint32);
            JsonKeyGet(ManufactureID, 0, Uint32);
            JsonKeyGet(JedecDeviceID, 0, Uint32);
            JsonKeyGet(AlternativeID, 0, Uint32);
            JsonKeyGet(DeviceID, 0, Uint32);

            JsonKeyGet(ChipSizeInKByte, 0, Uint32);
            if (chip.find("SectorSizeInByte") != chip.end())
                info.SectorSizeInByte = ConvertUint32(chip.at("SectorSizeInByte").get<std::string>());
            else if (chip.find("BlockSizeInByte") != chip.end())
                info.SectorSizeInByte = ConvertUint32(chip.at("BlockSizeInByte").get<std::string>());
            else
                DP_LOG(WARNING) << info.TypeName << ": @SectorSizeInByte or @BlockSizeInByte not found";
            JsonKeyGet(PageSizeInByte, 0, Uint32);
            JsonKeyGet(AddrWidth, 0, Uint32);
            JsonKeyGet(ReadDummyLen, 0, Uint32);
            JsonKeyGet(IDNumber, 0, Uint32);
            JsonKeyGet(RDIDCommand, 0, Uint32);

            // extra info
            JsonKeyGet(OperationDll, "", String);
            JsonKeyGet(SupportedProduct, 0, Uint32);
            JsonKeyGet(TopBootID, 0, Uint32);
            JsonKeyGet(BottomBootID, 0, Uint32);
            JsonKeyGet(AAIByte, 0, Uint32);
        }
        MakeDieInfo(info);
        flash_info_map_.insert(std::make_pair(info.TypeName, FlashInfo(std::move(info))));
    }
    DP_LOG(INFO) << "ChipList in db: " << flash_info_map_.size();
    return kSc;
}

DpError FlashDatabase::getFlashNameList(const std::pair<uint32_t, uint32_t> &readid, uint32_t power, uint32_t id,
                                        std::set<std::string> &flash_name_list)
{
    if (!isLoaded()) return kDevInvalidConfig;
    for (auto &flash_info : flash_info_map_)
    {
        auto info = flash_info.second.getInfo();
        if (info.RDIDCommand == readid.first && info.IDNumber == readid.second)
        {
            if (info.Voltage >= power)
            {
                if (info.JedecDeviceID == id)
                {
                    flash_name_list.insert(info.TypeName);
                }
            }
        }
    }
    return kSc;
}

int FlashDatabase::Save(const std::string &filename)
{
    if (0 == flash_info_map_.size()) return -1;
    DP_LOG(INFO) << __PRETTY_FUNCTION__;
    json write_data;
    json &database = write_data["DediProgChipDatabase"];

    database["Description"] = database_info_.Description;
    database["Creator"] = database_info_.Creator;
    database["Ver"] = database_info_.Ver;
    database["CreateDate"] = database_info_.CreateDate;
    database["Locale"] = database_info_.Locale;
    database["Portofolio"]["Description"] = database_info_.PortofolioDescription;

    for (auto &flash_info : flash_info_map_)
    {
        auto info = flash_info.second.getInfo();

        std::string type_name_ = info.TypeName;
#if !defined(CONFIG_DATABASE_SAVE_AS_DICT)
        json j_chip_info;
#endif
        JsonKeySet(TypeName);
        JsonKeySet(ICType);
        JsonKeySet(Class);
        JsonKeySet(UniqueID);
        JsonKeySet(Description);
        JsonKeySet(Manufacturer);
        JsonKeySet(ManufactureUrl);
        JsonKeySet(ProgramIOMethod);
        JsonKeySet(MXIC_WPmode);

        JsonKeySet(Voltage);
        JsonKeySet(VppSupport);
        JsonKeySet(Clock);
        JsonKeySet(Timeout);
        JsonKeySet(ManufactureID);
        JsonKeySet(JedecDeviceID);
        JsonKeySet(AlternativeID);
        JsonKeySet(DeviceID);

        JsonKeySet(ChipSizeInKByte);
        JsonKeySet(SectorSizeInByte);
        JsonKeySet(PageSizeInByte);
        JsonKeySet(AddrWidth);
        JsonKeySet(ReadDummyLen);
        JsonKeySet(IDNumber);
        JsonKeySet(RDIDCommand);

        JsonKeySet(OperationDll);
        JsonKeySet(SupportedProduct);
        JsonKeySet(TopBootID);
        JsonKeySet(BottomBootID);
        JsonKeySet(AAIByte);
#if !defined(CONFIG_DATABASE_SAVE_AS_DICT)
        database["Portofolio"]["Chip"].push_back(j_chip_info);
#endif
    }
    database["NewDb"] = true;
    DP_LOG(INFO) << __PRETTY_FUNCTION__ << ":" << database["Portofolio"]["Chip"].size() << " chips";
    std::ofstream o(filename);
    o << write_data.dump(4);  // 使用4个空格缩进
    return 0;
}
std::set<std::pair<uint32_t, uint32_t>> FlashDatabase::getReadIdInfoList()
{
    std::set<std::pair<uint32_t, uint32_t>> read_id;
    for (auto &flash_info : flash_info_map_)
    {
        auto info = flash_info.second.getInfo();
        read_id.insert({info.RDIDCommand, info.IDNumber});
    }
    return read_id;
}
std::set<uint32_t> FlashDatabase::getPowerVddList()
{
    std::set<uint32_t> power_vdd;
    for (auto &flash_info : flash_info_map_)
    {
        auto info = flash_info.second.getInfo();
        power_vdd.insert(info.Voltage);
    }
    return power_vdd;
}
/* static */ std::string FlashDatabase::ConvertString(const std::string &str) { return str; }
/* static */ uint32_t FlashDatabase::ConvertVoltage(const std::string &str)
{
    DP_CHECK(!str.empty()) << __PRETTY_FUNCTION__ << ": empty string";
    size_t unitPos = str.find_last_not_of("0123456789.");
    if (unitPos == std::string::npos)
    {
        return static_cast<int>(std::stod(str) * 1000);
    }
    std::string valueStr = str.substr(0, unitPos);
    std::string unitStr = str.substr(unitPos);
    if (strcasecmp(unitStr.c_str(), "V") == 0)
    {
        return static_cast<int>(std::stod(valueStr) * 1000);
    }
    else if (strcasecmp(unitStr.c_str(), "mV") == 0)
    {
        return static_cast<int>(std::stod(valueStr));
    }
    DP_LOG(FATAL) << __PRETTY_FUNCTION__ << ": unknown unit: " << unitStr;
    return 0;
}
/* static */ uint32_t FlashDatabase::ConvertFrequency(const std::string &str)
{
    DP_CHECK(!str.empty()) << __PRETTY_FUNCTION__ << ": empty string";
    size_t unitPos = str.find_first_not_of("0123456789");
    DP_CHECK_NE(unitPos, std::string::npos);
    std::string valueStr = str.substr(0, unitPos);
    std::string unitStr = str.substr(str.find_first_not_of("0123456789/"));
    if (strcasecmp(unitStr.c_str(), "MHz") == 0)
    {
        return static_cast<int>(std::stod(valueStr));
    }
    DP_LOG(FATAL) << __PRETTY_FUNCTION__ << ": " << str << ": unknown unit: " << unitStr;
    return static_cast<int>(std::stod(valueStr));
}
/* static */ uint32_t FlashDatabase::ConvertUint32(const std::string &str)
{
    DP_CHECK(!str.empty()) << __PRETTY_FUNCTION__ << ": empty string";
    bool isHex = str.size() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X');

    if (isHex)
    {
        return std::stoi(str, nullptr, 16);
    }
    else
    {
        return std::stoi(str);
    }
}
/* static */ bool FlashDatabase::ConvertBoolean(const std::string &str)
{
    DP_CHECK(!str.empty()) << __PRETTY_FUNCTION__ << ": empty string";
    if (strcasecmp(str.c_str(), "true") == 0)
        return true;
    else if (strcasecmp(str.c_str(), "false") == 0)
        return false;
    DP_LOG(FATAL) << __PRETTY_FUNCTION__ << ": " << str << ": invalid boolean: " << str;
    return false;
}
/* static */ void FlashDatabase::MakeDieInfo(struct flash_info_t &info)
{
    // info.die_count = 1;
    // for (int i = 0; i < info.die_count; i++)
    // {
    //     struct die_info_t &die = info.die_info[i];
    //     die.die_id = i;
    //     die.die_size = info.ChipSizeInKByte * 1024;
    //     die.sector_size = info.SectorSizeInByte;
    //     die.page_size = info.PageSizeInByte;

    //     die.sector_erase_addr_len = info.AddrWidth;
    //     die.page_program_addr_len = info.AddrWidth;
    //     die.page_read_addr_len = info.AddrWidth;
    //     die.random_program_addr_len = info.AddrWidth;
    //     die.random_read_addr_len = info.AddrWidth;

    //     die.read_id_cmd = info.RDIDCommand;
    //     die.read_uid_cmd = info.;
    //     die.erase_cmd;
    //     die.sector_erase_cmd;
    //     die.page_program_cmd;
    //     die.page_read_cmd;
    //     die.random_program_cmd;
    //     die.random_read_cmd;
    //     die.read_status_cmd;
    //     die.ready_bit_cmd;
    // }
}
}  // namespace dp