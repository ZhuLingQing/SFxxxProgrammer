#ifndef SOCKET_SIM_FLASH_HPP
#define SOCKET_SIM_FLASH_HPP

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "dp_config.h"
#include "hal_proto.pb.h"

namespace sim
{
class SocketSimFlash
{
   public:
    SocketSimFlash(int port = 8080);
    int run();

   private:
    int create();
    int wait();
    int ProtoProcess(proto_hal::PacketType type, uint64_t ctrl_word);
    int ProtoSendCtrl(proto_hal::HalSktCtrlPacket &pkt);
    int ProtoReadCtrl(proto_hal::HalSktCtrlPacket &pkt);
    uint32_t ProtoSendData(void *buf, uint32_t size);
    uint32_t ProtoReadData(void *buf, uint32_t size);

    int CtrlTransceive(uint64_t ctrl_word);
    int CtrlPolling(uint64_t ctrl_word);
    int CtrlSetPowerVpp(uint64_t ctrl_word);
    int CtrlSetTargetFlash(uint64_t ctrl_word);
    int CtrlReadOnBoardFlash(uint64_t ctrl_word);
    int CtrlWriteEEPROM(uint64_t ctrl_word);
    int CtrlSetIO(uint64_t ctrl_word);
    int CtrlGetProgInfo(uint64_t ctrl_word);
    int CtrlSetPowerVcc(uint64_t ctrl_word);
    int CtrlStandAlone(uint64_t ctrl_word);
    int CtrlSetCs(uint64_t ctrl_word);
    int CtrlSetIoMode(uint64_t ctrl_word);
    // int Ctrl(uint64_t ctrl_word);

    int cli_fd_;
    int skt_fd_;

    uint64_t ctrl_word_;
    std::vector<uint8_t> payload_;
    std::vector<uint8_t> data_;

    const int port_;
    static const size_t kBulkSize = CONFIG_BULK_BUFFER_SIZE;
    static const size_t kMaxBufferSize = CONFIG_PROTO_BUFFER_SIZE;

    static std::map<uint8_t, int (SocketSimFlash::*)(uint64_t ctrl)> other_ctrl_map_;
    static std::map<uint8_t, int (SocketSimFlash::*)(uint64_t ctrl)> endpoint_ctrl_map_;
};  // class SocketSimFlash

}  // namespace sim

#endif  // SOCKET_SIM_FLASH_HPP
