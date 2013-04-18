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

class Epoll : public cf::NonCopyable
{
public:
   Epoll(  cf_int listenfd,IOComplete & iocomplete,
           cf_const cf_int maxevents =256, cf_int createFlags =0):
      _listenfd(listenfd),
      _epfd(epoll_create1(createFlags)),
      _handler(_listenfd,_epfd,iocomplete),
      _maxevents(maxevents)
   {
      if(-1==_epfd)
         _THROW(cf::SyscallExecuteError, "Failed to execute epoll_create1 !")
         _events.resize(_maxevents);
      cf::SetBlocking(_listenfd,false);
      cf::AddEventEpoll(_epfd, _listenfd,_event,EPOLLIN);
   }
   ~Epoll()
   {
      cf_int rt =close(_epfd);
      rt =0; // TODO: Log warning.
   }

   cf_void AsyncRead(cf_int fd, cf_uint32 bytes)
   {
      _handler.AsyncRead(fd, bytes);
   }
   cf_void AsyncWrite(cf_int fd, cf_pvoid buff, cf_uint32 bytes)
   {
      _handler.AsyncWrite(fd, buff, bytes);
   }
   cf_void AsyncClose(cf_int fd)
   {
      _handler.AsyncClose(fd);
   }

   cf_void Wait(cf_int32 timeoutMilliseconds)
   {
      cf_int n =0;
      cf_int i =0;
      bool continueloop =true;
      while (continueloop)
      {
         n = cf_epoll_wait(_epfd, (epoll_event *)(&_events[0]), _maxevents,
                           timeoutMilliseconds);
         printf("\ncf_epoll_wait returned %d \n",n);
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
               else if (_events[i].events & EPOLLERR)
                  _handler.Error(_events[i].data.fd);
               else if (_events[i].events & EPOLLRDHUP)
                  _handler.Close(_events[i].data.fd);
               else if (_events[i].events & EPOLLIN)
                  _handler.Read(_events[i].data.fd);
               else if (_events[i].events & EPOLLOUT)
                  _handler.Write(_events[i].data.fd);
               else
                  _handler.Close(_events[i].data.fd);
            }
         }
         else if(0==n)
         {
            _handler.Timeout(_events[i].data.fd);
         }
         else
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_epoll_wait !")
         }
   }
private:
   cf_int _listenfd;
   cf_int _epfd;
   Handler _handler;
   cf_const cf_int _maxevents;
   struct epoll_event _event;
   std::vector < struct epoll_event > _events;
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

