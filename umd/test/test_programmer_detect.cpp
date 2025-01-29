// g++ -o ./build/test -std=c++20 \
    ./umd/test/test_programmer_detect.cpp \
    ./umd/src/flash_database.cpp \
    ./umd/src/flash_info.cpp \
    -I./umd/src -I./umd/inc \
    -I./third-party/plog/include \
    -I./third-party/json/include

#include <cstdlib>  // 包含getenv函数
#include <iostream>

#include "dp_logging.hpp"
#include "dummy_programmer.hpp"
#include "flash_database.hpp"

using FlashDatabase = dp::FlashDatabase;
using DummyProgrammer = dp::DummyProgrammer;

int main(int argc, char* argv[])
{
    const char* plog_level = getenv("PLOG_LEVEL");
    std::cout << "export PLOG_LEVEL=" << plog_level << " could adjust log level" << std::endl;
    std::cout << "export DUMMY_ID=" << getenv("DUMMY_ID") << " for dummy read id check" << std::endl;
    // DP_LOG_INIT_WITH_CONSOLE(INFO, "./test.log", 30000, 3);
    DP_LOG_INIT_CONSOLE_ONLY(static_cast<plog::Severity>(plog_level ? std::atoi(plog_level) : plog::info));

    std::shared_ptr<dp::ProgrammerHal> hal = std::make_shared<dp::DummyHal>();
    std::shared_ptr<dp::ProgrammerInterface> interface = std::make_shared<dp::DummyProgInterface>(hal);
    std::unique_ptr<dp::DummyProgrammer> prog =
        std::make_unique<dp::DummyProgrammer>(argc > 1 ? argv[1] : "", interface);
        
    std::set<std::string> flash_name_list;
    auto r = prog->Detect(flash_name_list);
    if (kSc != r)
        std::cout << "Detect return " << r << std::endl;
    else
    {
        std::cout << "Found " << flash_name_list.size() << " flash chips" << std::endl;
        for (auto ite : flash_name_list)
        {
            std::cout << "    - " << ite << std::endl;
        }
    }
    return 0;
}
