#!/bin/bash
CUR_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ARCHIVES=$CUR_DIR/../archives
PROJ_DIR=$CUR_DIR/..
CASE_NAME=test_sim_flash
NEED_BUILD=0
FLASH_NAME=M25P16

if [ ! -d $PROJ_DIR/build ]; then
    mkdir $PROJ_DIR/build
fi

if [ ! -f $PROJ_DIR/build/$CASE_NAME ]; then
    NEED_BUILD=1
elif [ $# -gt 0 ] && [ "$1" == "--rebuild" ]; then
    NEED_BUILD=1
fi

if [ $NEED_BUILD -eq 1 ]; then
echo "Compile $CASE_NAME.cpp"
rm -rf $PROJ_DIR/build/$CASE_NAME
start=$(date +%s%N)
g++ -o $PROJ_DIR/build/$CASE_NAME -std=c++20 \
    $PROJ_DIR/sim/test/$CASE_NAME.cpp \
    $PROJ_DIR/sim/sim_flash_mem.cpp \
    $PROJ_DIR/umd/src/flash_info.cpp \
    $PROJ_DIR/umd/src/flash_database.cpp \
    -I$PROJ_DIR/sim \
    -I$PROJ_DIR/umd/src \
    -I$PROJ_DIR/umd/inc \
    -I$PROJ_DIR/third-party/plog/include \
    -I$PROJ_DIR/third-party/json/include \
    -pthread -Og
if [ $? -ne 0 ]; then
    echo "Compile failed"
    exit 1
fi
end=$(date +%s%N)
echo "Build time: $(( (end - start) / 1000000000 )) sec"
fi

start=$(date +%s%N)

if [ $NEED_BUILD -eq 1 ]; then
    echo "execute: $PROJ_DIR/build/$CASE_NAME $FLASH_NAME"
    $PROJ_DIR/build/$CASE_NAME $FLASH_NAME
else
    $PROJ_DIR/build/$CASE_NAME $1
fi
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) ms"
