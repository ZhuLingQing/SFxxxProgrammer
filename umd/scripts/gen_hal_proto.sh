#!/bin/bash
CUR_DIR=$(cd $(dirname $0); pwd)
SRC_DIR=$CUR_DIR
TAR_DIR=$CUR_DIR/../src

protoc --proto_path=$SRC_DIR --cpp_out=$TAR_DIR hal_proto.proto