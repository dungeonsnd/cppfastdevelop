#!/bin/sh

cd cppfoundation/
./buildsrc.sh rc

cd ..
cd cpplibrary/

cd circularqueue/
./buildsrc.sh rc
cd test
./buildsrc.sh rc
cd ../../

cd log/
./buildsrc.sh rc
cd test
./buildsrc.sh rc
cd ../../

cd netserver/
./buildsrc.sh rc
cd test
./buildsrc.sh rc
cd ../../

