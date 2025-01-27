#include "programmer_interface.hpp"

#define URB_FUNCTION_VENDOR_DEVICE 0x0017
#define URB_FUNCTION_VENDOR_INTERFACE 0x0018
#define URB_FUNCTION_VENDOR_ENDPOINT 0x0019
#define URB_FUNCTION_VENDOR_OTHER 0x0020

#define URB_FUNCTION_CLASS_DEVICE 0x001A
#define URB_FUNCTION_CLASS_INTERFACE 0x001B
#define URB_FUNCTION_CLASS_ENDPOINT 0x001C
#define URB_FUNCTION_CLASS_OTHER 0x001F

#define VENDOR_DIRECTION_IN 0x80
#define VENDOR_DIRECTION_OUT 0x00

namespace dp
{

DpError ProgrammerInterface::Init(uint32_t timeout)
{
    if (hal_->isOpen()) return kAlreadyOwned;
    if (hal_->Open() < 0) return kDevErr;
    if (StartAppli() < 0) return kDevErr;
    if (AssignProg() < 0) return kDevErr;
    if (LeaveStandaloneMode() < 0) return kDevErr;
    if (QueryBoard() < 0) return kDevErr;
    if (CheckProgrammerInfo() < 0) return kDevErr;
    return kSc;
}
int ProgrammerInterface::StartAppli()
{
    uint8_t buf[1];
    return hal_->ControlIn(URB_FUNCTION_VENDOR_OTHER,0xb,0,0,buf, sizeof(buf));
}
int ProgrammerInterface::AssignProg()
{
    uint8_t buffer[32];
    int r;
    uint32_t fw[3];

    prog_type_ = kProgUnknown;
    if ((r = hal_->ControlIn(URB_FUNCTION_VENDOR_ENDPOINT,0x8,0,0,buffer,sizeof(buffer))) < 0) return r;
    sscanf((char*)&buffer[8], "V:%d.%d.%d", &fw[0], &fw[1], &fw[2]);
    firmware_ver_ = ((fw[0] << 16) | (fw[1] << 8) | fw[2]);

    return 0;
}
int ProgrammerInterface::LeaveStandaloneMode() { return -1; }
int ProgrammerInterface::QueryBoard() { return -1; }
int ProgrammerInterface::CheckProgrammerInfo() { return -1; }

}  // namespace dp
