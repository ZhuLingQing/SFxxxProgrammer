#ifndef HAL_SKT_DEVICE_HPP
#define HAL_SKT_DEVICE_HPP

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <cassert>
#include <string>
#include <vector>

#include "hal_proto.pb.h"
#include "programmer_hal.hpp"

namespace dp
{
struct ctrl_t
{
    uint8_t request_type;
    uint8_t request;
    uint16_t value;
    uint16_t index;
    uint16_t size;
};
class hal_ctrl
{
   public:
    hal_ctrl(uint64_t ctrl) : ctrl_(ctrl) {}
    uint64_t ctrl() const { return ctrl_; }
    uint8_t request_type() const { return ctrl_ >> 56; }
    uint8_t request() const { return ctrl_ >> 48; }
    uint16_t value() const { return ctrl_ >> 32; }
    uint16_t index() const { return ctrl_ >> 16; }
    uint16_t size() const { return ctrl_ >> 0; }
    static inline uint64_t to_ctrl(ctrl_t ctrl)
    {
        return (((uint64_t)ctrl.request_type << 56) | ((uint64_t)ctrl.request << 48) | ((uint64_t)ctrl.value << 32) |
                ((uint64_t)ctrl.index << 16) | ((uint64_t)ctrl.size & 0xFFFF));
    }

   private:
    uint64_t ctrl_;
};
class HalSktDevice : public ProgrammerHal
{
   public:
    HalSktDevice(std::string &ip, int port = 8080) : ProgrammerHal(), ip_(ip), port_(port), fd_(-1) {}
    int Open() override
    {
        if (isOpen()) Close();
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0)
        {
            fprintf(stderr, "Failed to create socket %d\n", fd_);
            return fd_;
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port_);  // 服务器端口
        inet_pton(AF_INET, ip_.c_str(), &serverAddress.sin_addr);
        if (connect(fd_, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            fprintf(stderr, "Failed to connect to server\n");
            close(fd_);
            fd_ = -1;
            return -1;
        }
        return 0;
    }
    void Close() override
    {
        if (isOpen())
        {
            close(fd_);
            fd_ = -1;
        }
    }
    bool isOpen() override { return fd_ != -1; }
    int Control(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
                uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        assert(fd_ != -1);
        proto_hal::HalSktCtrlPacket pkt;
        pkt.set_type(proto_hal::kCtrlIn);
        pkt.set_ctrl_word(hal_ctrl::to_ctrl({request_type, request, value, index, 0}));

        return ProtoSendCtrl(pkt);
    }
    int ControlOut(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf, uint32_t size,
                   uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        assert(fd_ != -1);
        proto_hal::HalSktCtrlPacket pkt;
        pkt.set_type(proto_hal::kCtrlOut);
        pkt.set_ctrl_word(hal_ctrl::to_ctrl({request_type, request, value, index, (uint16_t)size}));
        if (buf && size)
        {
            std::string byte_string;
            byte_string.assign(reinterpret_cast<uint8_t *>(buf), reinterpret_cast<uint8_t *>(buf) + size);
            pkt.set_payload(byte_string);
        }

        return ProtoSendCtrl(pkt);
    }
    int ControlIn(uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, void *buf, uint32_t size,
                  uint32_t timeout = CONFIG_DEFAULT_TIMEOUT) override
    {
        int r;
        assert(fd_ != -1);
        proto_hal::HalSktCtrlPacket pkt;
        pkt.set_type(proto_hal::kCtrlIn);
        pkt.set_ctrl_word(hal_ctrl::to_ctrl({request_type, request, value, index, (uint16_t)size}));
        if ((r = ProtoSendCtrl(pkt)) < 0) return r;

        if (size == 0) return 0;
        if ((r = ProtoReadCtrl(pkt)) < 0) return r;
        if (pkt.type() != proto_hal::kCtrlIn) return -2;
        if (buf && size)
        {
            std::string byte_string = pkt.payload();
            if (byte_string.size() != size)
            {
                fprintf(stderr, "Data size mismatch\n");
                return -2;
            }
            memcpy(buf, byte_string.data(), size);
        }

        return 0;
    }
    int BulkOut(void *buf, uint32_t size, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) override
    {
        int r;
        assert(fd_ != -1);
        assert(buf);
        if (size == 0) size = getBulkSize();
        assert(size % getBulkSize() == 0);

        proto_hal::HalSktCtrlPacket pkt;
        pkt.set_type(proto_hal::kBulkOut);
        pkt.set_ctrl_word(size);

        if ((r = ProtoSendCtrl(pkt)) < 0) return r;
        if (ProtoSendData(buf, size) != size) return -2;

        return 0;
    }
    int BulkIn(void *buf, uint32_t size, uint32_t timeout = CONFIG_DEFAULT_TIMEOUT, int ep_index = 0) override
    {
        int r;
        assert(fd_ != -1);
        assert(buf);
        if (size == 0) size = getBulkSize();
        assert(size % getBulkSize() == 0);

        proto_hal::HalSktCtrlPacket pkt;
        pkt.set_type(proto_hal::kBulkIn);
        pkt.set_ctrl_word(size);

        if ((r = ProtoSendCtrl(pkt)) < 0) return r;
        if (ProtoReadData(buf, size) != size) return -2;

        return 0;
    }

   private:
    int ProtoSendCtrl(proto_hal::HalSktCtrlPacket &pkt)
    {
        std::string serializedData;
        pkt.SerializeToString(&serializedData);
        uint32_t dataLength = htonl(serializedData.size());
        if (send(fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
        {
            fprintf(stderr, "Failed to send data length\n");
            return -1;
        }
        if (send(fd_, serializedData.data(), serializedData.size(), 0) != serializedData.size())
        {
            fprintf(stderr, "Failed to send data\n");
            return -1;
        }
        return 0;
    }
    int ProtoReadCtrl(proto_hal::HalSktCtrlPacket &pkt)
    {
        uint32_t dataLength;
        if (recv(fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
        {
            fprintf(stderr, "Failed to read data length\n");
            return -1;
        }
        dataLength = ntohl(dataLength);  // 转换字节序

        char buffer[kMaxBufferSize];
        if (recv(fd_, buffer, dataLength, 0) != dataLength)
        {
            fprintf(stderr, "Failed to read data\n");
            return -1;
        }

        if (!pkt.ParseFromArray(buffer, dataLength))
        {
            fprintf(stderr, "Failed to parse data\n");
            return -1;
        }
        return 0;
    }
    uint32_t ProtoSendData(void *buf, uint32_t size)
    {
        uint32_t naccess = 0, dataLength;
        uint8_t *p = reinterpret_cast<uint8_t *>(buf);
        proto_hal::HalSktDataPacket pkt;
        std::string serializedData;
        std::string byte_string;
        while (naccess < size)
        {
            byte_string.assign(p, p + getBulkSize());
            pkt.set_data(byte_string);

            pkt.SerializeToString(&serializedData);
            dataLength = htonl(serializedData.size());
            if (send(fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
            {
                fprintf(stderr, "Failed to send data length\n");
                return naccess;
            }
            if (send(fd_, serializedData.data(), serializedData.size(), 0) != serializedData.size())
            {
                fprintf(stderr, "Failed to send data\n");
                return naccess;
            }
            p += getBulkSize();
            naccess += getBulkSize();
        }
        return naccess;
    }
    uint32_t ProtoReadData(void *buf, uint32_t size)
    {
        uint32_t naccess = 0, dataLength;
        uint8_t *p = reinterpret_cast<uint8_t *>(buf);
        char buffer[kMaxBufferSize];
        proto_hal::HalSktDataPacket pkt;
        std::string serializedData;
        std::string byte_string;
        while (naccess < size)
        {
            if (recv(fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
            {
                fprintf(stderr, "Failed to read data length\n");
                return naccess;
            }
            dataLength = ntohl(dataLength);  // 转换字节序

            if (recv(fd_, buffer, dataLength, 0) != dataLength)
            {
                fprintf(stderr, "Failed to read data\n");
                return naccess;
            }

            if (!pkt.ParseFromArray(buffer, dataLength))
            {
                fprintf(stderr, "Failed to parse data\n");
                return naccess;
            }
            p += getBulkSize();
            naccess += getBulkSize();
        }
        return naccess;
    }
    std::string ip_;
    int port_;
    int fd_;
    static const size_t kMaxBufferSize = CONFIG_PROTO_BUFFER_SIZE;
};  // class HalSktDevice

}  // namespace dp

#endif  // #ifndef HAL_SKT_DEVICE_HPP
