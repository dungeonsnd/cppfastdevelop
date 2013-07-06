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

#ifndef _HEADER_FILE_CFD_CF_EVENT_LOOP_HPP_
#define _HEADER_FILE_CFD_CF_EVENT_LOOP_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"


namespace cf
{

namespace eventloopdefs
{
enum EVENT_TYPE
{
    EV_ACCEPT = 01,
    EV_READ = 02,
    EV_WRITE = 04,
    EV_TIMEOUT = 010,
    EV_CLOSE = 020,
    EV_ERROR = 040
};
} // namespace eventloopdefs

class EventHandler;

template <typename DemuxType>
class EventLoop : public cf::NonCopyable
{
public:
    EventLoop()
    {
        CF_NEWOBJ(p, DemuxType);
        if(NULL==p)
            _THROW(AllocateMemoryError, "Allocate memory failed !");
        _demux.reset(p);
        CF_NEWOBJ(p1, EventHandler);
        if(NULL==p1)
            _THROW(AllocateMemoryError, "Allocate memory failed !");
        _handler.reset(p1);
    }
    ~EventLoop()
    {
    }

    cf_void WaitEvent(cf_int timeoutMilliseconds)
    {
        _demux->WaitEvent(_vecEvent,timeoutMilliseconds);
        for(DemuxType::TYPE_VECEVENT_ITER it =_vecEvent.begin();it!=_vecEvent.end();it++)
        {
            _handler;
        }
    }
private:
    std::shared_ptr < DemuxType > _demux;
    DemuxType::TYPE_VECEVENT _vecEvent;
    std::shared_ptr < EventHandler > _handler;
};


}

#endif // _HEADER_FILE_CFD_CF_EVENT_LOOP_HPP_

