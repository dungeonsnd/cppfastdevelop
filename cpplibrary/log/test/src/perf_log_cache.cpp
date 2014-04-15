/*
 * Copyright 2013, Jeffery Qiu. All rights reserved.
 *
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.gnu.org/licenses/lgpl.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//// Author: Jeffery Qiu (dungeonsnd at gmail dot com)
////

/*
[pro@node001dev test]$ time bin/perf_log

real    0m8.398s
user    0m3.761s
sys     0m3.331s
*/

#include <string>
#include "log/cl_log_cache.hpp"

int main(int argc,char * argv[])
{
    CLLOGC_INIT("tmp.log");
    std::string buf(32,'a');

    struct timeval tv1;
    if(-1==gettimeofday(&tv1, NULL))
        _THROW(cf::SyscallExecuteError, "Failed to execute gettimeofday !");

    for(int i=0; i<1000000; i++)
    {
        CLLOGC(cl::log::TRACE,buf.c_str());
        //        CLLOGC_FLUSH();
    }

    CLLOGC_FLUSH();

    struct timeval tv2;
    if(-1==gettimeofday(&tv2, NULL))
        _THROW(cf::SyscallExecuteError, "Failed to execute gettimeofday !");

    long long int usec =tv2.tv_sec*1000000+tv2.tv_usec -(tv1.tv_sec*1000000
                        +tv1.tv_usec);
    long long int sec = usec/1000000;
    usec = usec%1000000;
    printf("Using time: %lld sec , %lld usec \n", sec,usec);

    return 0;
}
