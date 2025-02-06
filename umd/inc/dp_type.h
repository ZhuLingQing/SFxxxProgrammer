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
    kClk375K,
};
enum vendor_request_other_e
{
    kReqOtherReceivePage = 1,
    kReqOtherUnlockRASS = 3,
    kReqOtherEraseST7Sec1 = 4,
    kReqOtherEraseST7Sec2 = 5,
    kReqOtherREAD = 6,     // Read WriteManufacturerID
    kReqOtherREAD_EX = 7,  // ReadMemOnST7
    kReqOtherProgramPage = 8,
    kReqOtherUpdateChecksum = 9,
    kReqOtherStartAppli = 0xb,
};
enum vendor_request_endpoint_e
{
    kReqEpTransceive = 1,
    kReqEpPolling = 2,
    kReqEpSetPowerVpp = 3,       // dediprog_set_vpp_voltage
    kReqEpSetTargetFlash = 4,    // SET_TARGET_FLASH
    kReqEpReadOnBoardFlash = 5,  // ReadOnBoardFlash
    kReqEpWriteEEPROM = 6,       // WRITE_EEPROM
    kReqEpSetIO = 7,             // SET_IO, SetLEDProgBoard
    kReqEpGetProgInfo = 8,       // PROGINFO_REQUEST
    kReqEpPowerVcc = 9,          // SET_VCC
    kReqEpStandAlone = 0xA,      // SET_SA
    kReqEpSetCs = 0x14,          // SET_CS
    kReqEpSetIoMode = 0x15,      // SET_IOMODE
    kReqEpUpdateSF600Flash = 0x1A,
    kReqEpUpdateSF600FPGA = 0x1B,
    kReqEpGetFPGAVersion = 0x1C,
    kReqEpDTC_READ = 0x20,  // DTC_READ
    kReqEpWRITE = 0x30,     // BULK_WRITE
    kReqEpWRITE45 = 0x31,   // BULK_WRITE_45

    kReqEpSetSpiClock = 0x61,  // SET_SPICLK
    kReqEpCheckSDCard = 0x65,
    kReqEpReadSerialNumber = 0x71,
    kReqEpON,  // bOn ? 0x0 : 0x01
};
enum target_flash_e
{
    kStartupCARD = 0x00,
    kStartupSF1,
    kStartupSF2,
    kStartupSocket,
};
enum programmer_type_e
{
    kProgUnknown,
    kProgSimulator,
    kProgSF100,
    kProgSF600,
    kProgSF700,
    kProgSF600G2
};

enum programmer_led_e
{
    kLedOff = 0x709,
    kLedGreen = 0x609,
    kLedOrange = 0x509,
    kLedRed = 0x309
};

}  // namespace dp

#define FLASH_ID_MASK(len) (((~0U) >> (32 - (len)*8)))

#endif
