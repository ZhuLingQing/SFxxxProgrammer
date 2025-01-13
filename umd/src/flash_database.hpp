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

namespace dp::sf
{
struct die_info_t
{
    long die_size;
    int block_size;
    int page_size;
    int id_size;
    int uid_size;

    int block_erase_addr_size;
    int page_program_addr_size;
    int page_read_addr_size;
    int random_program_addr_size;
    int random_read_addr_size;

    int cmd_read_id;
    int cmd_read_uid;
    int cmd_erase;
    int cmd_block_erase;
    int cmd_page_program;
    int cmd_page_read;
    int cmd_random_program;
    int cmd_random_read;
    int cmd_read_status;
    int cmd_ready_bit;
};
struct flash_info_t
{
    std::string TypeName;
    std::string ICType;
    std::string Class;
    std::string UniqueID;
    std::string Description;
    std::string Manufacturer;
    std::string ManufactureUrl;
    std::string ProgramIOMethod;
    bool MXIC_WPmode;

    uint32_t Voltage;
    uint32_t VppSupport;
    uint32_t Clock;
    uint32_t Timeout;
    uint32_t ManufactureID;
    uint32_t JedecDeviceID;
    uint32_t AlternativeID;
    uint32_t DeviceID;

    uint32_t ChipSizeInKByte;
    uint32_t SectorSizeInByte;
    uint32_t PageSizeInByte;
    uint32_t AddrWidth;
    uint32_t ReadDummyLen;
    uint32_t IDNumber;
    uint32_t RDIDCommand;

    std::string OperationDll;
    uint32_t SupportedProduct;
    uint32_t TopBootID;
    uint32_t BottomBootID;
    uint32_t AAIByte;

    uint32_t die_count;
    struct die_info_t die_info[CONFIG_MAX_DIE_COUNT];
    void Dump() const;
};
struct database_info_t
{
    std::string Description;
    std::string Creator;
    std::string Ver;
    std::string CreateDate;
    std::string Locale;
    std::string PortofolioDescription;
    void Dump() const;
};

class FlashInfo
{
   public:
    FlashInfo(const struct flash_info_t &info = info_null_) { info_ = std::move(info); }
    bool isValid() const { return info_.TypeName.size() > 0; }
    const struct flash_info_t &getFlashInfo() const { return info_; }
    const std::string &getName() const { return info_.TypeName; }
    bool CheckId(const uint8_t *id, int size);
    void Dump() const { info_.Dump(); }
    static struct flash_info_t info_null_;

   private:
    struct flash_info_t info_;
};

struct flash_readid_info_t
{
    uint32_t RDIDCommand;
    uint32_t IDNumber;
};

class FlashDatabase
{
   public:
    static FlashDatabase &getInstance(std::string filename = "");
    DpError ReLoad(std::string &filename);
    bool isLoaded() const { return flash_info_map_.size() ? true : false; }
    size_t getCount() const { return flash_info_map_.size(); }
    const FlashInfo &getFlashInfo(const std::string &name);
    std::vector<const FlashInfo *> getFlashList(const uint8_t *id, int size);
    int Save(const std::string &filename);
    std::set<std::pair<uint32_t, uint32_t>> getReadIdInfoList();
    std::set<uint32_t> getPowerVddList();

   private:
    FlashDatabase();  // singleton
    static std::string ConvertString(const std::string &str);
    static uint32_t ConvertVoltage(const std::string &str);
    static uint32_t ConvertFrequency(const std::string &str);
    static uint32_t ConvertUint32(const std::string &str);
    static bool ConvertBoolean(const std::string &str);
    std::unordered_map<std::string, FlashInfo> flash_info_map_;
    struct database_info_t database_info_;
};
}  // namespace dp::sf

#endif  // FLASH_DATABASE_HPP