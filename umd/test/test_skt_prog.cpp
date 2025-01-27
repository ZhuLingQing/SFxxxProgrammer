// g++ -o ./build/test -std=c++20 \
    ./umd/test/test_flash_operation.cpp \
    ./umd/src/flash_database.cpp \
    ./umd/src/flash_info.cpp \
    ./umd/src/flash_interface.cpp \
    -I./umd/src -I./umd/inc \
    -I./third-party/plog/include \
    -I./third-party/json/include

#include <cstdlib>  // 包含getenv函数
#include <iostream>

#include "dp_logging.hpp"
#include "flash_database.hpp"
#include "socket_programmer.hpp"

using FlashDatabase = dp::FlashDatabase;
using SktProgrammer = dp::SktProgrammer;

int main(int argc, char* argv[])
{
    const char* plog_level = getenv("PLOG_LEVEL");
    std::cout << "export PLOG_LEVEL=" << plog_level << " could adjust log level" << std::endl;
    std::cout << "export DUMMY_ID=" << getenv("DUMMY_ID") << " for dummy read id check" << std::endl;
    // DP_LOG_INIT_WITH_CONSOLE(INFO, "./test.log", 30000, 3);
    DP_LOG_INIT_CONSOLE_ONLY(static_cast<plog::Severity>(plog_level ? std::atoi(plog_level) : plog::info));

    std::unique_ptr<SktProgrammer> prog = std::make_unique<SktProgrammer>(argc > 1 ? argv[1] : "");

    std::set<std::string> flash_name_list;
    auto r = prog->Detect(flash_name_list);
    if (kSc != r || flash_name_list.empty())
        std::cout << "Detect return " << r << std::endl;
    else
    {
        std::cout << "Found " << flash_name_list.size() << " flash chips" << std::endl;
        for (auto ite : flash_name_list)
        {
            std::cout << "    select - " << ite << std::endl;
            auto flash_info = prog->Select(ite);
            if (flash_info == nullptr)
            {
                std::cout << "    select - " << ite << " failed" << std::endl;
                return 1;
            }
            auto flash_ = prog->getSelectedFlash();
            if (flash_ == nullptr)
            {
                std::cout << "    select - " << ite << " failed" << std::endl;
                return 1;
            }
            auto info = flash_->getFlashInfo();
            if (info != flash_info)
            {
                std::cout << "    invalid info" << std::endl;
                return 1;
            }
            info->Dump();
            if (flash_->Identify() == false)
            {
                std::cout << info->getName() << ": identify failed" << std::endl;
                return 1;
            }
            auto id = flash_->ReadId();
            std::cout << info->getName() << ": identify OK. ID: " << std::hex << id << std::endl;
        }
    }
    prog->ShutDown();
    return 0;
}
