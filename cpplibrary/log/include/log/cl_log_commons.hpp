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

#ifndef _HEADER_FILE_CFD_CL_LOG_COMMONS_HPP_
#define _HEADER_FILE_CFD_CL_LOG_COMMONS_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cl
{
namespace log
{

enum CLLEVEL
{
    TRACE=0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    COUNT // End of enum define.
};
enum
{
    CLLEVELNAME_MAX_LEN =6
};

enum
{
    CL_LOG_MAX_MSG_LEN =8192,
    CL_LOG_MAX_LOOP =1024,
    CL_LOG_PREFIX_LEN =33,
    CL_LOG_SUFFIX_LEN =200
};

} //namespace cl
} // namespace log

#endif // _HEADER_FILE_CFD_CL_LOG_COMMONS_HPP_

