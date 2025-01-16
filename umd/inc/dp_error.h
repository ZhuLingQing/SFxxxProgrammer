#ifndef __DP_ERROR_H__
#define __DP_ERROR_H__

// Invalid linux file descriptor value
const int kInvalidFd = -1;

typedef enum
{
    // Run without any error
    kSc = 0,
    // Out of memory
    kOutOfMemory = -1,
    // The pointer address is invalid
    kInvalidPtr = -2,
    // The parameter is invalid
    kInvalidParam = -3,
    // Internal error
    kInternalErr = -4,
    // Generic error in software execution
    kErrExec = -5,
    // Invalid device id
    kInvalidDevIdx = -6,
    // Invalid device vid/pid
    kInvalidVidDid = -7,
    // Device is busy
    kDevBusy = -8,
    // Invalid driver instance id
    kInvalidInstId = -9,
    // Lock the file fail
    kFileOpenLockFail = -10,
    // re-construct the driver instance
    kAlreadyOwned = -11,
    // Operate the file which has locked
    kNotOwnedLocked = -12,
    // Sw execution timeout
    kTimeout = -13,
    // Epoll error
    kEpollErr = -14,
    // read file/buffer fail
    kReadErr = -15,
    // mmap the memory fail
    kMmapFail = -16,
    // Invalid attribute request
    kDevInvalidAttri = -17,
    // Unsupported configuration
    kDevInvalidConfig = -18,
    // System call fail
    kSysCallFail = -19,
    // File/Memory can not access
    kNoneAccess = -20,
    // Write file/memory fail
    kWriteErr = -21,
    // Unsupported device or setting
    kUnsupported = -22,
    // Fetal device error
    kDevDead = -23,
    // Generic device error
    kDevErr = -24,
    // Invalid op code of instruction
    kInvalidOpCode = -25,
    // INstruction execution timeout
    kInstrTimeout = -26,
    // EDL Reodering not supported
    kHardwareUnsupported = -27,
    // try again
    kAgain = -28,
    // the signal has ready signaled
    kAlreadySignaled = -29,
    // the device or object is disabled
    kIsNotEnable = -30,
    // the object is exist
    kExist = -31,
    // Feature not implemented
    kNotImplemented = -32,
} DpError;

#endif
