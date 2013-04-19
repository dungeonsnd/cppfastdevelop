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

#include "cppfoundation/cf_process.hpp"

namespace cf
{

cf_void Process::SetUmask(mode_t mask)
{
    cf_umask(mask);
}
cf_void Process::SetRLimit(cf_int res, cf_const struct rlimit * rlimit)
{
    if (0!=cf_setrlimit(res, rlimit))
        _THROW(SyscallExecuteError, "Failed to execute cf_setrlimit !");
}
struct rlimit Process::GetRLimit(cf_int res)
{
    struct rlimit rl;
    rl.rlim_cur =0;
    rl.rlim_max =0;
    if (0!=cf_getrlimit(res, &rl) )
        _THROW(SyscallExecuteError, "Failed to execute cf_getrlimit !");
    return rl;
}
cf_void Process::ParseOptions()
{
}


} // namespace cf
