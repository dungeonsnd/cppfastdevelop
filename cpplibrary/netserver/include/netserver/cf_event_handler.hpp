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

#ifndef _HEADER_FILE_CFD_CF_EVENT_HANDLER_HPP_
#define _HEADER_FILE_CFD_CF_EVENT_HANDLER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

class EventHandler : public cf::NonCopyable
{
public:
    EventHandler()
    {
    }
    ~EventHandler()
    {
    }
    
    cf_void AsyncRead(cf_uint32 sizeToRead)
    {
    }
    cf_void AsyncWrite(cf_cpvoid buf,cf_uint32 bufSize)
    {
    }

    cf_void OnConnect()
    {
    }
    cf_void OnRead()
    {
    }
    cf_void OnWrite()
    {
    }
    cf_void OnTimeout()
    {
    }
    cf_void OnClose()
    {
    }
    cf_void OnError()
    }
private:
};


}

#endif // _HEADER_FILE_CFD_CF_EVENT_HANDLER_HPP_

