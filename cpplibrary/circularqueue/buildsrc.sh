#!/bin/sh

echo "---- Start to build src ----"

# 最后是否清除所有临时文件?
CLEAN_TMP=0
if [[ $# -eq 1 ]] && [[ "$1"=="r" ]]; then
    CLEAN_TMP=1
fi


cd src
make cleanall
make
cd ..


if [ $CLEAN_TMP -eq 1 ] ; then
find . -name "*~" | xargs rm -rf
find . -name "*.o" | xargs rm -rf
fi

echo "---- End to build src , see bin ----"
