#ifndef SOCKET_PROGRAMMER_HPP
#define SOCKET_PROGRAMMER_HPP

#include <arpa/inet.h>
#include <unistd.h>

#include <cstdlib>  // getenv
#include "flash_database.hpp"
#include "programmer.hpp"

using FlashDatabase = dp::FlashDatabase;
using FlashInfo = dp::FlashInfo;

namespace dp
{
class SocketProgrammerInterface : public ProgrammerInterface
{
   public:
    SocketProgrammerInterface(void *handler) : ProgrammerInterface(handler), cs_(kCsHigh)
    {
        const char *skt_port = reinterpret_cast<const char *>(handler);
        if (nullptr == skt_port) skt_port_ = 8080;
        skt_port_ = std::stoi(skt_port);

        skt_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        DP_CHECK(skt_fd_ >= 0) << "socket create failed";

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(skt_port_);  // 服务器端口
        inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
        if (connect(skt_fd_, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            DP_LOG(FATAL) << "Failed to connect to server\n";
            close(skt_fd_);
        }
    }
    ~SocketProgrammerInterface() { close(skt_fd_); }

    DpError PowerOn() noexcept override
    {
        DP_LOG(INFO) << "" << vdd_ << "mV";
        return kSc;
    }
    DpError PowerOff() noexcept override
    {
        DP_LOG(INFO) << vdd_ << "mV";
        return kSc;
    }
    int getPowerConfig(DevPowerChan chan) noexcept override
    {
        DP_LOG(INFO) << "Chan" << chan << "->" << ((chan == kPwrVcc) ? vdd_ : 0) << "mV";
        if (chan == kPwrVcc) return vdd_;
        return 0;
    }
    DpError PowerConfig(DevPowerChan chan, int mvolt) noexcept override
    {
        DP_LOG(INFO) << "Chan" << chan << "<-" << mvolt << "mV";
        if (chan == kPwrVcc) vdd_ = mvolt;
        return kSc;
    }
    DpError TransferIn(uint8_t *data, size_t size, cs_pin_state_e cs = kCsHigh) noexcept
    {
        if (0x9F == last_cmd_code_) memcpy(data, &jedec_id_, size);
        cs_ = cs;
        if (cs_ == kCsHigh) last_cmd_code_ = 0;
        return kSc;
    }
    DpError TransferOut(const uint8_t *data, size_t size, cs_pin_state_e cs = kCsHigh) noexcept
    {
        if (cs_ == kCsHigh && size == 1 && cs == kCsKeepLow)
            last_cmd_code_ = *data;
        else
            last_cmd_code_ = 0;
        cs_ = cs;
        return kSc;
    }

   private:
    int skt_port_;
    int skt_fd_;

    uint8_t last_cmd_code_;
    cs_pin_state_e cs_;
    uint32_t jedec_id_;
    uint32_t vdd_;
};

class SocketProgrammer : public Programmer
{
   public:
    SocketProgrammer(const std::string &db_file) : Programmer(db_file)
    {
        prog_interface_ = std::shared_ptr<SocketProgrammerInterface>(
            new SocketProgrammerInterface(reinterpret_cast<void *>(getenv("SKT_PORT"))));
    }

   private:
    uint32_t CommonIdentifier(uint8_t cmd, uint8_t size) override
    {
        if (prog_interface_->getPowerConfig(kPwrVcc) < 3000) return 0;
        return Programmer::CommonIdentifier(cmd, size);
    }
};
}  // namespace dp

#endif  // #ifndef PROGRAMMER_HPP