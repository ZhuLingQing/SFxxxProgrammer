// g++ -o test -std=c++17 ./umd/test/test_database.cpp ./umd/src/flash_database.cpp \
    -I./umd/src -I./umd/inc \
    -I./third-party/plog/include \
    -I./third-party/json/include 

#include <iostream>
#include <cstdlib> // 包含getenv函数

#include "dp_logging.hpp"
#include "flash_database.hpp"

using FlashDatabase = dp::sf::FlashDatabase;

int main(int argc, char* argv[])
{
    const char* plog_level = getenv("PLOG_LEVEL");
    std::cout <<"export PLOG_LEVEL=" << plog_level << " could adjust log level" << std::endl;
    // DP_LOG_INIT_WITH_CONSOLE(INFO, "./test.log", 30000, 3);
    DP_LOG_INIT_CONSOLE_ONLY(static_cast<plog::Severity>(plog_level ? std::atoi(plog_level) : plog::info));
    auto dbi = FlashDatabase::getInstance(argc > 1 ? argv[1] : "");
    if (!dbi.isLoaded())
    {
        std::cout << "Database not loaded" << std::endl;
        return 1;
    }
    if (argc > 2)
    {
        int rc = dbi.Save(std::string(argv[2]));
        std::cout << "Database save to: " << argv[2] << " rc=" << rc << std::endl;
    }
    std::string test_chip_name = "M25P64";
    auto info = dbi.getFlashInfo(test_chip_name);
    if (info.isValid() == false)
    {
        std::cout << test_chip_name << " not found" << std::endl;
        return 1;
    }
    else
    {
        info.Dump();
    }

    auto rdid_list = dbi.getReadIdInfoList();
    std::cout << "RDID[" << rdid_list.size() << "]: ";
    for (auto ite : rdid_list)
    {
        std::cout << "[0x" << std::hex << ite.first << ", " << std::dec << ite.second << "], ";
    }
    std::cout << std::endl;

    auto power_vdd = dbi.getPowerVddList();
    std::cout << "PowerVoltage[" << power_vdd.size() << "]: ";
    for (auto ite : power_vdd)
    {
        std::cout << ite << ", ";
    }
    std::cout << std::endl;

    return 0;
}