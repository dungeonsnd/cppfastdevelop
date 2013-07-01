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

#ifndef _HEADER_FILE_CFD_CF_DEMUX_HPP_
#define _HEADER_FILE_CFD_CF_DEMUX_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_memory.hpp"
#ifdef __linux
#include "cppfoundation/cf_epoll.hpp"
#else
#ifdef __bsd
#include "cppfoundation/cf_kqueue.hpp"
#endif
#endif // __linux

namespace cf
{

template <typename DemuxType>
class Demux : public cf::NonCopyable
{
public:
    Demux()
    {
        CF_NEWOBJ(p, DemuxType);
        if(NULL==p)
            _THROW(AllocateMemoryError, "Allocate memory failed !");
        _demux.reset(p);
    }
    ~Demux()
    {
    }
    cf_void AddEvent(cf_int fd, cf_uint32 ev)
    {
        _demux->AddEvent(fd, ev);
    }
    cf_void DelEvent(cf_int fd, cf_uint32 ev)
    {
        _demux->DelEvent(fd, ev);
    }
    cf_void UnregisterConn(cf_int fd)
    {
        _demux->UnregisterConn(fd);
    }
private:
    std::shared_ptr < DemuxType > _demux;
};


}

#endif // _HEADER_FILE_CFD_CF_DEMUX_HPP_

