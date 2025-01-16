#ifndef FLASH_MEMORY_HPP
#define FLASH_MEMORY_HPP

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "dp_error.h"
#include "dp_type.h"

namespace dp
{
class Programmer;
class ProgrammerInterface;
class FlashInfo;

class FlashInterface
{
    enum flash_attr_e
    {
        kAttrTransSize,
        kAttrTransOut,
        kAttrTransIn,
        kAttrClockFreq,
        kAttrIoMode,
    };
    enum flash_cmd_e
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
    enum flash_status_e
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

   public:
    // FlashInterface(ProgrammerInterface &prog_interface, const FlashInfo *flash_info, int site_index = 0);
    FlashInterface(Programmer *prog_, int site_index = 0);
    // Programmer &getProgrammer() { return programmer_; }
    int getIndex() const { return site_index_; }
    // int getDieCount() const { return die_.size(); }
    // MemDie &getDie(int index) { return die_[index]; }
    const FlashInfo *getFlashInfo() const { return flash_info_; }

    [[nodiscard]] virtual DpError setAttribute(flash_attr_e attr, long value);
    [[nodiscard]] virtual DpError getAttribute(flash_attr_e attr, long &value);
    [[nodiscard]] virtual long getAttribute(flash_attr_e attr);
    [[nodiscard]] virtual bool Identify();
    [[nodiscard]] virtual DpError ReadStatus(uint8_t &status);
    [[nodiscard]] uint8_t ReadStatus();
    [[nodiscard]] virtual DpError WriteEnable();
    [[nodiscard]] virtual DpError WriteDisable();
    [[nodiscard]] virtual DpError Unprotect(uint32_t retry = 5);
    [[nodiscard]] virtual bool isProtected();
    [[nodiscard]] virtual uint32_t ReadId();
    [[nodiscard]] virtual DpError ChipErase();
    [[nodiscard]] virtual DpError BlockErase(uint32_t block_index, size_t count = 1);
    [[nodiscard]] virtual DpError PageProgram(uint32_t page_index, const void *buf, size_t size);
    [[nodiscard]] virtual DpError PageRead(uint32_t page_index, void *buf, size_t size);

    [[nodiscard]] virtual DpError Write(const void *buf, size_t size, uint64_t address = 0);
    [[nodiscard]] virtual DpError Read(void *buf, size_t size, uint64_t address = 0);
    [[nodiscard]] virtual DpError Verify(const void *buf, size_t size, uint64_t address = 0);
    [[nodiscard]] virtual DpError BlankCheck(size_t size, uint64_t address = 0);

    /// PreBatchOperation is a virtual function that is intended to be overridden in a derived class. It is used to
    /// perform any necessary setup or initialization before a batch operation. The function returns a DpError type,
    /// which indicates the result of the operation. If the operation is successful, the function returns kSc, which is
    /// a predefined constant of type DpError.
    [[nodiscard]] virtual DpError PreBatchOperation() { return kSc; }
    [[nodiscard]] virtual DpError PostBatchOperation() { return kSc; }

   protected:
    [[nodiscard]] virtual DpError pollingWEL(uint32_t retry = 5);
    [[nodiscard]] virtual DpError pollingWIP(uint32_t timeout_usec = ~0U);
    // Programmer &programmer_;
    std::recursive_mutex mutex_;

   private:
    int site_index_;
    // std::vector<MemDie> die_;
    const FlashInfo *flash_info_;
    std::shared_ptr<ProgrammerInterface> prog_interface_;
    static const uint32_t kSleepResolutionUsec_;
};

// class SpiFlashDetector : public FlashInterface
// {
// public:
//     SpiFlashDetector(std::unique_ptr<Programmer>& programmer, int site_index = 0) : FlashInterface(programmer,
//     site_index) {
//     }
//     int ReadId(std::vector<uint8_t> &id) override;
//     int ReadUID(std::vector<uint8_t> &uid) override;
//     int IoControl(struct spi_io_ctrl_t &ctrl, const uint8_t *idata, uint8_t *odata, uint32_t timeout = 0) override;
// };

// class MemDie
// {
// public:
//     MemDie(FlashInterface& flash, int die_index_ = 0) : flash_(flash), die_index_(die_index_),
//     die_info_ (flash.getFlashInfo().die_info[die_index_]) {
//     }

//     int getIndex() const { return die_index_; }
//     bool valid() const { return die_info_.die_size > 0; }
//     const struct die_info_t& getDieInfo() const { return die_info_; }

//     virtual DpError ReadId(std::vector<uint8_t> &id) { return flash_.ReadId(id); }
//     virtual DpError ReadUID(std::vector<uint8_t> &id) { return flash_.ReadUID(id); }
//     virtual DpError IoControl(struct spi_io_ctrl_t &ctrl, const uint8_t *idata, uint8_t *odata, uint32_t timeout = 0)
//     {
//         return flash_.IoControl(ctrl, idata, odata, timeout);
//     }
//     virtual DpError Erase() = 0;
//     virtual DpError BlockErase(int block_index, int count = 1) = 0;
//     virtual DpError PageProgram(long page_index, const void *buf, size_t size) = 0;
//     virtual DpError PageRead(long page_index, void *buf, size_t size) = 0;
//     virtual DpError RandomProgram(long address, const void *buf, size_t size) = 0;
//     virtual DpError RandomRead(long address, void *buf, size_t size) = 0;
// private:
//     FlashInterface& flash_;
//     int die_index_;
//     const struct die_info_t &die_info_;
// };
}  // namespace dp

#endif  // FLASH_MEMORY_HPP
