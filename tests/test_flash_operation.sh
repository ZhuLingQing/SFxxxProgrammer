#!/bin/bash
CUR_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ARCHIVES=$CUR_DIR/../archives
PROJ_DIR=$CUR_DIR/..
CASE_NAME=test_flash_operation
NEED_BUILD=0

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
    $PROJ_DIR/umd/test/$CASE_NAME.cpp \
    $PROJ_DIR/umd/src/flash_info.cpp \
    $PROJ_DIR/umd/src/flash_database.cpp \
    $PROJ_DIR/umd/src/programmer.cpp \
    $PROJ_DIR/umd/src/programmer_interface.cpp \
    $PROJ_DIR/umd/src/flash_interface.cpp \
    -I$PROJ_DIR/umd/src \
    -I$PROJ_DIR/umd/inc \
    -I$PROJ_DIR/third-party/plog/include \
    -I$PROJ_DIR/third-party/json/include
if [ $? -ne 0 ]; then
    echo "Compile failed"
    exit 1
fi
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
fi

echo "execute $CASE_NAME.cpp"
start=$(date +%s%N)
$PROJ_DIR/build/$CASE_NAME $ARCHIVES/ChipInfoDb_opt.json
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
