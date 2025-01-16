#ifndef FLASH_INFO_HPP
#define FLASH_INFO_HPP

//  -- cpp headers --
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

//  -- local headers --
#include "dp_config.h"
#include "dp_error.h"

namespace dp
{
enum flash_class_e
{
    kVendor_NANTRONICS,
    kClass_N25Sxx = kVendor_NANTRONICS,
    kVendor_NANTRONICS_END,

    kVendor_ATO = kVendor_NANTRONICS_END,
    kClass_ATO25Qxx = kVendor_ATO,
    kVendor_ATO_END,

    kVendor_STMICROELECTRONICS = kVendor_ATO_END,
    kClass_M25Pxx_Large_4Die = kVendor_STMICROELECTRONICS,
    kClass_M25Pxx_Large_2Die,
    kClass_M25Pxx_Large,
    kClass_N25Qxxx_Large,
    kClass_M25PExx,
    kClass_M25Pxx,
    kClass_M45PExx,
    kVendor_STMICROELECTRONICS_END,

    kVendor_SST = kVendor_STMICROELECTRONICS_END,
    kClass_25xFxxA = kVendor_SST,
    kClass_25xFxxB,
    kClass_25xFxxC,
    kClass_25xFxx,
    kClass_26VFxxC,
    kVendor_SST_END,

    kVendor_WINBOND = kVendor_SST_END,
    kClass_W25Bxx = kVendor_WINBOND,
    kClass_W25Pxx_Large,
    kClass_W25Pxx,
    kClass_W25Qxx_Large,
    kClass_W25Xxx,
    kClass_W25Mxx_Large,
    kVendor_WINBOND_END,

    kVendor_PMC = kVendor_WINBOND_END,
    kClass_PM25LVxxx = kVendor_PMC,
    kClass_PM25Wxxx,
    kVendor_PMC_END,

    kVendor_SPANSION = kVendor_PMC_END,
    kClass_S25FLxxx_Large = kVendor_SPANSION,
    kClass_S25FLxxx,
    kClass_S25FSxxxS_Large,
    kClass_S25FLxxxL_Large,
    kClass_S70FSxxx_Large,
    kVendor_SPANSION_END,

    kVendor_MACRONIX = kVendor_SPANSION_END,
    kClass_MX25Lxxx_Large = kVendor_MACRONIX,
    kClass_MX25Lxxx_PP32,
    kClass_MX25Lxxx,
    kVendor_MACRONIX_END,

    kVendor_EON = kVendor_MACRONIX_END,
    kClass_EN25QHxx_Large = kVendor_EON,
    kClass_EN25Xxx,
    kVendor_EON_END,

    kVendor_ATMEL = kVendor_EON_END,
    kClass_AT26xxx = kVendor_ATMEL,
    kClass_AT25Fxxx,
    kClass_AT25FSxxx,
    kClass_AT45DBxxxD,
    kClass_AT45DBxxxB,
    kVendor_ATMEL_END,

    kVendor_AMIC = kVendor_ATMEL_END,
    kClass_A25Lxxx = kVendor_AMIC,
    kClass_A25LQxxx,
    kVendor_AMIC_END,

    kVendor_ESMT = kVendor_AMIC_END,
    kClass_F25Lxx = kVendor_ESMT,
    kVendor_ESMT_END,

    kVendor_INTEL = kVendor_ESMT_END,
    kClass_S33 = kVendor_INTEL,
    kVendor_INTEL_END,

    kVendor_FREESCALE = kVendor_INTEL_END,
    kClass_MCF = kVendor_FREESCALE,
    kVendor_FREESCALE_END,

    kVendor_SANYO = kVendor_FREESCALE_END,
    kClass_LE25FWxxx = kVendor_SANYO,
    kVendor_SANYO_END,

    kVendor_TSI = kVendor_SANYO_END,
    kClass_TS25Lxx_0A = kVendor_TSI,
    kClass_TS25Lxx,
    kVendor_TSI_END,

    kVendor_SILICONBLUE = kVendor_TSI_END,
    kClass_iCE65 = kVendor_SILICONBLUE,
    kVendor_SILICONBLUE_END,

    kVendor_FIDELIX = kVendor_SILICONBLUE_END,
    kClass_FM25Qxx = kVendor_FIDELIX,
    kVendor_FIDELIX_END,

    kVendor_FUDAN = kVendor_FIDELIX_END,
    kClass_FM25Fxx,
    kVendor_FUDAN_END,

    kClass_Number = kVendor_FUDAN_END,
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

    void Dump() const;
};

class FlashClass
{
   public:
    // FlashClass &getInstance() {
    //     static FlashClass flash_class_;
    //     return flash_class_;
    // }
    static flash_class_e getClass(const std::string &class_name)
    {
        auto it = flash_class_.find(class_name);
        if (it == flash_class_.end()) return kClass_Number;
        return it->second;
    }
    static flash_class_e getVendor(const std::string &vendor_name)
    {
        auto it = flash_vendor_.find(vendor_name);
        if (it == flash_vendor_.end()) return kClass_Number;
        return it->second;
    }
    static const std::string getVendor(flash_class_e class_enum)
    {
        for (auto x : flash_vendor_)
            if (x.second <= class_enum) return x.first;
        return std::string();
    }
    static size_t getCount() { return flash_class_.size(); }

   private:
    // FlashClass() {}
    static const std::unordered_map<std::string, flash_class_e> flash_class_;
    static const std::unordered_map<std::string, flash_class_e> flash_vendor_;
};  // namespace dp

class FlashInfo
{
   public:
    FlashInfo(const struct flash_info_t &info);
    bool isValid() const { return info_.TypeName.size() > 0; }
    const struct flash_info_t &getInfo() const { return info_; }
    flash_class_e getClass() const { return class_enum_; }
    const std::string &getName() const { return info_.TypeName; }
    void Dump() const { info_.Dump(); }
    bool operator==(const FlashInfo &rhs) const { return info_.TypeName == rhs.info_.TypeName; }
    static FlashInfo &null()
    {
        static FlashInfo flash_info_null_(null_info_);
        return flash_info_null_;
    }

   private:
    void doOverride();
    static struct flash_info_t null_info_;
    struct flash_info_t info_;
    flash_class_e class_enum_;
};
}  // namespace dp

#endif  // FLASH_INFO_HPP