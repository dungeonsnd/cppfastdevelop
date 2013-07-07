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
#include "cppfoundation/cf_network.hpp"
//#include "cppfoundation/cf_ipc.hpp"

namespace cf
{

namespace epolldefs
{
enum
{
    SIZE_CREATE = 1,
    SIZE_MAXEVENTS = 8192
};
} // namespace lockdefs

class Epoll : public cf::NonCopyable
{
public:
    typedef std::map < cf_fd, cf_ev > TYPE_MAPEVENT;
    typedef TYPE_MAPEVENT::iterator TYPE_MAPEVENT_ITER;
    typedef std::vector < std::pair < cf_fd, networkdefs::EV_TYPE > > TYPE_VECEVENT;
    typedef TYPE_VECEVENT::iterator TYPE_VECEVENT_ITER;

    Epoll(cf_fd listenfd ,cf_int maxEvents =epolldefs::SIZE_MAXEVENTS)
        :_listenfd(listenfd),_maxEvents(maxEvents)
    {
        _epfd =cf_epoll_create(epolldefs::SIZE_CREATE);
        EpollCtl(_listenfd,EPOLL_CTL_ADD, EPOLLIN);
        //        EpollCtl(_pipe[0],EPOLL_CTL_ADD, EPOLLIN);
        if(_maxEvents<1)
            _THROW_FMT(ValueError, "_maxEvents{%d}<1 !",_maxEvents);
        _retEvents.resize(_maxEvents);
    }
    ~Epoll()
    {
    }
    cf_void AddConn(cf_fd fd,networkdefs::EV_TYPE ev=networkdefs::EV_READ)
    {
        cf_ev event =EPOLLRDHUP;
        if(ev&networkdefs::EV_READ)
            event |= EPOLLIN;
        if(ev&networkdefs::EV_WRITE)
            event |= EPOLLOUT;

        EpollCtl(fd,EPOLL_CTL_ADD, event);
        _mapEvent.insert(std::make_pair(fd,event));
    }
    cf_void DelConn(cf_fd fd)
    {
        EpollCtl(fd,EPOLL_CTL_DEL, 0);
        _mapEvent.erase(fd);
    }
    cf_void AddEvent(cf_fd fd,networkdefs::EV_TYPE ev)
    {
        cf_ev event =EPOLLRDHUP;
        if(ev&networkdefs::EV_READ)
            event |= EPOLLIN;
        if(ev&networkdefs::EV_WRITE)
            event |= EPOLLOUT;

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
    cf_void DelEvent(cf_fd fd,networkdefs::EV_TYPE ev)
    {
        cf_ev event =EPOLLIN;
        if(ev&networkdefs::EV_READ)
            event |= EPOLLIN;
        if(ev&networkdefs::EV_WRITE)
            event |= EPOLLOUT;

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
        _retEvents.clear();
#if CFD_SWITCH_PRINT
        fprintf (stderr, "before epoll_wait  \n");
#endif
        cf_int n =cf_epoll_wait(_epfd, &(_retEvents[0]),_maxEvents,
                                timeoutMilliseconds);
#if _DEBUG
        usleep(1000*1000); // only for testing
#endif
#if CFD_SWITCH_PRINT
        fprintf (stderr, "epoll_wait return , n=%d \n",n);
#endif

        if(-1==n)
        {
            _THROW(SyscallExecuteError, "Failed to execute epoll_wait !");
        }
        else if(0==n)
        {
#if CFD_SWITCH_PRINT
            fprintf (stderr, "epoll_wait return , time out \n");
#endif
        }
        else
        {
            for(cf_int i=0; i<n; i++)
            {
                if((_retEvents[i].events & EPOLLERR)||(_retEvents[i].events & EPOLLHUP))
                {
#if CFD_SWITCH_PRINT
                    fprintf (stderr, "epoll_wait return , EPOLLERR or EPOLLHUP\n");
#endif
                    vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                       networkdefs::EV_ERROR) );
                    continue;
                }
                else if(_retEvents[i].data.fd==_listenfd)
                {
#if CFD_SWITCH_PRINT
                    fprintf (stderr, "epoll_wait return , EV_ACCEPT\n");
#endif
                    vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                       networkdefs::EV_ACCEPT) );
                    continue;
                }
                /*
                                else if(_retEvents[i].data.fd==_pipe[1])
                                {
                #if CFD_SWITCH_PRINT
                                    fprintf (stderr, "epoll_wait return , rewait after epoll_ctl\n");
                #endif
                                    ssize_t rdn =cf::read(_pipe[1],_buf4pipe,sizeof _buf4pipe);
                                    if(sizeof _buf4pipe!=rdn)
                                        _THROW_FMT(ValueError, "sizeof _buf4pipe{%lld}!=rdn{%lld} !",(cf_int64)(sizeof _buf4pipe),(cf_int64)rdn);
                                    continue;
                                }
                */
                if(_retEvents[i].events & EPOLLIN)
                {
#if CFD_SWITCH_PRINT
                    fprintf (stderr, "epoll_wait return , EPOLLIN\n");
#endif
                    vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                       networkdefs::EV_READ) );
                }
                if(_retEvents[i].events & EPOLLOUT)
                {
#if CFD_SWITCH_PRINT
                    fprintf (stderr, "epoll_wait return , EV_WRITE\n");
#endif
                    vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                       networkdefs::EV_WRITE) );
                }
                if(_retEvents[i].events & EPOLLRDHUP)
                {
#if CFD_SWITCH_PRINT
                    fprintf (stderr, "epoll_wait return , EPOLLRDHUP\n");
#endif
                    vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                       networkdefs::EV_CLOSE) );
                }
            }
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

        cf_int rt =cf_epoll_ctl(_epfd, op, fd, &st_ev);
        if(0!=rt)
        {
        }
#if CFD_SWITCH_PRINT
        fprintf (stderr, "cf_epoll_ctl return %d, _epfd=%d, fd=%d \n" , rt,_epfd,fd);
#endif
    }

    cf_fd _listenfd;
    cf_int _epfd;
    TYPE_MAPEVENT _mapEvent;
    cf_int _maxEvents;
    std::vector < epoll_event > _retEvents;
    //    Pipe < SocketpairPipe > _pipe;
    //    cf_char _buf4pipe[1];
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_EPOLL_HPP_

