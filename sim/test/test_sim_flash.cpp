// bash ./test_sim_flash.sh --rebuild

#include <chrono>
#include <cstdlib>  // 包含getenv函数
#include <iostream>
#include <limits>
#include <random>

// #include "dp_logging.hpp"
#include "flash_database.hpp"
#include "sim_flash_mem.hpp"

constexpr size_t kTestBufSize = 512;
using test_buf_t = std::array<uint8_t, kTestBufSize>;

#define TEST_COMMAND_DATA(cmd, num)                                                     \
    do                                                                                  \
    {                                                                                   \
        r = flash->Transfer(std::vector<uint8_t>(cmd, cmd + sizeof(cmd)), resp, false); \
        r |= flash->Transfer(num, resp);                                                \
    } while (0)
#define TEST_COMMAND(cmd) r = flash->Transfer(std::vector<uint8_t>(cmd, cmd + sizeof(cmd)), resp)

static void dump(const uint8_t* buf, size_t size, std::string prefix = "[DUMP] ")
{
    std::cout << prefix;
    for (size_t i = 0; i < size; ++i)
    {
        std::cout << to_hex_string(buf[i]) << " ";
        if (i && (i % 16 == 0)) std::cout << std::endl << "       ";
    }
    std::cout << std::endl;
}

static void addrConvert(uint8_t* buf, uint64_t addr, size_t nAddr)
{
    for (size_t i = 0; i < nAddr; ++i)
    {
        buf[i] = static_cast<uint8_t>(addr >> (8 * (nAddr - 1 - i)));
    }
}

class Random
{
   public:
    Random() : gen_(rd_()) {}
    test_buf_t getPageBuf()
    {
        test_buf_t buf;
        std::uniform_int_distribution<> dist(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());

        for (auto& elem : buf)
        {
            elem = dist(gen_);
        }
        return buf;
    }
    uint64_t getAddress(uint64_t max_)
    {
        uint64_t addr;
        std::uniform_int_distribution<> dist(0, max_);
        return dist(gen_);
    }
    std::vector<uint8_t> getUID(size_t num)
    {
        std::vector<uint8_t> buf;
        buf.reserve(num);
        std::uniform_int_distribution<> dist(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());

        for (size_t i = 0; i < num; ++i)
        {
            buf.push_back(dist(gen_));
        }
        return buf;
    }

   private:
    std::random_device rd_;
    std::mt19937 gen_;
};

int main(int argc, char* argv[])
{
    int r, retry;
    Random rnd;
    auto uid = rnd.getUID(16);
    const uint8_t rdsr[2] = {0x05};
    const uint8_t wren[1] = {0x06};
    const uint8_t wrdi[1] = {0x04};
    const uint8_t rdid[4] = {0x9f};
    const uint8_t read_uid[1 + 3 + 1 + 16] = {0x9e};
    const uint8_t wrsr_1c[2] = {0x01, 0x1C};
    const uint8_t wrsr_00[2] = {0x01, 0x00};
    const uint8_t ce[1] = {0xC7};
    uint8_t se[4] = {0xD8};
    uint8_t pp[1 + 3 + 256] = {0x02};
    uint8_t rd[1 + 3] = {0x3};
    uint8_t frd[1 + 3 + 1] = {0xB};

    uint64_t addr;
    test_buf_t wbuf, rbuf;
    std::vector<uint8_t> resp;

    // const char* plog_level = getenv("PLOG_LEVEL");
    // std::cout << "export PLOG_LEVEL=" << plog_level << " could adjust log level" << std::endl;
    // DP_LOG_INIT_WITH_CONSOLE(INFO, "./test.log", 30000, 3);
    // DP_LOG_INIT_CONSOLE_ONLY(static_cast<plog::Severity>(plog_level ? std::atoi(plog_level) : plog::info));

    std::cout << std::endl << "\t---- Test Start ----" << std::endl;
    auto db = dp::FlashDatabase::getInstance(argv[1]);
    if (!db.isLoaded())
    {
        std::cout << "Database not loaded" << std::endl;
        return 1;
    }
    auto info = db.getFlashInfo(argv[2]);
    if (info == nullptr)
    {
        std::cout << "Flash " << argv[2] << " not found" << std::endl;
        return 1;
    }
    auto finfo = info->getInfo();
    auto flash = sim::SimFlashFactory(info);
    if (flash == nullptr)
    {
        std::cout << "Flash " << argv[2] << " not factory" << std::endl;
        return 1;
    }
    flash->setUID(uid);
    flash->setVcc(3300);
    flash->setClock(12);
    dump(uid.data(), uid.size(), "[UID]");
    {
        std::cout << "\t>>>>  RDID <<<<" << std::endl;
        TEST_COMMAND(rdid);
        std::cout << "RDID: " << to_hex_string(resp[1]) << to_hex_string(resp[2]) << to_hex_string(resp[3])
                  << ", return " << r << std::endl;
        dump(resp.data(), resp.size());

        TEST_COMMAND(read_uid);
        std::cout << "RUID: "
                  << "return " << r << std::endl;
        dump(resp.data(), resp.size());
        std::cout << "FRD: " << (std::equal(resp.begin() + 5, resp.end(), uid.begin()) ? "OK" : "FAIL") << std::endl;
    }
    {
        std::cout << "\t>>>>  WREN, WRDI, RDSR <<<<" << std::endl;
        TEST_COMMAND(rdsr);
        std::cout << "RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
        TEST_COMMAND(wren);
        std::cout << "WREN: "
                  << " return " << r << std::endl;
        TEST_COMMAND(rdsr);
        std::cout << "RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
        TEST_COMMAND(wrdi);
        std::cout << "WRDI: "
                  << " return " << r << std::endl;
        TEST_COMMAND(rdsr);
        std::cout << "RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
        TEST_COMMAND(wrsr_1c);
        std::cout << "WRSR: 0x" << to_hex_string(wrsr_1c[1]) << " return " << r << std::endl;
        TEST_COMMAND(rdsr);
        std::cout << "RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
    }
    {
        std::cout << "\t>>>>  WP <<<<" << std::endl;
        flash->setWP(true);
        std::cout << "setWP: true" << std::endl;
        TEST_COMMAND(wren);
        std::cout << "WREN: "
                  << " return " << r << std::endl;
        TEST_COMMAND(wrsr_00);
        std::cout << "WRSR: 0x" << to_hex_string(wrsr_00[1]) << " return " << r << std::endl;
        TEST_COMMAND(rdsr);
        std::cout << "RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
        flash->setWP(false);
        std::cout << "setWP: false" << std::endl;
        TEST_COMMAND(wrsr_00);
        std::cout << "WRSR: 0x" << to_hex_string(wrsr_00[1]) << " return " << r << std::endl;
        TEST_COMMAND(rdsr);
        std::cout << "RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
    }
    {
        std::cout << "\t>>>>  CE <<<<" << std::endl;
        TEST_COMMAND(wren);
        TEST_COMMAND(ce);
        retry = 0;
        auto start = std::chrono::high_resolution_clock::now();
        do
        {
            TEST_COMMAND(rdsr);
            retry++;
        } while (resp[1] & kStatusWIP);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "CE async time: " << duration << " us. Tried " << retry << " times." << std::endl;
        std::cout << "CE: RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
    }
    {
        std::cout << "\t>>>>  Backdoor <<<<" << std::endl;
        r = flash->BackdoorBlankCheck(0, finfo.ChipSizeInKByte * 1024LL);
        std::cout << "Backdoor::BlankCheck: return " << r << std::endl;

        wbuf = rnd.getPageBuf();
        addr = rnd.getAddress(finfo.ChipSizeInKByte * 1024LL - wbuf.size());
        r = flash->BackdoorWrite(addr, wbuf.data(), wbuf.size());
        std::cout << "Backdoor::Write: @" << std::hex << addr << " return " << r << std::endl;
        r = flash->BackdoorBlankCheck(0, finfo.ChipSizeInKByte * 1024LL);
        std::cout << "Backdoor::BlankCheck: return " << r << std::endl;
        r = flash->BackdoorRead(addr, rbuf.begin(), rbuf.size());
        std::cout << "Backdoor::Read: @" << std::hex << addr << " return " << r << std::endl;
        std::cout << "Compare: " << (rbuf == wbuf ? "OK" : "FAIL") << std::endl;

        r = flash->BackdoorErase(0, finfo.ChipSizeInKByte * 1024LL / finfo.SectorSizeInByte);
        std::cout << "Backdoor::Erase: return " << r << std::endl;
        r = flash->BackdoorBlankCheck(0, finfo.ChipSizeInKByte * 1024LL);
        std::cout << "Backdoor::BlankCheck: return " << r << std::endl;
    }
    {
        std::cout << "\t>>>>  SE <<<<" << std::endl;
        addr = rnd.getAddress(finfo.ChipSizeInKByte * 1024LL - wbuf.size());
        r = flash->BackdoorWrite(addr, wbuf.data(), wbuf.size());
        addrConvert(se + 1, addr, finfo.AddrWidth);
        TEST_COMMAND(wren);
        TEST_COMMAND(se);
        retry = 0;
        auto start = std::chrono::high_resolution_clock::now();
        do
        {
            TEST_COMMAND(rdsr);
            retry++;
        } while (resp[1] & kStatusWIP);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "SE: @" << std::hex << addr << ". async time: " << std::dec << duration << " ms. Tried " << retry
                  << " times." << std::endl;
        std::cout << "SE: RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
        r = flash->BackdoorBlankCheck(0, finfo.ChipSizeInKByte * 1024LL);
        std::cout << "Backdoor::BlankCheck: return " << r << std::endl;
    }
    {
        std::cout << "\t>>>>  PP, RD, FRD <<<<" << std::endl;
        addr = rnd.getAddress(finfo.ChipSizeInKByte * 1024LL - wbuf.size()) & (~255LL);
        auto start = std::chrono::high_resolution_clock::now();
        addrConvert(pp + 1, addr, finfo.AddrWidth);
        std::copy(pp + 1 + finfo.AddrWidth, pp + sizeof(pp), wbuf.begin());
        TEST_COMMAND(wren);
        TEST_COMMAND(pp);
        retry = 0;
        do
        {
            TEST_COMMAND(rdsr);
            retry++;
        } while (resp[1] & kStatusWIP);
        addrConvert(pp + 1, addr + 256, finfo.AddrWidth);
        std::copy(pp + 1 + finfo.AddrWidth, pp + sizeof(pp), wbuf.begin() + 256);
        TEST_COMMAND(wren);
        TEST_COMMAND(pp);
        retry = 0;
        do
        {
            TEST_COMMAND(rdsr);
            retry++;
        } while (resp[1] & kStatusWIP);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "PP: @" << std::hex << addr << ". async time: " << std::dec << duration << " ms. Tried " << retry
                  << " times." << std::endl;
        std::cout << "PP: RDSR: 0x" << to_hex_string(resp[1]) << ", return " << r << std::endl;
        // RD
        addrConvert(rd + 1, addr, finfo.AddrWidth);
        TEST_COMMAND_DATA(rd, wbuf.size());
        std::cout << "RD: " << (std::equal(resp.begin(), resp.end(), wbuf.begin()) ? "OK" : "FAIL") << std::endl;
        // FRD
        addrConvert(frd + 1, addr, finfo.AddrWidth);
        TEST_COMMAND_DATA(frd, wbuf.size());
        std::cout << "FRD: " << (std::equal(resp.begin(), resp.end(), wbuf.begin()) ? "OK" : "FAIL") << std::endl;
    }
    std::cout << "<DONE>" << std::endl;
    return 0;
}
