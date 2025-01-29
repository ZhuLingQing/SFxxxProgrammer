#include "flash_info.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "dp_logging.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#define DumpInfo(name) std::cout << #name << ": " << name << std::endl;
#define MakeClass(name)      \
    {                        \
#name, kClass_##name \
    }
#define MakeVendor(name)      \
    {                         \
#name, kVendor_##name \
    }

namespace dp
{
struct flash_info_t FlashInfo::null_info_ = {
    .TypeName = "",
};

const std::unordered_map<std::string, flash_class_e> FlashClass::flash_vendor_{
    MakeVendor(NANTRONICS), MakeVendor(ATO),         MakeVendor(STMICROELECTRONICS),
    MakeVendor(SST),        MakeVendor(WINBOND),     MakeVendor(PMC),
    MakeVendor(SPANSION),   MakeVendor(MACRONIX),    MakeVendor(EON),
    MakeVendor(ATMEL),      MakeVendor(AMIC),        MakeVendor(ESMT),
    MakeVendor(INTEL),      MakeVendor(FREESCALE),   MakeVendor(SANYO),
    MakeVendor(TSI),        MakeVendor(SILICONBLUE), MakeVendor(FIDELIX),
    MakeVendor(FUDAN),
};

const std::unordered_map<std::string, flash_class_e> FlashClass::flash_class_{
    MakeClass(N25Sxx),
    MakeClass(ATO25Qxx),
    MakeClass(M25Pxx_Large_4Die),
    MakeClass(M25Pxx_Large_2Die),
    MakeClass(M25Pxx_Large),
    MakeClass(N25Qxxx_Large),
    MakeClass(M25PExx),
    MakeClass(M25Pxx),
    MakeClass(M45PExx),
    MakeClass(25xFxxA),
    MakeClass(25xFxxB),
    MakeClass(25xFxxC),
    MakeClass(25xFxx),
    MakeClass(26VFxxC),
    MakeClass(W25Bxx),
    MakeClass(W25Pxx_Large),
    MakeClass(W25Pxx),
    MakeClass(W25Qxx_Large),
    MakeClass(W25Xxx),
    MakeClass(W25Mxx_Large),
    MakeClass(PM25LVxxx),
    MakeClass(PM25Wxxx),
    MakeClass(S25FLxxx_Large),
    MakeClass(S25FLxxx),
    MakeClass(S25FSxxxS_Large),
    MakeClass(S25FLxxxL_Large),
    MakeClass(S70FSxxx_Large),
    MakeClass(MX25Lxxx_Large),
    MakeClass(MX25Lxxx_PP32),
    MakeClass(MX25Lxxx),
    MakeClass(EN25QHxx_Large),
    MakeClass(EN25Xxx),
    MakeClass(AT26xxx),
    MakeClass(AT25Fxxx),
    MakeClass(AT25FSxxx),
    MakeClass(AT45DBxxxD),
    MakeClass(AT45DBxxxB),
    MakeClass(A25Lxxx),
    MakeClass(A25LQxxx),
    MakeClass(F25Lxx),
    MakeClass(S33),
    MakeClass(MCF),
    MakeClass(LE25FWxxx),
    MakeClass(TS25Lxx_0A),
    MakeClass(TS25Lxx),
    MakeClass(iCE65),
    MakeClass(FM25Qxx),
    MakeClass(FM25Fxx),
};

void flash_info_t::Dump() const
{
    DumpInfo(TypeName);
    DumpInfo(ICType);
    DumpInfo(Class);
    DumpInfo(UniqueID);
    DumpInfo(Description);
    DumpInfo(Manufacturer);
    DumpInfo(ManufactureUrl);
    DumpInfo(ProgramIOMethod);
    DumpInfo(MXIC_WPmode);

    DumpInfo(Voltage);
    DumpInfo(VppSupport);
    DumpInfo(Clock);
    DumpInfo(Timeout);
    DumpInfo(ManufactureID);
    DumpInfo(JedecDeviceID);
    DumpInfo(AlternativeID);
    DumpInfo(DeviceID);

    DumpInfo(ChipSizeInKByte);
    DumpInfo(SectorSizeInByte);
    DumpInfo(PageSizeInByte);
    DumpInfo(AddrWidth);
    DumpInfo(ReadDummyLen);
    DumpInfo(IDNumber);
    DumpInfo(RDIDCommand);

    DumpInfo(OperationDll);
    DumpInfo(SupportedProduct);
    DumpInfo(TopBootID);
    DumpInfo(BottomBootID);
    DumpInfo(AAIByte);
}

FlashInfo::FlashInfo(const struct flash_info_t &info)
{
    info_ = std::move(info);
    class_enum_ = FlashClass::getClass(info_.Class);
    doOverride();
}
void FlashInfo::doOverride() {}
}  // namespace dp