#ifndef PROGRAMMER_HAL_HPP
#define PROGRAMMER_HAL_HPP

#include "dp_config.h"

#include <cstdint>

namespace dp
{
class ProgrammerHal
{
   public:
    ProgrammerHal() : kBulkSize_(CONFIG_BULK_BUFFER_SIZE) {}
    virtual int Open(void) { return 0; }
    virtual void Close(void) {}
    virtual bool isOpen(void) { return false; }
    virtual int Control(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
                        uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) = 0;
    virtual int ControlOut(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf,
                           uint32_t size, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) = 0;
    virtual int ControlIn(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf,
                          uint32_t size, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) = 0;
    virtual int BulkOut(void *buf, uint32_t size = 0, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) = 0;
    virtual int BulkIn(void *buf, uint32_t size = 0, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) = 0;
    size_t getBulkSize() const { return kBulkSize_; }
    void setBulkSize(size_t size) { kBulkSize_ = size; }
    private:
    size_t kBulkSize_;
};
}  // namespace dp

#endif  // PROGRAMMER_HAL_HPP