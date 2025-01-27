#ifndef HAL_USB_DEVICE_HPP
#define HAL_USB_DEVICE_HPP

#include <stdint.h>
#include <stdio.h>
#include <cassert>
#include <vector>
#include "dp_config.h"
#include "programmer_hal.hpp"

#ifdef __FreeBSD__
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

static inline int libusb_get_endpoint_descriptor(libusb_device_handle *dev, uint8_t desc_index, uint16_t langid,
                                                 unsigned char *data, int length)
{
    return libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                   (uint16_t)((LIBUSB_DT_ENDPOINT << 8) | desc_index), langid, data, (uint16_t)length,
                                   CONFIG_DEFAULT_TIMEOUT);
}

namespace dp
{
class HalUsbDevice : public ProgrammerHal
{
   public:
    enum vendor_request_function_e
    {
        kVendorFuncDevice = 0x40,
        kVendorFuncInterface = 0x41,
        kVendorFuncEndpoint = 0x42,
        kVendorFuncOther = 0x43
    };
    HalUsbDevice(libusb_device *device) : ProgrammerHal(), device_(device), handle_(nullptr) {}
    int Open() override
    {
        int r;
        if (handle_) Close();
        if ((r = libusb_open(device_, &handle_)) < 0) goto error_handler;
        if (nullptr == handle_) return -1;
        if ((r = libusb_set_configuration(handle_, 1)) < 0) goto error_handler;
        if ((r = libusb_claim_interface(handle_, 0)) < 0) goto error_handler;
        FindBulkEndpoints();
        return 0;
    error_handler:
        fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, libusb_error_name(r));
        return r;
    }
    void Close() override
    {
        if (nullptr == handle_) return;
        libusb_release_interface(handle_, 0);
        libusb_close(handle_);
    }
    bool isOpen() override { return handle_ != nullptr; }
    int Control(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
                uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        return ControlOut(request_type, request, value, index, nullptr, 0, timeout);
    }
    int ControlOut(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf, uint32_t size,
                   uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        assert(handle_);
        int r = libusb_control_transfer(handle_, request_type | LIBUSB_ENDPOINT_OUT, request, value, index,
                                        reinterpret_cast<uint8_t *>(buf), size, timeout);
        if (r)
        {
            fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, libusb_error_name(r));
            return -1;
        }
        return 0;
    }
    int ControlIn(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf, uint32_t size,
                  uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        assert(handle_);
        int r = libusb_control_transfer(handle_, request_type | LIBUSB_ENDPOINT_IN, request, value, index,
                                        reinterpret_cast<uint8_t *>(buf), size, timeout);
        if (r)
        {
            fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, libusb_error_name(r));
            return -1;
        }
        return 0;
    }
    int BulkOut(void *buf, uint32_t size = 0, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) override
    {
        assert(handle_);
        assert(buf);
        if (size == 0) size = getBulkSize();
        assert(size % getBulkSize() == 0);
        if (ep_index >= bulk_out_ep_.size()) return -1;
        int n_access;
        int r = libusb_bulk_transfer(handle_, bulk_out_ep_[ep_index], reinterpret_cast<uint8_t *>(buf), size, &n_access,
                                     timeout);
        if (r < 0)
        {
            fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, libusb_error_name(r));
            return r;
        }
        return n_access;
    }
    int BulkIn(void *buf, uint32_t size = 0, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) override
    {
        assert(handle_);
        assert(buf);
        if (size == 0) size = getBulkSize();
        assert(size % getBulkSize() == 0);
        if (ep_index >= bulk_out_ep_.size()) return -1;
        int n_access;
        int r = libusb_bulk_transfer(handle_, bulk_in_ep_[ep_index], reinterpret_cast<uint8_t *>(buf), size, &n_access,
                                     timeout);
        if (r < 0)
        {
            fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, libusb_error_name(r));
            return r;
        }
        return n_access;
    }
   private:
    int FindBulkEndpoints()
    {
        int r, ep;
        uint8_t buf[256];
        struct libusb_endpoint_descriptor *descr = reinterpret_cast<struct libusb_endpoint_descriptor *>(buf);
        bulk_in_ep_.clear();
        bulk_out_ep_.clear();
        for (ep = 0; ep < 16; ep++)
        {
            r = libusb_get_endpoint_descriptor(handle_, ep, 0, buf, sizeof(buf));
            if (r < 0) return 0;
            if ((descr->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK)
            {
                if (descr->bEndpointAddress & LIBUSB_ENDPOINT_IN)
                    bulk_in_ep_.push_back(descr->bEndpointAddress);
                else
                    bulk_out_ep_.push_back(descr->bEndpointAddress);
                setBulkSize(descr->wMaxPacketSize);
            }
        }
        return 0;
    }
    libusb_device *device_;
    libusb_device_handle *handle_;

    std::vector<int> bulk_out_ep_;
    std::vector<int> bulk_in_ep_;
};  // class HalUsbDevice

}  // namespace dp

#endif  // #ifndef HAL_USB_DEVICE_HPP
