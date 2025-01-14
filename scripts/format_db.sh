#!/bin/bash
CUR_DIR=$(cd $(dirname $0); pwd)
ARCHIVES=$CUR_DIR/../archives
echo "$CUR_DIR"
echo "$ARCHIVES"

rm $ARCHIVES/ChipInfoDb.xml $ARCHIVES/ChipInfoDb.json -f
python3 $CUR_DIR/xml-format.py $ARCHIVES/ChipInfoDb.dedicfg $ARCHIVES/ChipInfoDb.xml
python3 $CUR_DIR/xml-json.py $ARCHIVES/ChipInfoDb.xml $ARCHIVES/ChipInfoDb.json
