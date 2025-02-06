#ifndef DP_SPI_FLASH_COMMAND_SET_H
#define DP_SPI_FLASH_COMMAND_SET_H

enum flash25_cmd_e
{
    kCmdWriteEnable = 0x06,
    kCmdWriteDisable = 0x04,
    kCmdReadStatus = 0x05,
    kCmdWriteStatus = 0x01,
    kCmdReadData = 0x03,
    kCmdFastReadData = 0x0B,
    // kCmdFastReadDual = 0x3B,
    // kCmdFastReadDualIo = 0xBB,
    kCmdPageProgram = 0x02,
    kCmdBlockErase = 0xD8,
    // kCmdSectorErase = 0x20,
    kCmdChipErase = 0xC7,
    kCmdPowerDown = 0xB9,
    kCmdReleasePowerDown = 0xAB,
    kCmdReadSignature = 0xAB,
    // kCmdManufacturerId = 0x90,
    kCmdJedecId = 0x9F,
    // kCmdUniqueId = 0x4B,
    // kCmdReadUID = 0x4B,
    // kCmdReadParameter = 0x58,
    // kCmdReadSecurityRegister = 0x48,
    // kCmdProgramSecurityRegister = 0x42,
    // kCmdReadLock = 0xE8,
    kCmdReadSecurityRegister = 0x2B,
    kCmdEnter4BitMode = 0xB7,
    kCmdExit4BitMode = 0xE9,
};
enum flash25_status_e
{
    kStatusWIP = 0x01,
    kStatusWEL = 0x02,
    kStatusBP0 = 0x04,
    kStatusBP1 = 0x08,
    kStatusBP2 = 0x10,
    kStatusBP3 = 0x20,
    kStatusWP = 0x40,
    kStatusWPEnable = 0x80,
    kStatusSRWD = 0x80,  // Status Register Write Disable

    kStatusBP = 0x9C,
};
enum flash25_protect_e
{
    kProtectNone = 0 << 2,
    kProtectUpper1 = 1 << 2,
    kProtectUpper2 = 2 << 2,
    kProtectUpper4 = 3 << 2,
    kProtectUpper8 = 4 << 2,
    kProtectUpperQuarter = 5 << 2,
    kProtectUpperHalf = 6 << 2,
    kProtectAll = 7 << 2,
    kProtectMask = kProtectAll,
};

#endif  // DP_SPI_FLASH_COMMAND_SET_H