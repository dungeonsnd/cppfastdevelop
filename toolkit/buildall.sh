#!/bin/sh

cd toolkit/misc/codeformat/
./format_cpp.sh
cd ../../../
pwd

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


