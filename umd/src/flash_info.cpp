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

namespace dp::sf
{
#define DumpInfo(name) DP_LOG(INFO) << #name << ": " << name
struct flash_info_t FlashInfo::null_info_ = {
    .TypeName = "",
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
} // namespace dp::sf