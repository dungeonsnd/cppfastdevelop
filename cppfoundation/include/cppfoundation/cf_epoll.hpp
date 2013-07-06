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

#ifndef _HEADER_FILE_CFD_CF_EPOLL_HPP_
#define _HEADER_FILE_CFD_CF_EPOLL_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_memory.hpp"

namespace cf
{

using namespace eventloopdefs;
enum EVENT_TYPE;

namespace epolldefs
{
enum
{
    SIZE_CREATE = 1,
    SIZE_MAXEVENTS = 8192
};
} // namespace lockdefs

class epoll : public cf::NonCopyable
{
public:
    typedef std::map < cf_fd, cf_ev > TYPE_MAPEVENT;
    typedef TYPE_MAPEVENT::iterator TYPE_MAPEVENT_ITER;
    typedef std::vector < std::pair < cf_fd, cf_ev > > TYPE_VECEVENT;
    typedef TYPE_VECEVENT::iterator TYPE_VECEVENT_ITER;
    
    epoll(cf_int maxEvents =epolldefs::SIZE_MAXEVENTS):_maxEvents(maxEvents)
    {
        _epfd =epoll_create(epolldefs::SIZE_CREATE);
        CF_NEWOBJ(p, epoll_event);
        if(NULL==p)
            _THROW(AllocateMemoryError, "Allocate memory failed !");
        _retEvents.reset(p);
    }
    ~epoll()
    {
    }
    cf_void AddConn(cf_fd fd,cf_ev event =EPOLLIN | EPOLLRDHUP)
    {
        EpollCtl(fd,EPOLL_CTL_ADD, event);
        _mapEvent.insert(std::make_pair(fd,event));
    }
    cf_void DelConn(cf_fd fd)
    {
        EpollCtl(fd,EPOLL_CTL_DEL, 0);
        _mapEvent.erase(fd);
    }
    cf_void AddEvent(cf_fd fd,cf_ev event)
    {
        TYPE_MAPEVENT_ITER it =_mapEvent.find(fd);
        if(_mapEvent.end()!=it)
        {
            cf_ev oldev =it->second;
            if(oldev&event)
            {
                EpollCtl(fd,EPOLL_CTL_MOD, oldev|event);
                it->second =oldev|event;
            }
            else
            {
            }
        }
        else
        {
        }
    }
    cf_void DelEvent(cf_fd fd,cf_ev event)
    {
        TYPE_MAPEVENT_ITER it =_mapEvent.find(fd);
        if(_mapEvent.end()!=it)
        {
            cf_ev oldev =it->second;
            if(oldev&event)
            {
            }
            else
            {
                EpollCtl(fd,EPOLL_CTL_MOD, oldev&(~event));
                it->second =oldev&(~event);
            }
        }
        else
        {
        }
    }
    cf_void WaitEvent(TYPE_VECEVENT & vecEvent, cf_int timeoutMilliseconds)
    {
        epoll_event * events =_retEvents.get();
        cf_int n =epoll_wait(_epfd, events,_maxEvents, timeoutMilliseconds);
        if(-1==n)
        {
        }
        else if(0==n)
        {
            for(cf_int i=0;i<n;i++)
            {
                vecEvent.push_back();
            }
        }
        else
        {
        }
    }
private:
    cf_void EpollCtl(cf_fd fd,cf_int op, cf_ev event)
    {
        epoll_data_t st_data;
        st_data.fd =fd;
        
        epoll_event st_ev;
        st_ev.events =event;
        st_ev.data =st_data;
        
        cf_int rt =epoll_ctl(_epfd, op, fd, &st_ev);
        if(0!=rt)
        {
        }
    }
    
    cf_int _epfd;
    TYPE_MAPEVENT _mapEvent;
    cf_int _maxEvents;
    std::shared_ptr < epoll_event > _retEvents;
};


}

#endif // _HEADER_FILE_CFD_CF_EPOLL_HPP_

