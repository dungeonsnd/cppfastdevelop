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
#include "cppfoundation/cf_network.hpp"
#include "cppfoundation/cf_socket.hpp"
#include "cppfoundation/cf_event_loop.hpp"
#include "netserver/cl_buffer.hpp"

namespace cl
{

class EventHandler : public cf::NonCopyable
{
public:

    typedef std::map < cf_fd, cf::T_SESSION  > T_MAPSESSIONS;
    typedef std::vector < cf::T_SESSION > T_VECCLIENTS;

    typedef std::map < cf_fd, std::shared_ptr < ReadBuffer >  > T_MAPREADBUFFER;
    typedef std::map < cf_fd, std::shared_ptr < WriteBuffer >  > T_MAPWRITEBUFFER;
    typedef std::map < cf_fd, std::shared_ptr < Packet >  > T_MAPPACKET;

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
        T_VECCLIENTS clients;
        cf::AcceptAsync(_listenfd, clients);
        cf_fd fd;
        for(T_VECCLIENTS::iterator it=clients.begin(); it!=clients.end(); it++)
        {
            fd =(*it)->Fd();
            if(_mapSession.find(fd)==_mapSession.end())
            {
                _mapSession.insert( std::make_pair(fd,*it) );
                cf::SetBlocking(fd,false);
                _demux->AddConn(fd, cf::networkdefs::EV_READ);
#if CFD_SWITCH_PRINT
                cf_uint32 ip;
                std::string addr;
                cf_uint16 port;
                ip =(*it)->Ip();
                addr =(*it)->Addr();
                port =(*it)->Port();
                fprintf (stderr, "clients , fd=%d,ip=%0x,addr=%s,port=%u \n",fd,ip,addr.c_str(),
                         port);
#endif
            }
            else
            {
            }
        }
    }

    cf_void OnRead(cf_fd fd)
    {
        CF_PRINT_FUNC;
        T_MAPSESSIONS::iterator it =_mapSession.find(fd);
        if(it!=_mapSession.end())
        {
            cf::T_SESSION session =it->second;
            std::string buf(8192,'\0');
            cf_int rdn =session->RecvAsync(&buf[0], 6);
            if(rdn==6)
                _demux->DelEvent(fd, cf::networkdefs::EV_READ);

#if CFD_SWITCH_PRINT
            fprintf (stderr, "clients,fd=%d,addr=%s,rdn=%d,buf=%s \n",
                     session->Fd(),session->Addr().c_str(),rdn,buf.c_str());
#endif
        }
        else
        {
        }
    }
    cf_void OnWrite(cf_fd fd)
    {
        CF_PRINT_FUNC;
    }
    cf_void OnClose(cf_fd fd)
    {
        CF_PRINT_FUNC;
    }
    cf_void OnTimeout()
    {
        CF_PRINT_FUNC;
    }
    cf_void OnError(cf_fd fd)
    {
        CF_PRINT_FUNC;
    }
private:
    cf_fd _listenfd;
    std::shared_ptr < cf::Demux > _demux;
    T_MAPSESSIONS _mapSession;

    T_MAPREADBUFFER _readBuf;
    T_MAPWRITEBUFFER _writeBuf;
    T_MAPPACKET _packet;
};


} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

