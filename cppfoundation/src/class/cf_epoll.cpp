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

#include "cppfoundation/cf_epoll.hpp"

namespace cf
{

Epoll::Epoll(cf_fd listenfd ,cf_int maxEvents)
    :_listenfd(listenfd),_maxEvents(maxEvents)
{
    _epfd =cf_epoll_create(epolldefs::SIZE_CREATE);
    AddConn(_listenfd,networkdefs::EV_READ);
    //        EpollCtl(_pipe[0],EPOLL_CTL_ADD, EPOLLIN);
    if(_maxEvents<1)
        _THROW_FMT(ValueError, "_maxEvents{%d}<1 !",_maxEvents);
    _retEvents.resize(_maxEvents);
}
Epoll::~Epoll()
{
}
cf_void Epoll::AddConn(cf_fd fd,networkdefs::EV_TYPE ev)
{
    CF_PRINT_FUNC;
    cf_ev event =EPOLLRDHUP;
    if(ev&networkdefs::EV_READ)
        event |= EPOLLIN;
    if(ev&networkdefs::EV_WRITE)
        event |= EPOLLOUT;

    EpollCtl(fd,EPOLL_CTL_ADD, event);
    _mapEvent.insert(std::make_pair(fd,event));
}
cf_void Epoll::DelConn(cf_fd fd)
{
    CF_PRINT_FUNC;
    EpollCtl(fd,EPOLL_CTL_DEL, 0);
    _mapEvent.erase(fd);
}
cf_void Epoll::AddEvent(cf_fd fd,networkdefs::EV_TYPE ev)
{
    CF_PRINT_FUNC;
    cf_ev event =0;
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
#if CFD_SWITCH_PRINT
            fprintf (stderr, "AddEvent , already has,oldev=%x,event=%x \n",oldev,event);
#endif
        }
        else
        {
            EpollCtl(fd,EPOLL_CTL_MOD, oldev|event);
            it->second =oldev|event;
        }
    }
    else
    {
    }
}
cf_void Epoll::DelEvent(cf_fd fd,networkdefs::EV_TYPE ev)
{
    CF_PRINT_FUNC;
    cf_ev event =0;
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
            EpollCtl(fd,EPOLL_CTL_MOD, oldev&(~event));
            it->second =oldev&(~event);
        }
        else
        {
#if CFD_SWITCH_PRINT
            fprintf (stderr, "AddEvent , already deled,oldev=%x,event=%x \n",oldev,event);
#endif
        }
    }
    else
    {
    }
}
cf_void Epoll::WaitEvent(TYPE_VECEVENT & vecEvent, cf_int timeoutMilliseconds)
{
    CF_PRINT_FUNC;
    _retEvents.clear();
    cf_int n =cf_epoll_wait(_epfd, &(_retEvents[0]),_maxEvents,
                            timeoutMilliseconds);
#if _DEBUG
    //        usleep(100*1000); // only for testing
#endif
#if CFD_SWITCH_PRINT
    fprintf (stderr, "epoll_wait return , n=%d ,pid=%d \n",n,cf_int(getpid()));
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
            if(_retEvents[i].data.fd==_listenfd)
            {
#if CFD_SWITCH_PRINT
                fprintf (stderr, "epoll_wait return , EV_ACCEPT\n");
#endif
                vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                   networkdefs::EV_ACCEPT) );
                continue;
            }
            else if((_retEvents[i].events & EPOLLERR)||(_retEvents[i].events & EPOLLHUP))
            {
#if CFD_SWITCH_PRINT
                fprintf (stderr, "epoll_wait return , EPOLLERR or EPOLLHUP\n");
#endif
                vecEvent.push_back( std::make_pair(_retEvents[i].data.fd,
                                                   networkdefs::EV_ERROR) );
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

cf_void Epoll::EpollCtl(cf_fd fd,cf_int op, cf_ev event)
{
    CF_PRINT_FUNC;
    epoll_event st_ev;
    memset(&st_ev,0,sizeof(epoll_event));
    st_ev.events =event;
    st_ev.data.fd =fd;

    cf_int rt =cf_epoll_ctl(_epfd, op, fd, &st_ev);
    if(0!=rt)
    {
    }
#if CFD_SWITCH_PRINT
    fprintf (stderr, "cf_epoll_ctl return %d, _epfd=%d, fd=%d \n" , rt,_epfd,fd);
#endif
}


} // namespace cf

