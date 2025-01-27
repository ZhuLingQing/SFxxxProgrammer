#!/bin/bash
CUR_DIR=$(cd $(dirname $0); pwd)
ARCHIVES=$CUR_DIR/../archives
PROJ_DIR=$(cd $CUR_DIR/..; pwd)
CASE_NAME=test_skt_programmer
NEED_BUILD=0

echo "PWD: $CUR_DIR"

if [ ! -f $PROJ_DIR/build/$CASE_NAME ]; then
    NEED_BUILD=1
elif [ $# -gt 0 ] && [ "$1" == "--rebuild" ]; then
    NEED_BUILD=1
fi

if [ $NEED_BUILD -eq 1 ]; then
echo "Compile $CASE_NAME.cpp"
rm -rf $PROJ_DIR/build/test_skt_programmer $PROJ_DIR/umd/test/flash_info.pb.*
start=$(date +%s%N)
echo "generate flash_info.pb.cc"
if [ ! -f $PROJ_DIR/umd/test/flash_info.proto ]; then
    echo "flash_info.proto not found"
    exit 1
else
    echo "$PROJ_DIR/umd/test/flash_info.proto found"
fi
protoc --proto_path=$PROJ_DIR/umd/test --cpp_out=$PROJ_DIR/umd/test flash_info.proto
if [ $? -ne 0 ]; then
    echo "flash_info.proto Compile failed"
    exit 1
fi
# generate socket_cli
g++ -o $PROJ_DIR/build/test_skt_programmer -std=c++20 -I$PROJ_DIR\
    $PROJ_DIR/umd/test/flash_info.pb.cc -lprotobuf -pthread \
    $PROJ_DIR/umd/test/test_skt_prog.cpp \
    $PROJ_DIR/umd/src/flash_info.cpp \
    $PROJ_DIR/umd/src/flash_database.cpp \
    $PROJ_DIR/umd/src/programmer.cpp \
    $PROJ_DIR/umd/src/flash_interface.cpp \
    -I$PROJ_DIR/umd/src \
    -I$PROJ_DIR/umd/inc \
    -I$PROJ_DIR/third-party/plog/include \
    -I$PROJ_DIR/third-party/json/include
if [ $? -ne 0 ]; then
    echo "test_skt_prog Compile failed"
    exit 1
fi
fi
# generate socket_svr
# g++ -o $PROJ_DIR/build/socket_svr -std=c++20 -I.\
#     $PROJ_DIR/umd/test/message.pb.cc -lprotobuf -pthread \
#     $PROJ_DIR/umd/test/socket_svr.cpp 
# if [ $? -ne 0 ]; then
#     echo "socket_svr Compile failed"
#     exit 1
# fi
# end=$(date +%s%N)
# echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
# fi

# echo "execute $CASE_NAME.cpp"
# start=$(date +%s%N)
# $PROJ_DIR/build/socket_svr & \
# sleep 0.5
# $PROJ_DIR/build/socket_cli
# end=$(date +%s%N)
# echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
