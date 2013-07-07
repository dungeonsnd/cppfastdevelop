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

#ifndef _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_
#define _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_utility.hpp"
#include "cppfoundation/cf_event_loop.hpp"

namespace cl
{

class EventHandler : public cf::NonCopyable
{
public:
    EventHandler(cf_fd listenfd, std::shared_ptr < cf::Demux > demux)
        :_listenfd(listenfd),_demux(demux)
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

    cf_void OnAccept()
    {
        CF_PRINT_FUNC;
        while (true)
        {
            struct sockaddr in_addr;
            socklen_t in_len;
            cf_int infd;
            cf_char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

            in_len = sizeof in_addr;
            infd = cf_accept (_listenfd, &in_addr, &in_len);
            if (infd == -1)
            {
                if ((errno == EAGAIN)||(errno == EWOULDBLOCK))
                    break;
                else
                {
                    _THROW(cf::SyscallExecuteError, "Failed to execute accept !");
                    break;
                }
            }

            cf_int s = getnameinfo(&in_addr, in_len,hbuf, sizeof hbuf,sbuf, sizeof sbuf,
                                   NI_NUMERICHOST | NI_NUMERICSERV);
            if (s == 0)
            {
#if CFD_SWITCH_PRINT
                fprintf(stderr,"Accepted connection on descriptor %d(host=%s, port=%s)\n", infd,
                        hbuf, sbuf);
#endif
            }

            cf::SetBlocking(infd,false);
            _demux->AddConn(infd, cf::networkdefs::EV_READ);
        }
    }

    cf_void OnRead()
    {
        CF_PRINT_FUNC;
    }
    cf_void OnWrite()
    {
        CF_PRINT_FUNC;
    }
    cf_void OnClose()
    {
        CF_PRINT_FUNC;
    }
    cf_void OnTimeout()
    {
        CF_PRINT_FUNC;
    }
    cf_void OnError()
    {
        CF_PRINT_FUNC;
    }
private:
    cf_fd _listenfd;
    std::shared_ptr < cf::Demux > _demux;
};


} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

