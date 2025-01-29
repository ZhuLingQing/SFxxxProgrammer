#ifndef FLASH_INTERFACE_HPP
#define FLASH_INTERFACE_HPP

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "dp_error.h"
#include "dp_type.h"

namespace dp
{
class ProgrammerInterface;
class FlashInfo;

class FlashInterface
{
    enum run_state_e
    {
        kThreadIdle = 0,
        kThreadLaunching,
        kThreadRunning,
        kThreadStopping,
        kThreadTerminated,
    };

   public:
    FlashInterface(std::shared_ptr<ProgrammerInterface> interface, const FlashInfo *flash_info, int site_index = 0);
    // Programmer &getProgrammer() { return programmer_; }
    int getIndex() const { return site_index_; }
    // int getDieCount() const { return die_.size(); }
    // MemDie &getDie(int index) { return die_[index]; }
    const FlashInfo *getFlashInfo() const { return flash_info_; }

    // [[nodiscard]] virtual DpError setAttribute(flash_attr_e attr, long value);
    // [[nodiscard]] virtual DpError getAttribute(flash_attr_e attr, long &value);
    // [[nodiscard]] virtual long getAttribute(flash_attr_e attr);
    [[nodiscard]] virtual bool Identify();
    [[nodiscard]] virtual uint32_t ReadId();

    [[nodiscard]] virtual DpError ChipErase() { return kSc; }
    [[nodiscard]] virtual DpError BlockErase(uint32_t block_index, size_t count = 1) { return kSc; }
    [[nodiscard]] virtual DpError PageProgram(uint32_t page_index, const void *buf, size_t size) { return kSc; }
    [[nodiscard]] virtual DpError PageRead(uint32_t page_index, void *buf, size_t size) { return kSc; }

    [[nodiscard]] virtual DpError Write(const void *buf, size_t size, uint64_t address = 0) { return kSc; }
    [[nodiscard]] virtual DpError Read(void *buf, size_t size, uint64_t address = 0) { return kSc; }
    [[nodiscard]] virtual DpError Verify(const void *buf, size_t size, uint64_t address = 0) { return kSc; }
    [[nodiscard]] virtual DpError BlankCheck(size_t size, uint64_t address = 0) { return kSc; }

    bool Stop()
    {
        if (run_state_ == kThreadRunning)
        {
            run_state_ = kThreadStopping;
            return true;
        }
        return false;
    }

   protected:
    [[nodiscard]] virtual DpError ReadStatus(uint16_t &status) { return kSc; }
    [[nodiscard]] uint16_t ReadStatus()
    {
        uint16_t status;
        ReadStatus(status);
        return status;
    }
    [[nodiscard]] virtual DpError WriteEnable() { return kSc; }
    [[nodiscard]] virtual DpError WriteDisable() { return kSc; }
    [[nodiscard]] virtual DpError Unprotect(uint32_t retry = 5) { return kSc; }
    [[nodiscard]] virtual bool isProtected() { return false; }

    [[nodiscard]] virtual DpError WEL(uint32_t retry = 5) { return kSc; }
    [[nodiscard]] virtual DpError WIP(uint32_t timeout_usec = ~0U) { return kSc; }

    /// PreBatchOperation is a virtual function that is intended to be overridden in a derived class. It is used to
    /// perform any necessary setup or initialization before a batch operation. The function returns a DpError type,
    /// which indicates the result of the operation. If the operation is successful, the function returns kSc, which is
    /// a predefined constant of type DpError.
    [[nodiscard]] virtual DpError PreBatchOperation() { return kSc; }
    [[nodiscard]] virtual DpError PostBatchOperation() { return kSc; }

    std::recursive_mutex mutex_;

    std::shared_ptr<ProgrammerInterface> interface_;
    const FlashInfo *flash_info_;
    int site_index_;
    run_state_e run_state_;
    // std::vector<MemDie> die_;
    static const uint32_t kSleepResolutionUsec_;
};

class FlashInterface25 : public FlashInterface
{
   public:
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

        kStatusBP = 0x9C,
    };
    [[nodiscard]] DpError ChipErase() override;
    [[nodiscard]] DpError BlockErase(uint32_t block_index, size_t count = 1) override;
    [[nodiscard]] DpError PageProgram(uint32_t page_index, const void *buf, size_t size) override;
    [[nodiscard]] DpError PageRead(uint32_t page_index, void *buf, size_t size) override;

    [[nodiscard]] DpError Write(const void *buf, size_t size, uint64_t address = 0) override;
    [[nodiscard]] DpError Read(void *buf, size_t size, uint64_t address = 0) override;
    [[nodiscard]] DpError Verify(const void *buf, size_t size, uint64_t address = 0) override;
    [[nodiscard]] DpError BlankCheck(size_t size, uint64_t address = 0) override;

   protected:
    [[nodiscard]] DpError ReadStatus(uint16_t &status) override;
    [[nodiscard]] DpError WriteEnable() override;
    [[nodiscard]] DpError WriteDisable() override;
    [[nodiscard]] DpError Unprotect(uint32_t retry = 5) override;
    [[nodiscard]] bool isProtected() override;

    [[nodiscard]] DpError WEL(uint32_t retry = 5) override;
    [[nodiscard]] DpError WIP(uint32_t timeout_usec = ~0U) override;

    // [[nodiscard]] DpError PreBatchOperation() override;
    // [[nodiscard]] DpError PostBatchOperation() override;

   private:
    int enable_quad_io_;
    int protected_val_;
    bool addr_4_byte_mode_;
};

}  // namespace dp

#endif  // FLASH_INTERFACE_HPP
