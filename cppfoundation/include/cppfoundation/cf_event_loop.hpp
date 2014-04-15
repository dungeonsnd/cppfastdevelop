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
#include "cppfoundation/cf_network.hpp"
#include "cppfoundation/cf_memory.hpp"

#ifdef __linux__
#include "cppfoundation/cf_epoll.hpp"
#else
#ifdef __bsd__
#include "cppfoundation/cf_kqueue.hpp"
#else
#include "cppfoundation/cf_poll.hpp"
#endif // __bsd__
#endif // __linux__

namespace cf
{

#ifdef __linux__
typedef Epoll  Demux;
#else
#ifdef __bsd__
typedef Kqueue  Demux;
#else
typedef Poll  Demux;
#endif // __bsd__
#endif // __linux__

template <typename EventHandlerType>
class EventLoop : public cf::NonCopyable
{
public:
    typedef std::unordered_map < cf_fd, cf_uint32  > T_LISTENFD_MAXCONN;

    EventLoop(const T_LISTENFD_MAXCONN & listenfd_maxconn,
              EventHandlerType & handler,
              cf_int processid)
        :_stop(false),_handler(handler)
    {
        std::unordered_set < cf_fd > listenfds;
        for (T_LISTENFD_MAXCONN::const_iterator it=
                 listenfd_maxconn.begin();
             it!=listenfd_maxconn.end(); it++)
            listenfds.insert(it->first);
        CF_PRINT_FUNC;
        CF_NEWOBJ(p, Demux, listenfds);
        if(NULL==p)
            _THROW(AllocateMemoryError, "Allocate memory failed !");
        _demux.reset(p);

        _handler.Init(listenfd_maxconn, _demux, processid);
    }
    cf_void Stop()
    {
        _stop =true;
    }
    ~EventLoop()
    {
        _stop =true;
    }

    cf_void WaitEvent(const cf_int timeoutMilliseconds)
    {
        CF_PRINT_FUNC;
        // only for valgrind test,
        // to check exit program gracefully other than ctrl+c.
        //        cf_int times =5000;
        while(false==_stop)
        {
            //            if(0==times--)
            //                break;

            _vecEvent.clear();
            _demux->WaitEvent(_vecEvent,timeoutMilliseconds);

            if(_vecEvent.size())
            {
                for(Demux::TYPE_VECEVENT_ITER it =_vecEvent.begin(); it!=_vecEvent.end(); it++)
                {
                    switch(it->second)
                    {
                    case networkdefs::CFEV_ACCEPT:
                        _handler.OnAccept(it->first);
                        break;
                    case networkdefs::CFEV_READ:
                        _handler.OnRead(it->first);
                        break;
                    case networkdefs::CFEV_WRITE:
                        _handler.OnWrite(it->first);
                        break;
                    case networkdefs::CFEV_CLOSE:
                        _handler.OnClose(it->first);
                        break;
                    case networkdefs::CFEV_ERROR:
                        _handler.OnError(it->first);
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                _handler.OnTimeout();
            }
        }
    }
private:
    bool _stop;
    EventHandlerType & _handler;
    std::shared_ptr < Demux > _demux;
    Demux::TYPE_VECEVENT _vecEvent;
};


}

#endif // _HEADER_FILE_CFD_CF_EVENT_LOOP_HPP_

