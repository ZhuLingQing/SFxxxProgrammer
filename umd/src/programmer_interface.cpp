#include "programmer_interface.hpp"
#include "programmer_hal.hpp"

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

DpError ProgrammerInterface::Init(uint32_t timeout)
{
    if (hal_->isOpen()) return kSc;;
    if (hal_->Open() < 0) return kDevErr;
    // if (StartAppli() < 0) return kDevErr;
    // if (AssignProg() < 0) return kDevErr;
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
// int ProgrammerInterface::StartAppli()
// {
//     uint8_t buf[1];
//     return hal_->ControlIn(URB_FUNCTION_VENDOR_OTHER,0xb,0,0,buf, sizeof(buf));
// }
// int ProgrammerInterface::AssignProg()
// {
//     return 0;
// }
// int ProgrammerInterface::LeaveStandaloneMode() { return -1; }
// int ProgrammerInterface::QueryBoard() { return -1; }
// int ProgrammerInterface::CheckProgrammerInfo() { return -1; }

DpError ProgrammerInterface::Polling(uint32_t timeout)
{
    DP_CHECK(false) << "Undefined API::" << __PRETTY_FUNCTION__;
    return kSc;
}
DpError ProgrammerInterface::TransceiveIn(uint8_t *data, size_t size, bool result_in)
{
    return hal_->ControlIn((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)ProgrammerHal::kReqTransceive, 3000, 0,
                           (void *)data, (uint32_t)size)
               ? kDevErr
               : kSc;
}
DpError ProgrammerInterface::TransceiveOut(uint8_t *data, size_t size, bool result_in)
{
    return hal_->ControlOut((uint8_t)ProgrammerHal::kVendorFuncEndpoint, (uint8_t)ProgrammerHal::kReqTransceive, 0,
                            result_in ? 1 : 0, (void *)data, (uint32_t)size)
               ? kDevErr
               : kSc;
}
DpError ProgrammerInterface::PowerOn(DevPowerChan chan)
{
    assert(chan < kPowerChanMax);
    return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint,
                         (uint8_t)((chan == kPowerVcc) ? ProgrammerHal::kReqPowerVcc : ProgrammerHal::kReqPowerVpp),
                         power_vcc_map_.find(power_val_[chan])->second, 0x4 | ((uint16_t)flash_series_))
               ? kDevErr
               : kSc;
}
DpError ProgrammerInterface::PowerOff(DevPowerChan chan)
{
    assert(chan < kPowerChanMax);
    return hal_->Control((uint8_t)ProgrammerHal::kVendorFuncEndpoint,
                         (uint8_t)((chan == kPowerVcc) ? ProgrammerHal::kReqPowerVcc : ProgrammerHal::kReqPowerVpp),
                         (uint16_t)kVccOff, 0x4 | ((uint16_t)flash_series_))
               ? kDevErr
               : kSc;
}
DpError ProgrammerInterface::setPowerConfig(DevPowerChan chan, int mvolt)
{
    assert(chan < kPowerChanMax);
    auto x = power_vcc_map_.find(mvolt);
    if (x == power_vcc_map_.end()) return kInvalidParam;
    power_val_[chan] = mvolt;
    return kSc;
}
DpError ProgrammerInterface::setBusClock(int freq)
{
    auto x = bus_clock_map_.find(freq);
    if (x == bus_clock_map_.end()) return kInvalidParam;
    bus_clock_ = freq;
    return kSc;
}

}  // namespace dp
