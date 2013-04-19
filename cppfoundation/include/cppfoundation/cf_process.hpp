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

#ifndef _HEADER_FILE_CFD_CF_PROCESS_HPP_
#define _HEADER_FILE_CFD_CF_PROCESS_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

class Process : public NonCopyable
{
public:
    Process() {};
    virtual ~Process() {};

    virtual cf_int Run()
    {
        return 0;
    }

    pid_t Getpid()
    {
        return getpid();
    }
    pid_t Getppid()
    {
        return getppid();
    }
    cf_void SetUmask(mode_t mask = 0);
    cf_void SetRLimit(cf_int res, cf_const struct rlimit * rlimit);
    struct rlimit GetRLimit(cf_int res);
private:
    cf_void ParseOptions();
};

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_PROCESS_HPP_
