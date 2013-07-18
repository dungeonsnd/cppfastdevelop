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
    }
    virtual cf_void OnReadComplete(cf::T_SESSION session,
                                   std::shared_ptr < ReadBuffer > readBuffer)
    {
    }
    virtual cf_void OnWriteComplete(cf::T_SESSION session)
    {
    }
    virtual cf_void OnCloseComplete(cf::T_SESSION session)
    {
    }
    virtual cf_void OnTimeoutComplete()
    {
    }
    virtual cf_void OnErrorComplete(cf::T_SESSION session)
    {
    }

    cf_void AsyncRead(cf_fd fd, cf_uint32 sizeToRead)
    {
        CF_PRINT_FUNC;
        std::shared_ptr < ReadBuffer > rb;
        T_MAPREADBUFFER::iterator itbuf =_readBuf.find(fd);
        if( itbuf!=_readBuf.end() )
        {
            rb =itbuf->second;
            rb->Clear();
        }
        else
        {
            rb =_rbpool.GetFromPool();
            _readBuf.insert( std::make_pair(fd,rb) );
        }
        rb->SetTotal(sizeToRead);
        _demux->AddEvent(fd, cf::networkdefs::EV_READ);
    }
    cf_void AsyncWrite(cf_fd fd, cf_cpvoid buf,cf_uint32 bufSize)
    {
        CF_PRINT_FUNC;
        std::shared_ptr < WriteBuffer > wb;
        T_MAPWRITEBUFFER::iterator itbuf =_writeBuf.find(fd);
        if( itbuf!=_writeBuf.end() )
        {
            wb =itbuf->second;
            wb->Clear();
        }
        else
        {
            CF_NEWOBJ(p, WriteBuffer);
            if(NULL==p)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            wb.reset(p);
            _writeBuf.insert( std::make_pair(fd,wb) );
        }
        wb->SetBuffer(buf,bufSize);
        _demux->AddEvent(fd, cf::networkdefs::EV_WRITE);
    }
    cf_void AsyncClose(cf_fd fd)
    {
        CF_PRINT_FUNC;
        // TODO
    }

    cf_void OnAccept()
    {
        CF_PRINT_FUNC;
        T_VECCLIENTS clients;
        cf::AcceptAsync(_listenfd, clients);
        cf_fd fd;
#if CF_SWITCH_PRINT
        fprintf (stderr, "OnAccept,clients.size()=%u \n", (cf_uint32)clients.size());
#endif
        for(T_VECCLIENTS::iterator it=clients.begin(); it!=clients.end(); it++)
        {
            cf::T_SESSION session =*it;
            fd =session->Fd();
            if(_mapSession.find(fd)==_mapSession.end())
            {
#if CF_SWITCH_PRINT
                fprintf (stderr, "OnAccept,new connection ,fd=%d \n", fd);
#endif
                _mapSession.insert( std::make_pair(fd,session) );
                cf::SetBlocking(fd,false);
                _demux->AddConn(fd, cf::networkdefs::EV_CLOSE);
                OnAcceptComplete(session);
            }
            else
            {
                //Warning
#if CF_SWITCH_PRINT
                fprintf (stderr, "OnAccept,Warning ,fd=%d \n", fd);
#endif
            }
        }
    }

    cf_void OnRead(cf_fd fd)
    {
        CF_PRINT_FUNC;
        T_MAPSESSIONS::iterator it =_mapSession.find(fd);
        T_MAPREADBUFFER::iterator itbuf =_readBuf.find(fd);
        if( it!=_mapSession.end() && itbuf!=_readBuf.end() )
        {
            cf::T_SESSION session =it->second;
            std::shared_ptr < ReadBuffer > readBuffer =itbuf->second;
            bool peerClosedWhenRead =false;
            readBuffer->Read(session, peerClosedWhenRead);
            if(peerClosedWhenRead||readBuffer->IsComplete())
            {
                _demux->DelEvent(fd, cf::networkdefs::EV_READ);
            }
            if(readBuffer->IsComplete())
            {
                OnReadComplete(session, readBuffer);
            }
        }
        else
        {
            //Warning
#if CF_SWITCH_PRINT
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
#if CF_SWITCH_PRINT
            fprintf (stderr, "OnWrite,Warning ,fd=%d \n", fd);
#endif
        }
    }
    cf_void OnClose(cf_fd fd)
    {
        // TODO: When to invoke _rbpool.PutIntoPool() and
        //       when to remove k-v from _readBuf。Alse write pool and buf.
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
#if CF_SWITCH_PRINT
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
        // TODO : Remove k-v from map, release buf from pool.
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
#if CF_SWITCH_PRINT
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

    BufferPool < ReadBuffer > _rbpool;
    BufferPool < WriteBuffer > _wbpool;
};

} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

