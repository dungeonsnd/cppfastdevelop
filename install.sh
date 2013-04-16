#!/bin/sh

PRJ_PATH=/home/pro/git_src/cppfastdevelop/c

cd /usr/lib64/

ln -s $PRJ_PATH/cppfoundation/bin/libcfclass_d.so libcfclass_d.so
ln -s $PRJ_PATH/cppfoundation/bin/libcfstub_d.so libcfstub_d.so

ln -s $PRJ_PATH/cpplibrary/circularqueue/bin/libclcircularqueue_d.so libclcircularqueue_d.so
ln -s $PRJ_PATH/cpplibrary/log/bin/libcllog_d.so libcllog_d.so
ln -s $PRJ_PATH/cpplibrary/netserver/bin/libclnetserver_d.so libclnetserver_d.so

