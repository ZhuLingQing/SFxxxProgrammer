cmake_minimum_required(VERSION 3.16)

if (DEBUG)
    set(CMAKE_BUILD_TYPE "Debug")
    message(WARNING "build as debug")
endif()

find_package(Git)
 
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
else()
  string(FATAL "no git found")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -static")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static")

if (${PROJECT} STREQUAL "sim")
    enable_testing()
    
    message(WARNING "build sim")
    set(PROJ_NAME test_sim_flash )

    file( GLOB SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/sim/test/test_sim_flash.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/sim/sim_flash_mem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/umd/src/flash_info.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/umd/src/flash_database.cpp"
    )

    add_executable(${PROJ_NAME} ${SRCS})

    target_include_directories(${PROJ_NAME} PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/sim"
        "${CMAKE_CURRENT_SOURCE_DIR}/umd/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/umd/inc"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/plog/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/json/include"
    )

    target_link_libraries(${PROJ_NAME}
        pthread
    )

    add_test( NAME ${PROJ_NAME} COMMAND ${PROJ_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/archives/ChipInfoDb_opt.json M25P16)

    # install( TARGETS ${LIB_NAME} )
else()
endif()
