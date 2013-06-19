#!/bin/sh

# 最后是否清除所有临时文件?
CLEAN_TMP=0
# 编译前是否清空.o文件?
REBUILD_ALL=0
# 编译过程线程数
MAKE_J=8

if [[ $# -eq 1 ]]; then
    
    if [ $1 = "c" ]; then
        CLEAN_TMP=1
    elif [ $1 = "r" ]; then
        REBUILD_ALL=1
    elif [ $1 = "rc" ] || [ $1 = "cr" ]; then
        REBUILD_ALL=1
        CLEAN_TMP=1
    fi
fi

cd src
if [ $REBUILD_ALL -eq 1 ] ; then
    make cleanall
fi
make -j$MAKE_J
cd ..


if [ $CLEAN_TMP -eq 1 ] ; then
    find . -name "*~" | xargs rm -rf
    find . -name "*.o" | xargs rm -rf
fi

echo "---- End to build src , see bin ----"
