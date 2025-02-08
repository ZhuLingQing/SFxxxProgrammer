#include "socket_sim_prog.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include "dp_error.h"
#include "dp_spi_flash_command_set.h"
#include "dp_type.h"

#include "flash_info.hpp"
#include "hal_proto.pb.h"
#include "hal_skt_device.hpp"

namespace sim
{
// SocketSimProg

#define MAKE_CTRL(name)                          \
    {                                            \
        kReqEp##name, &SocketSimProg::Ctrl##name \
    }
/* static */ std::map<uint8_t, int (SocketSimProg::*)(uint64_t ctrl)> SocketSimProg::endpoint_ctrl_map_ = {
    MAKE_CTRL(Transceive),       MAKE_CTRL(Polling),     MAKE_CTRL(SetPowerVpp), MAKE_CTRL(SetTargetFlash),
    MAKE_CTRL(ReadOnBoardFlash), MAKE_CTRL(WriteEEPROM), MAKE_CTRL(SetIO),       MAKE_CTRL(GetProgInfo),
    MAKE_CTRL(SetPowerVcc),      MAKE_CTRL(StandAlone),  MAKE_CTRL(SetCs),       MAKE_CTRL(SetIoMode),
};

SocketSimProg::SocketSimProg(int port) : port_(port), skt_fd_(-1) {}

int SocketSimProg::run()
{
    int rc;
    proto_hal::HalSktCtrlPacket pkt;
    std::vector<uint8_t> data;
    if ((skt_fd_ = create()) < 0) return -1;
    if ((cli_fd_ = wait()) < 0) return -1;

    while (true)
    {
        if ((rc = ProtoReadCtrl(pkt)) < 0) break;
        if (pkt.type() == proto_hal::kCtrlOut && pkt.payload().size())
            payload_.assign(reinterpret_cast<const uint8_t *>(pkt.payload().data()),
                            reinterpret_cast<const uint8_t *>(pkt.payload().data()) + pkt.payload().size());
        if ((rc = ProtoProcess(pkt.type(), pkt.ctrl_word())) < 0) break;
        if (pkt.type() == proto_hal::kCtrlIn && payload_.size())
        {
            proto_hal::HalSktCtrlPacket pkt;
            pkt.set_type(proto_hal::kCtrlIn);
            pkt.set_ctrl_word(pkt.ctrl_word());
            std::string byte_string;
            byte_string.assign(reinterpret_cast<uint8_t *>(payload_.data()),
                               reinterpret_cast<uint8_t *>(payload_.data()) + payload_.size());
            pkt.set_payload(byte_string);
            if ((rc = ProtoSendCtrl(pkt)) < 0) break;
        }
        payload_.clear();
    }
    (rc ? std::cerr : std::cout) << "SocketSimProg exit with code " << rc << std::endl;
    return rc;
}

int SocketSimProg::ProtoProcess(proto_hal::PacketType type, uint64_t ctrl_word)
{
    int r;
    switch (type)
    {
        case proto_hal::kCtrlOut:
        case proto_hal::kCtrlIn:
            std::map<uint8_t, int (sim::SocketSimProg::*)(uint64_t ctrl)>::iterator fp;
            dp::hal_ctrl ctrl(ctrl_word);
            if ((ctrl.request_type() & 0x7F) == dp::ProgrammerHal::kVendorFuncOther)
                fp = other_ctrl_map_.find(ctrl.request());
            else if ((ctrl.request_type() & 0x7F) == dp::ProgrammerHal::kVendorFuncEndpoint)
                fp = endpoint_ctrl_map_.find(ctrl.request());
            else
                fp = endpoint_ctrl_map_.end();
            if (fp == other_ctrl_map_.end() || fp == endpoint_ctrl_map_.end())
            {
                std::cerr << "Invalid request" << std::endl;
                return kInvalidParam;
            }
            if ((r = (this->*(fp->second))(ctrl_word)) != 0)
            {
                std::cerr << "kCtrl(" << std::hex << ctrl_word << ") Exe err " << r << std::endl;
                return kErrExec;
            }
            break;
        case proto_hal::kBulkOut:
            std::cerr << "Not support yet." << std::endl;
            return kInvalidParam;
            break;
        case proto_hal::kBulkIn:
            std::cerr << "Not support yet." << std::endl;
            break;
        case proto_hal::kShutdown:
            std::cerr << "Not support yet." << std::endl;
            break;
        default:
            std::cerr << "Invalid packet type" << std::endl;
            return kInvalidParam;
    }
    return 0;
}

int SocketSimProg::create()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 绑定地址和端口
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port_);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server is listening on port " << port_ << "..." << std::endl;
    return serverSocket;
}

int SocketSimProg::wait()
{
    // 接受客户端连接
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(skt_fd_, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket == -1)
    {
        std::cerr << "Failed to accept client connection\n";
        close(skt_fd_);
        skt_fd_ = -1;
        return -1;
    }

    std::cout << "Client connected\n";
    return clientSocket;
}

int SocketSimProg::ProtoSendCtrl(proto_hal::HalSktCtrlPacket &pkt)
{
    std::string serializedData;
    pkt.SerializeToString(&serializedData);
    uint32_t dataLength = htonl(serializedData.size());
    if (send(cli_fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
    {
        fprintf(stderr, "Failed to send data length\n");
        return -1;
    }
    if (send(cli_fd_, serializedData.data(), serializedData.size(), 0) != serializedData.size())
    {
        fprintf(stderr, "Failed to send data\n");
        return -1;
    }
    return 0;
}

int SocketSimProg::ProtoReadCtrl(proto_hal::HalSktCtrlPacket &pkt)
{
    uint32_t dataLength;
    if (recv(cli_fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
    {
        fprintf(stderr, "Failed to read data length\n");
        return -1;
    }
    dataLength = ntohl(dataLength);  // 转换字节序

    char buffer[kMaxBufferSize];
    if (recv(cli_fd_, buffer, dataLength, 0) != dataLength)
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

uint32_t SocketSimProg::ProtoSendData(void *buf, uint32_t size)
{
    uint32_t naccess = 0, dataLength;
    uint8_t *p = reinterpret_cast<uint8_t *>(buf);
    proto_hal::HalSktDataPacket pkt;
    std::string serializedData;
    std::string byte_string;
    while (naccess < size)
    {
        byte_string.assign(p, p + kBulkSize);
        pkt.set_data(byte_string);

        pkt.SerializeToString(&serializedData);
        dataLength = htonl(serializedData.size());
        if (send(cli_fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
        {
            fprintf(stderr, "Failed to send data length\n");
            return naccess;
        }
        if (send(cli_fd_, serializedData.data(), serializedData.size(), 0) != serializedData.size())
        {
            fprintf(stderr, "Failed to send data\n");
            return naccess;
        }
        p += kBulkSize;
        naccess += kBulkSize;
    }
    return naccess;
}

uint32_t SocketSimProg::ProtoReadData(void *buf, uint32_t size)
{
    uint32_t naccess = 0, dataLength;
    uint8_t *p = reinterpret_cast<uint8_t *>(buf);
    char buffer[kMaxBufferSize];
    proto_hal::HalSktDataPacket pkt;
    std::string serializedData;
    std::string byte_string;
    while (naccess < size)
    {
        if (recv(cli_fd_, &dataLength, sizeof(dataLength), 0) != sizeof(dataLength))
        {
            fprintf(stderr, "Failed to read data length\n");
            return naccess;
        }
        dataLength = ntohl(dataLength);  // 转换字节序

        if (recv(cli_fd_, buffer, dataLength, 0) != dataLength)
        {
            fprintf(stderr, "Failed to read data\n");
            return naccess;
        }

        if (!pkt.ParseFromArray(buffer, dataLength))
        {
            fprintf(stderr, "Failed to parse data\n");
            return naccess;
        }
        p += kBulkSize;
        naccess += kBulkSize;
    }
    return naccess;
}

// int SocketSimProg::CtrlXXX(uint64_t ctrl_word)
int SocketSimProg::CtrlTransceive(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlPolling(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlSetPowerVpp(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlSetTargetFlash(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlReadOnBoardFlash(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlWriteEEPROM(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlSetIO(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlGetProgInfo(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlSetPowerVcc(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlStandAlone(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlSetCs(uint64_t ctrl_word) { return 0; }
int SocketSimProg::CtrlSetIoMode(uint64_t ctrl_word) { return 0; }

}  // namespace sim
