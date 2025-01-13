#!/bin/bash
CUR_DIR=$(cd $(dirname $0); pwd)
ARCHIVES=$CUR_DIR/../archives
PROJ_DIR=$CUR_DIR/..

if [ $# -eq 0 ] || [ "$1" != "no-build" ]; then
echo "Compile test_database.cpp"
start=$(date +%s%N)
g++ -o $PROJ_DIR/build/test -std=c++17 $PROJ_DIR/umd/test/test_database.cpp $PROJ_DIR/umd/src/flash_database.cpp \
    -I$PROJ_DIR/umd/src -I$PROJ_DIR/umd/inc \
    -I$PROJ_DIR/third-party/plog/include \
    -I$PROJ_DIR/third-party/json/include
if [ $? -ne 0 ]; then
    echo "Compile failed"
    exit 1
fi
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
fi

echo "execute test_database.cpp"
start=$(date +%s%N)
$PROJ_DIR/build/test $ARCHIVES/ChipInfoDb.json $ARCHIVES/ChipInfoDb_opt.json
end=$(date +%s%N)
echo "Execution time: $(( (end - start) / 1000000 )) milliseconds"
