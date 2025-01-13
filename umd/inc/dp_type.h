#ifndef DP_TYPE_H
#define DP_TYPE_H

#ifdef __cplusplus
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

typedef enum
{
    kPwrVcc = 0,
    kPwrVpp = 1,
} DevPowerChan;

typedef enum
{
    kDevResetNone = 0,
    kDevColdReset = 0x00000001,
    kDevWarmReset = 0x00000002,
} DevResetType;

enum class DevType
{
    kDevUnknown = 0,
    kDevSFxxx = 1,
    kDevGangProgrammer = 2,
};
};
#else
typedef enum
{
    kDevUnknown = 0,
    kDevSFxxx = 1,
    kDevGangProgrammer = 2,
} DevType;
#endif

#endif
