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

#ifndef _HEADER_FILE_CFD_CL_HANDLER_HPP_
#define _HEADER_FILE_CFD_CL_HANDLER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_io_utility.hpp"
#include "netserver/cl_net_buffer.hpp"

namespace cl
{
namespace ns
{

class Handler : public cf::NonCopyable
{
public:
    Handler()
    {
    }
    ~Handler()
    {
    }
    void OnAccept(int epfd, int fd)
    {
        struct sockaddr in_addr;
        socklen_t in_len =sizeof(in_addr);
        int clientfd =0;
        int rt =0;
         char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        while (1)
        {
            clientfd =cf_accept(fd, &in_addr, &in_len);
            if(-1 == clientfd)
            {
                if ( (errno == EAGAIN)||(errno == EWOULDBLOCK) )
                  break;
                else
                {
                    _THROW(cf::SyscallExecuteError, "Failed to execute cf_accept !")
                    break;
                }
            }
            rt = getnameinfo (&in_addr,in_len,hbuf, sizeof(hbuf),sbuf, sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);
            if (0==rt)
            {
              printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", clientfd, hbuf, sbuf);
            }
            else
                ; //Log.
            
            cf::SetBlocking(clientfd,false);
            struct epoll_event event;
            event.data.fd = clientfd;
            event.events = EPOLLIN;
            if (-1==cf_epoll_ctl (epfd, EPOLL_CTL_ADD, clientfd, &event))
                _THROW(cf::SyscallExecuteError, "Failed to execute cf_epoll_ctl !")
        }
    }
    void OnRead(int fd)
    {
        // cf_read
        // _netbuf.append()
    }
    void OnWrite(int fd)
    {
        // cf_write
        // _netbuf.remove()
    }
    void OnTimeout()
    {
    }
    void OnClose(int fd)
    {
        // cf_close
        // _netbuf.erase(fd)
    }
    void OnError(int fd)
    {
        // cf_close
        // _netbuf.erase(fd)
    }

private:
    std::map < int , std::shared_ptr<NetBuffer> > _netbuf;
};

} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_HANDLER_HPP_
