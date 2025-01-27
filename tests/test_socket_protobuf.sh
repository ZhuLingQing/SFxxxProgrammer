#!/bin/bash
CUR_DIR=$(cd $(dirname $0); pwd)
ARCHIVES=$CUR_DIR/../archives
PROJ_DIR=$(cd $CUR_DIR/..; pwd)
SRC_DIR=$PROJ_DIR/umd/test/socket_protobuf
CASE_NAME=test_socket_protobuf
NEED_BUILD=0

echo "PWD: $CUR_DIR"

if [ ! -f $PROJ_DIR/build/socket_cli ] && [ ! -f $PROJ_DIR/build/socket_svr ]; then
    NEED_BUILD=1
elif [ $# -gt 0 ] && [ "$1" == "--rebuild" ]; then
    NEED_BUILD=1
fi

if [ $NEED_BUILD -eq 1 ]; then
echo "Compile $CASE_NAME.cpp"
rm -rf $PROJ_DIR/build/socket_cli $PROJ_DIR/build/socket_svr $SRC_DIR/message.pb.*
start=$(date +%s%N)
# generate message.pb.cc
protoc --proto_path=$SRC_DIR --cpp_out=$SRC_DIR message.proto
if [ $? -ne 0 ]; then
    echo "message.proto Compile failed"
    exit 1
fi
# generate socket_cli
g++ -o $PROJ_DIR/build/socket_cli -std=c++20 \
    $SRC_DIR/message.pb.cc -lprotobuf -pthread \
    -I$PROJ_DIR/umd/inc \
    -I$PROJ_DIR/third-party/plog/include \
    $SRC_DIR/socket_cli.cpp 
if [ $? -ne 0 ]; then
    echo "socket_cli Compile failed"
    exit 1
fi
# generate socket_svr
g++ -o $PROJ_DIR/build/socket_svr -std=c++20 \
    $SRC_DIR/message.pb.cc -lprotobuf -pthread \
    -I$PROJ_DIR/umd/inc \
    -I$PROJ_DIR/third-party/plog/include \
    $SRC_DIR/socket_svr.cpp 
if [ $? -ne 0 ]; then
    echo "socket_svr Compile failed"
    exit 1
fi
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
fi

echo "execute $CASE_NAME.cpp"
start=$(date +%s%N)
$PROJ_DIR/build/socket_svr & \
sleep 1
$PROJ_DIR/build/socket_cli
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
