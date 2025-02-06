#include "programmer_interface.hpp"
#include "programmer_hal.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace dp
{
const std::map<int, DevPowerVccSet> ProgrammerInterface::power_vcc_map_ = {
    {0, kVccOff},    {1800, kVcc1V8}, {2500, kVcc2V5}, {3000, kVcc3V5},  // TODO: check this in database
    {3300, kVcc3V5},
};

const std::map<int, DevBusClockSet> ProgrammerInterface::bus_clock_map_ = {
    {24000, kClk24M}, {8000, kClk8M},  {12000, kClk12M}, {3000, kClk3M},
    {2180, kClk2M18}, {1500, kClk1M5}, {750, kClk750K},  {375, kClk375K},
};

const std::map<std::string, programmer_type_e> ProgrammerInterface::prog_type_map_ = {{"SimProg", kProgSimulator},
                                                                                      {"SF100", kProgSF100},
                                                                                      {"SF600", kProgSF600},
                                                                                      {"SF700", kProgSF700},
                                                                                      {"SF600G2", kProgSF600G2}};

DpError ProgrammerInterface::Init(uint32_t timeout)
{
    DpError rc;
    if (hal_->isOpen()) return kSc;
    if (hal_->Open() < 0) return kDevErr;
    if ((rc = StartAppli()) != kSc) return rc;
    if ((rc = ReadDeviceInfo()) != kSc) return rc;
    if ((rc = ReadFpgaVersion()) != kSc) return rc;
    if ((rc = setStandAlone(false)) != kSc) return rc;
    // if (LeaveStandaloneMode() < 0) return kDevErr;
    // if (QueryBoard() < 0) return kDevErr;
    // if (CheckProgrammerInfo() < 0) return kDevErr;
    return kSc;
}

DpError ProgrammerInterface::Shutdown()
{
    if (hal_->isOpen()) hal_->Close();
    return kSc;
}

DpError ProgrammerInterface::StartAppli()
{
    uint8_t buf[1];  // TODO: confirm why this is needed
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncOther, (uint8_t)kReqOtherStartAppli, 0, 0, buf,
                           sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::Polling(uint32_t timeout)
{
    uint8_t buf[4];
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpPolling, timeout >> 16,
                           static_cast<uint16_t>(timeout), buf, sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::TransceiveIn(uint8_t *data, size_t size, bool result_in)
{
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpTransceive, 3000, 0,
                           (void *)data, (uint32_t)size)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::TransceiveOut(uint8_t *data, size_t size, bool result_in)
{
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpTransceive, 0,
                            result_in ? 1 : 0, (void *)data, (uint32_t)size)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::PowerOn(DevPowerChan chan)
{
    assert(chan < kPowerChanMax);
    if (chan == kPowerVcc)
        return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, kReqEpPowerVcc,
                             power_vcc_map_.find(power_val_[chan])->second, 0x4 | ((uint16_t)flash_series_))
                   ? kDevErr
                   : kSc;
    else if (chan == kPowerVpp)  // SetVpp4IAP(on)
    {
        uint8_t buf[1] = {0};  // TODO: confirm why this is needed
        if (hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetPowerVpp,
                          (uint16_t)power_vcc_map_.find(power_val_[chan])->second, 0))
            return kDevErr;  // no override
        return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncOther, 0, 0, 0, buf, sizeof(buf)) ? kDevErr : kSc;
    }
    return kInvalidParam;
}

DpError ProgrammerInterface::PowerOff(DevPowerChan chan)
{
    assert(chan < kPowerChanMax);
    if (chan == kPowerVcc)
        return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, kReqEpPowerVcc, (uint16_t)kVccOff,
                             0x4 | ((uint16_t)flash_series_))
                   ? kDevErr
                   : kSc;
    else if (chan == kPowerVpp)  // SetVpp4IAP(off)
    {
        uint8_t buf[1] = {0};  // TODO: confirm why this is needed
        return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncOther, 1, 0, 0, buf, sizeof(buf)) ? kDevErr : kSc;
    }
    return kInvalidParam;
}

DpError ProgrammerInterface::setPowerConfig(DevPowerChan chan, int mvolt)
{
    assert(chan < kPowerChanMax);
    auto x = power_vcc_map_.find(mvolt);
    if (x == power_vcc_map_.end()) return kInvalidParam;
    power_val_[chan] = mvolt;
    return kSc;
}

DpError ProgrammerInterface::setIO(uint32_t io_state)
{
    io_state_ = io_state;
    return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetIO, (io_state & 0xF) | 0x70, 0)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::setLED(programmer_led_e led_state) {}

DpError ProgrammerInterface::setBusClock(int freq)
{
    auto x = bus_clock_map_.find(freq);
    if (x == bus_clock_map_.end()) return kInvalidParam;
    bus_clock_ = freq;
    return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetSpiClock, x->second, 0)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::setCS(uint16_t value)
{
    uint8_t buf[1] = {0};  // TODO: confirm why this is needed
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetCs, value, 0, buf,
                            sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::setIoMode(uint32_t io_mode)
{
    uint8_t buf[1] = {0};                      // TODO: confirm why this is needed
    if (prog_type_ == kProgSF100) return kSc;  // sf100 does not support this
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetIoMode, (uint16_t)io_mode, 0,
                            buf, sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::setTargetFlash(target_flash_e mode)
{
    uint8_t buf[1] = {0};  // TODO: confirm why this is needed
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetTargetFlash, (uint16_t)mode,
                            0, buf, sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::ReadOnBoardFlash(uint8_t data[16], bool is_read_uid)
{
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpReadOnBoardFlash, 0,
                           is_read_uid ? 1 : 0, data, 16)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::WriteSF600UID(uint32_t uid, uint8_t manuID)
{
    uint8_t buf[16] = {0};
    buf[0] = (uint8_t)(uid >> 16);
    buf[1] = (uint8_t)(uid >> 8);
    buf[2] = (uint8_t)(uid);
    buf[3] = (uint8_t)(manuID);
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpWriteEEPROM, 0, 0, buf,
                            sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::setStandAlone(bool enable)
{
    uint8_t buf[1];
    if (prog_type_ == kProgSimulator || prog_type_ == kProgSF100) return kSc;
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpStandAlone, enable ? 1 : 0, 0,
                            buf, sizeof(buf))
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterface::ReadDeviceInfo()
{
    std::string hardware_ver_, firmware_ver;
    char data[32] = {0};
    if (hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpGetProgInfo, 0, 0, (void *)data,
                        sizeof(data)))
        return kDevErr;
    // get prog type
    prog_name_.assign(data, data + 8);
    if (prog_type_map_.find(prog_name_) == prog_type_map_.end())
    {
        prog_type_ = kProgUnknown;
        return kUnsupported;
    }
    prog_type_ = prog_type_map_.find(prog_name_)->second;
    if (prog_type_ == kProgSimulator)
    {
        hardware_ver_ = "Simulator";
        firmware_ver = "000000";
    }
    else if (prog_type_ == kProgSF600G2)
    {
        hardware_ver_.assign(data + 12, data + 12 + 9);
        firmware_ver.assign(data + 25, data + 25 + 5);
    }
    else
    {
        firmware_ver.assign(data + 10, data + 10 + 7);
        if (prog_type_ == kProgSF600)
            hardware_ver_.assign(data + 20, data + 20 + 4);
        else if (prog_type_ == kProgSF700)
            hardware_ver_.assign(data + 21, data + 21 + 4);
        else
            hardware_ver_ = "N/A";
    }
    // get firmware version
    std::string token;
    std::istringstream version_stream(firmware_ver);
    firmware_ver_ = 0;
    while (std::getline(version_stream, token, '.'))
    {
        firmware_ver_ <<= 8;
        firmware_ver_ |= std::stoi(token);
    }
    // make is_new_command
    is_new_command = false;
    if (prog_type_ == kProgSF100)
    {
        if (firmware_ver_ >= FirmwareVersionConvert(5, 5, 0)) is_new_command = true;
    }
    else if (prog_type_ == kProgSF600)
    {
        if (firmware_ver_ == FirmwareVersionConvert(6, 9, 0))
            is_new_command = true;
        else if (firmware_ver_ > FirmwareVersionConvert(7, 0, 1))
            is_new_command = true;
    }
    // dump
    if (prog_type_ == kProgSF700 || prog_type_ == kProgSF600G2) is_new_command = true;
    DP_LOG(WARNING) << "Hardware Version: " << hardware_ver_;
    DP_LOG(WARNING) << "Firmware Version: " << firmware_ver << "HEX: " << std::hex << firmware_ver_;
    return kSc;
}

DpError ProgrammerInterface::ReadFpgaVersion()
{
    uint8_t buf[2];
    fpga_ver_ = 0;
    if (prog_type_ == kProgSF100 || prog_type_ == kProgSimulator) return kSc;
    if (hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpGetFPGAVersion, 0, 0, (void *)buf,
                        sizeof(buf)))
        return kDevErr;
    fpga_ver_ = ((uint32_t)buf[0] << 8) | buf[1];
    return kSc;
}

DpError ProgrammerInterface::ReadBoardUID()
{
    uint16_t buf_uid[16];
    board_uid_ = 0;
    if (prog_type_ == kProgSF600G2 || prog_type_ == kProgSF700)
    {
        uint8_t buf[6] = {0, 0, 0, 2, 0, 0};
        uint8_t bulk_buf[512];
        // SPEC: read twice
        if (hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpReadSerialNumber, 0, 0,
                             (void *)buf, sizeof(buf)))
            return kDevErr;
        if (hal_->BulkIn(bulk_buf, sizeof(bulk_buf)) != sizeof(bulk_buf)) return kDevErr;

        if (hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpReadSerialNumber, 0, 0,
                             (void *)buf, sizeof(buf)))
            return kDevErr;
        if (hal_->BulkIn(bulk_buf, sizeof(bulk_buf)) != sizeof(bulk_buf)) return kDevErr;
        memcpy(buf_uid, bulk_buf, 16);
        board_uid_ = (uint32_t)buf_uid[2] << 16 | (uint32_t)buf_uid[1] << 8 | buf_uid[0];
    }
    else if (prog_type_ == kProgSF600)
    {
        if (!is_new_command)
            if (hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpReadOnBoardFlash, 0, 1,
                                buf_uid, sizeof(buf_uid)))
                return kDevErr;
            else if (hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpReadOnBoardFlash, 1, 0,
                                     buf_uid, sizeof(buf_uid)))
                return kDevErr;
        board_uid_ = (uint32_t)buf_uid[0] << 16 | (uint32_t)buf_uid[1] << 8 | buf_uid[2];
    }
    else if (prog_type_ == kProgSF100)
    {
        if (hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncOther, (uint8_t)kReqOtherREAD_EX, 0, 0xEF00, buf_uid, 3))
            return kDevErr;
        board_uid_ = (uint32_t)buf_uid[0] << 16 | (uint32_t)buf_uid[1] << 8 | buf_uid[2];
    }
    return kSc;
}

void ProgrammerInterface::Dump()
{
    std::cout << "Programmer Name: " << prog_name_ << std::endl;
    std::cout << "Programmer Type: " << prog_type_ << std::endl;
    std::cout << "Hardware Version: " << hardware_ver_ << std::endl;
    std::cout << "Firmware Version: " << firmware_ver_ << std::endl;
    std::cout << "FPGA Version: " << fpga_ver_ << std::endl;
    std::cout << "Board UID: " << board_uid_ << std::endl;
    std::cout << "IO1 Selection: " << io1_selection_ << std::endl;
    std::cout << "IO4 Selection: " << io4_selection_ << std::endl;
    std::cout << "IO State: " << io_state_ << std::endl;
    std::cout << "New Command: " << (is_new_command ? "true" : "false") << std::endl;
    std::cout << "Power VCC: " << power_val_[kPowerVcc] << std::endl;
    std::cout << "Power VPP: " << power_val_[kPowerVpp] << std::endl;
    std::cout << "Bus Clock: " << bus_clock_ << std::endl;
    std::cout << "Flash Series: " << flash_series_ << std::endl;
}
// ----------------------------------------------------------------------------

DpError ProgrammerInterfaceNew::TransceiveIn(uint8_t *data, size_t size, bool result_in)
{
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpTransceive, 1, 0, (void *)data,
                           (uint32_t)size)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterfaceNew::TransceiveOut(uint8_t *data, size_t size, bool result_in)
{
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpTransceive, result_in ? 1 : 0,
                            0, (void *)data, (uint32_t)size)
               ? kDevErr
               : kSc;
}

DpError ProgrammerInterfaceNew::PowerOn(DevPowerChan chan)
{
    assert(chan < kPowerChanMax);
    if (chan == kPowerVcc)
        return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, kReqEpPowerVcc,
                             power_vcc_map_.find(power_val_[chan])->second, 0)
                   ? kDevErr
                   : kSc;
    else if (chan == kPowerVpp)  // SetVpp4IAP(on)
    {
        uint8_t buf[1] = {0};  // TODO: confirm why this is needed
        if (hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetPowerVpp,
                          (uint16_t)power_vcc_map_.find(power_val_[chan])->second, 0))
            return kDevErr;  // no override
        return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncOther, 0, 0, 0, buf, sizeof(buf)) ? kDevErr : kSc;
    }
    return kInvalidParam;
}

DpError ProgrammerInterfaceNew::PowerOff(DevPowerChan chan)
{
    assert(chan < kPowerChanMax);
    if (chan == kPowerVcc)
        return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, kReqEpPowerVcc, (uint16_t)kVccOff,
                             0x4 | ((uint16_t)flash_series_))
                   ? kDevErr
                   : kSc;
    else if (chan == kPowerVpp)  // SetVpp4IAP(off)
    {
        uint8_t buf[1] = {0};  // TODO: confirm why this is needed
        return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncOther, 1, 0, 0, buf, sizeof(buf)) ? kDevErr : kSc;
    }
    return kInvalidParam;
}

DpError ProgrammerInterfaceNew::setIO(uint32_t io_state)
{
    io_state_ = io_state;
    return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpSetIO, io_state, 0x7) ? kDevErr
                                                                                                           : kSc;
}

DpError ProgrammerInterfaceNew::ReadOnBoardFlash(uint8_t data[16], bool is_read_uid)
{
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)kReqEpReadOnBoardFlash,
                           is_read_uid ? 1 : 0, 0, data, 16)
               ? kDevErr
               : kSc;
}

}  // namespace dp
