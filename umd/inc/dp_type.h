#ifndef DP_TYPE_H
#define DP_TYPE_H

namespace dp
{
// Indicate that is a internal API.
#define INTERNAL_API

// Device Attributes
enum DevAttri
{
    // Simulated Dev or not
    kIsSimulation = 0,
    // Device PCIe BDF
    kDevAttrBdf = 1,
    // Device VID DID
    kDevAttrVidDid = 2,
    // Device HW Version
    kDevAttrHwVer = 3,
    // Driver Version
    kDevAttrDrvVer = 4,
    // Engine Utilization
    kDevEngineUtil = 5,
    // Memory Utilization
    kDevMemUtil = 6,
    // Device type
    kDevAttrType = 7,
    // Kernel driver Version
    kDevAttrKrlDrvVer = 8,
    // HW Serial Number
    kDevHwSerialNum = 9,
    // With fpga sim
    kDevAttrWithSim = 10,
    // IOMMU enabled or not
    kDevAttrIommuEnable = 11,
    // Attribute End
    kDevAttriEnd,
};

enum DevPowerChan
{
    kPowerVcc = 0,
    kPowerVpp = 1,
    kPowerChanMax,
};

enum DevResetType
{
    kDevResetNone = 0,
    kDevColdReset = 0x00000001,
    kDevWarmReset = 0x00000002,
};

enum DevFlashSeries
{
    kFlashSeries25 = 0,
    kFlashSeries45 = 8,
};

enum DevPowerVccSet
{
    kVccOff = 0,
    kVcc3V5 = 0x10,
    kVcc2V5 = 0x11,
    kVcc1V8 = 0x12,
    kVccKeep = 0xFF
};

enum DevBusClockSet
{
    kClk24M = 0,
    kClk8M,
    kClk12M,
    kClk3M,
    kClk2M18,
    kClk1M5,
    kClk750K,
    kClk375K
};

}  // namespace dp

#define FLASH_ID_MASK(len) (((~0U) >> (32 - (len)*8)))

#endif
