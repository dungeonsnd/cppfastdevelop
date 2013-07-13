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

    EventHandler()
    {
    }
    ~EventHandler()
    {
    }
    void Init(cf_fd listenfd, std::shared_ptr < cf::Demux > demux)
    {
        _listenfd =listenfd;
        _demux =demux;
    }

    virtual cf_void OnAcceptComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnReadComplete(cf::T_SESSION session,
                                   std::shared_ptr < ReadBuffer > readBuffer)
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnWriteComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnCloseComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnTimeoutComplete()
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnErrorComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }

    cf_void AsyncRead(cf_fd fd, cf_uint32 sizeToRead)
    {
#if CFD_SWITCH_PRINT
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ $$ before AsyncRead ,time=%llu.%u \n",seconds,useconds);
#endif
        CF_PRINT_FUNC;
#if CFD_SWITCH_PRINT
        fprintf (stderr, "AsyncRead,fd=%d ,sizeToRead=%u \n", fd, sizeToRead);
#endif
        std::shared_ptr < ReadBuffer > rb;
        T_MAPREADBUFFER::iterator itbuf =_readBuf.find(fd);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ before 1 ,time=%llu.%u \n",seconds,useconds);
#endif
        if( itbuf!=_readBuf.end() )
        {
            rb =itbuf->second;
            rb->Clear();
        }
        else
        {
#if CFD_SWITCH_PRINT
            fprintf (stderr, "AsyncRead,insert ,fd=%d \n", fd);
#endif
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr, "+++ before 2 ,time=%llu.%u \n",seconds,useconds);
#endif
            CF_NEWOBJ(p, ReadBuffer);
            if(NULL==p)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            rb.reset(p);
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr, "+++ before 3 ,time=%llu.%u \n",seconds,useconds);
#endif
            _readBuf.insert( std::make_pair(fd,rb) );
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr, "+++ before 4 ,time=%llu.%u \n",seconds,useconds);
#endif
        }
        rb->SetTotal(sizeToRead);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ before 5 ,time=%llu.%u \n",seconds,useconds);
#endif
        _demux->AddEvent(fd, cf::networkdefs::EV_READ);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "--- after AsyncRead ,time=%llu.%u \n",seconds,useconds);
#endif
    }
    cf_void AsyncWrite(cf_fd fd, cf_cpvoid buf,cf_uint32 bufSize)
    {
        CF_PRINT_FUNC;
#if CFD_SWITCH_PRINT
        fprintf (stderr, "AsyncWrite,fd=%d ,bufSize=%u \n", fd, bufSize);
#endif
        std::shared_ptr < WriteBuffer > wb;
        T_MAPWRITEBUFFER::iterator itbuf =_writeBuf.find(fd);
        if( itbuf!=_writeBuf.end() )
        {
            wb =itbuf->second;
            wb->Clear();
        }
        else
        {
#if CFD_SWITCH_PRINT
            fprintf (stderr, "AsyncWrite,insert ,fd=%d \n", fd);
#endif
            CF_NEWOBJ(p, WriteBuffer);
            if(NULL==p)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            wb.reset(p);
            _writeBuf.insert( std::make_pair(fd,wb) );
        }
        wb->SetBuffer(buf,bufSize);
        _demux->AddEvent(fd, cf::networkdefs::EV_WRITE);
    }

    cf_void OnAccept()
    {
        CF_PRINT_FUNC;
#if CFD_SWITCH_PRINT
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ before accept ,time=%llu.%u \n",seconds,useconds);
#endif
        T_VECCLIENTS clients;
        cf::AcceptAsync(_listenfd, clients);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "--- after AcceptAsync ,time=%llu.%u \n",seconds,useconds);
#endif
        cf_fd fd;
        for(T_VECCLIENTS::iterator it=clients.begin(); it!=clients.end(); it++)
        {
            cf::T_SESSION session =*it;
            fd =session->Fd();
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr, "--- before _mapSession.find ,time=%llu.%u \n",seconds,
                     useconds);
#endif
            if(_mapSession.find(fd)==_mapSession.end())
            {
#if CFD_SWITCH_PRINT
                cf_uint32 ip;
                std::string addr;
                cf_uint16 port;
                ip =session->Ip();
                addr =session->Addr();
                port =session->Port();
                fprintf (stderr, "clients ,fd=%d,ip=%0x,addr=%s,port=%u \n",fd,ip,addr.c_str(),
                         port);
#endif
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr, "--- before 1 ,time=%llu.%u \n",seconds,useconds);
#endif
                _mapSession.insert( std::make_pair(fd,session) );
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr, "--- before 2 ,time=%llu.%u \n",seconds,useconds);
#endif
                cf::SetBlocking(fd,false);
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr, "--- before 3 ,time=%llu.%u \n",seconds,useconds);
#endif
                _demux->AddConn(fd, cf::networkdefs::EV_CLOSE);
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr, "--- before 4 ,time=%llu.%u \n",seconds,useconds);
#endif
                OnAcceptComplete(session);
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr, "--- before 5 ,time=%llu.%u \n",seconds,useconds);
#endif
            }
            else
            {
                //Warning
#if CFD_SWITCH_PRINT
                fprintf (stderr, "OnAccept,Warning ,fd=%d \n", fd);
#endif
            }
        }
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "--- after accept ,time=%llu.%u \n",seconds,useconds);
#endif
    }

    cf_void OnRead(cf_fd fd)
    {
        CF_PRINT_FUNC;
#if CFD_SWITCH_PRINT
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr,
                 "+++ before _mapSession.find ,time=%llu.%u \n",seconds,useconds);
#endif
        T_MAPSESSIONS::iterator it =_mapSession.find(fd);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr,
                 "+++ after _mapSession.find ,time=%llu.%u \n",seconds,useconds);
#endif
        T_MAPREADBUFFER::iterator itbuf =_readBuf.find(fd);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr,
                 "+++ after _readBuf.find ,time=%llu.%u \n",seconds,useconds);
#endif
        if( it!=_mapSession.end() && itbuf!=_readBuf.end() )
        {
            cf::T_SESSION session =it->second;
            std::shared_ptr < ReadBuffer > readBuffer =itbuf->second;
            bool peerClosedWhenRead =false;
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr,
                     "+++ before readBuffer->Read ,time=%llu.%u \n",seconds,useconds);
#endif
            readBuffer->Read(session, peerClosedWhenRead);
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr,
                     "+++ after readBuffer->Read ,time=%llu.%u \n",seconds,useconds);
#endif
            if(peerClosedWhenRead||readBuffer->IsComplete())
            {
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr,
                         "+++ before _demux->DelEvent,time=%llu.%u \n",seconds,useconds);
#endif
                _demux->DelEvent(fd, cf::networkdefs::EV_READ);
            }
            if(readBuffer->IsComplete())
            {
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr,
                         "+++ before OnReadComplete,time=%llu.%u \n",seconds,useconds);
#endif
                OnReadComplete(session, readBuffer);
            }
        }
        else
        {
            //Warning
#if CFD_SWITCH_PRINT
            fprintf (stderr, "OnRead,Warning ,fd=%d \n", fd);
#endif
        }
    }

    cf_void OnWrite(cf_fd fd)
    {
        CF_PRINT_FUNC;
        T_MAPSESSIONS::iterator it =_mapSession.find(fd);
        T_MAPWRITEBUFFER::iterator itbuf =_writeBuf.find(fd);
        if( it!=_mapSession.end() && itbuf!=_writeBuf.end() )
        {
            cf::T_SESSION session =it->second;
            std::shared_ptr < WriteBuffer > writeBuffer =itbuf->second;
            writeBuffer->Write(session);
            if(writeBuffer->IsComplete())
            {
                _demux->DelEvent(fd, cf::networkdefs::EV_WRITE);
                OnWriteComplete(session);
            }
        }
        else
        {
            //Warning
#if CFD_SWITCH_PRINT
            fprintf (stderr, "OnWrite,Warning ,fd=%d \n", fd);
#endif
        }
    }
    cf_void OnClose(cf_fd fd)
    {
        CF_PRINT_FUNC;
        _demux->DelConn(fd);

        T_MAPSESSIONS::iterator it =_mapSession.find(fd);
        if( it!=_mapSession.end() )
        {
            cf::T_SESSION session =it->second;
            OnCloseComplete(session);
        }
        else
        {
            //Warning
#if CFD_SWITCH_PRINT
            fprintf (stderr, "OnClose,Warning ,fd=%d \n", fd);
#endif
        }
    }
    cf_void OnTimeout()
    {
        CF_PRINT_FUNC;
        OnTimeoutComplete();
    }
    cf_void OnError(cf_fd fd)
    {
        CF_PRINT_FUNC;
        _demux->DelConn(fd);

        T_MAPSESSIONS::iterator it =_mapSession.find(fd);
        if( it!=_mapSession.end() )
        {
            cf::T_SESSION session =it->second;
            OnErrorComplete(session);
        }
        else
        {
            //Warning
#if CFD_SWITCH_PRINT
            fprintf (stderr, "OnClose,Warning ,fd=%d \n", fd);
#endif
        }
    }
private:
    cf_fd _listenfd;
    std::shared_ptr < cf::Demux > _demux;
    T_MAPSESSIONS _mapSession;

    T_MAPREADBUFFER _readBuf;
    T_MAPWRITEBUFFER _writeBuf;
};

} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

