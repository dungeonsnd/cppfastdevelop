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

#ifndef _HEADER_FILE_CFD_CL_EVENT_LOOP_HPP_
#define _HEADER_FILE_CFD_CL_EVENT_LOOP_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_io_utility.hpp"
#include "netserver/cl_handler.hpp"

namespace cl
{
namespace ns
{

template < typename HandlerType >
class Epoll : public cf::NonCopyable
{
public:
    Epoll(cf_int listenfd, const int maxevents =256, cf_int createFlags =0):
        _listenfd(listenfd),
        _maxevents(maxevents)
    {
        cf::SetBlocking(_listenfd,false);
        _epfd = epoll_create1(createFlags);
        
        _event.data.fd = _listenfd;
        _event.events = EPOLLIN;
        if (-1==cf_epoll_ctl (_epfd, EPOLL_CTL_ADD, _listenfd, &_event))
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_epoll_ctl !")
    }
    ~Epoll()
    {
        int rt =close(_epfd);
        rt =0; // TODO: Log warning.
    }
    
    void AsyncRead(cf_int fd, cf_uint32 bytes)
    {
        _event.data.fd = fd;
        _event.events = EPOLLIN | EPOLLONESHOT;
        if (-1==cf_epoll_ctl (_epfd, EPOLL_CTL_ADD, _listenfd, &_event))
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_epoll_ctl !")
    }
    void AsyncWrite(cf_int fd, const void * buff, cf_uint32 bytes)
    {
        _event.data.fd = fd;
        _event.events = EPOLLOUT;
        if (-1==cf_epoll_ctl (_epfd, EPOLL_CTL_DEL, _listenfd, &_event))
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_epoll_ctl !")
    }
    
    cf_void WaitEvents(cf_int32 timeoutMilliseconds)
    {
        int n,i;
        bool continueloop =true;
        while (continueloop)
        {
            n = cf_epoll_wait(_epfd, (epoll_event *)(&_events[0]), _maxevents, timeoutMilliseconds);
            if(n>0)
            {
                for (i = 0; i < n; i++)
                {
                    if (_listenfd == _events[i].data.fd)
                    {
                        if (!(_events[i].events & EPOLLIN))
                            continueloop =false;
                        else
                            _handler.Accept(_epfd, _events[i].data.fd);
                    }
                    else if (_events[i].events & EPOLLIN)
                        _handler.Read(_events[i].data.fd);
                    else if (_events[i].events & EPOLLOUT)
                        _handler.Write(_events[i].data.fd);
                    else if (_events[i].events & EPOLLRDHUP)
                        _handler.Close(_events[i].data.fd);
                    else if (_events[i].events & EPOLLERR)
                        _handler.Error(_events[i].data.fd);
                }
            }
            else if(0==n)
            {
                _handler.Timeout();
            }
            else 
                _THROW(cf::SyscallExecuteError, "Failed to execute cf_epoll_wait !")
        }
    }
private:
    
    int _listenfd;
    const int _maxevents;
    int _epfd;
    struct epoll_event _event;
    std::vector < struct epoll_event > _events;
    
    HandlerType _handler;
};

template < typename demux >
class EventLoop : public cf::NonCopyable
{
public:
    EventLoop()
    {
    }
    ~EventLoop()
    {
    }
    cf_void WaitEvents(cf_int32 timeoutMilliseconds)
    {
    }
};

} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_LOOP_HPP_

