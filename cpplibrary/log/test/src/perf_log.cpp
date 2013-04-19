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
#include "log/cl_log.hpp"

int main(int argc,char * argv[])
{
    cl::log::LogNonCache < cf::FakeMutex > log("tmp.log");
    std::string buf(1024,'@');
    buf ="abcdfeg";

    for(int i=0; i<1; i++)
        CLLOG(log,cl::log::WARN,buf.c_str(),buf.size());

    return 0;
}
