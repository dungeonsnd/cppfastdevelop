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

#ifndef _HEADER_FILE_CFD_CL_IO_COMPLETE_HPP_
#define _HEADER_FILE_CFD_CL_IO_COMPLETE_HPP_

#include "cppfoundation/cf_root.hpp"

namespace cl
{
namespace ns
{

class IOComplete : public cf::NonCopyable
{
public:
    virtual ~IOComplete()
    {
    }
    virtual cf_void OnAcceptComplete(cf_int fd) =0;
    virtual cf_void OnReadComplete(cf_int fd, cf_void * buff, cf_uint32 bytes) =0;
    virtual cf_void OnWriteComplete(cf_int fd, cf_uint32 bytes) =0;
    virtual cf_void OnCloseComplete(cf_int fd) =0;
    virtual cf_void OnTimeoutComplete(cf_int fd) =0;
    virtual cf_void OnErrorComplete(cf_int fd) =0;
};

} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_IO_COMPLETE_HPP_
