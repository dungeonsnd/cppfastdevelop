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

#include "netserver/cl_event_handler.hpp"
#include "cppfoundation/cf_memory.hpp"

namespace cl
{

void EventHandler::Init(cf_fd listenfd, std::shared_ptr < cf::Demux > demux,
                        cf_int index, cf_uint maxConnections)
{
    _listenfd =listenfd;
    _demux =demux;
    _index =index;
    _maxConnections =maxConnections;
}

cf_void EventHandler::AsyncRead(cf_fd fd, cf_uint32 sizeToRead)
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
        rb->Clear();
        _readBuf.insert( std::make_pair(fd,rb) );
    }
    rb->SetTotal(sizeToRead);
    _demux->AddEvent(fd, cf::networkdefs::EV_READ);
}
cf_void EventHandler::AsyncWrite(cf_fd fd, cf_cpvoid buf,cf_uint32 bufSize)
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
        wb =_wbpool.GetFromPool();
        wb->Clear();
        _writeBuf.insert( std::make_pair(fd,wb) );
    }
    wb->SetBuffer(buf,bufSize);
    _demux->AddEvent(fd, cf::networkdefs::EV_WRITE);
}
cf_void EventHandler::AsyncClose(cf_fd fd)
{
    CF_PRINT_FUNC;
    // TODO: Need test. Be careful about _rbpool.PutIntoPool() and removing k-v from _readBuf.
    // Shall we support half-close ?
    OnClose(fd);
    /*
            T_MAPSESSIONS::iterator it =_mapSession.find(fd);
            if( it!=_mapSession.end() )
            {
                cf::T_SESSION session =it->second;
                OnCloseComplete(session); // Don't need to notify user,i think.
            }
            else
            {
                //Warning
    #if CF_SWITCH_PRINT
                fprintf (stdout, "OnClose,Warning ,fd=%d \n", fd);
    #endif
            }
    */
}

cf_void EventHandler::StartListening()
{
    CF_PRINT_FUNC;
    _demux->AddConn(_listenfd, cf::networkdefs::EV_READ);
}
cf_void EventHandler::StopListening()
{
    CF_PRINT_FUNC;
    _demux->DelConn(_listenfd);
}

cf_void EventHandler::OnAccept()
{
    CF_PRINT_FUNC;
    T_VECCLIENTS clients;
    if(false==cf::AcceptAsync(_listenfd, clients))
        return;
    cf_fd fd =0;
#if CF_SWITCH_PRINT
    fprintf (stdout, "OnAccept,clients.size()=%u,pid=%d\n",
             (cf_uint32)clients.size(),int(getpid()));
#endif

    for(T_VECCLIENTS::iterator it=clients.begin(); it!=clients.end(); it++)
    {
        cf::T_SESSION session =*it;
        fd =session->Fd();
        if(_mapSession.find(fd)==_mapSession.end())
        {
#if CF_SWITCH_PRINT
            fprintf (stdout, "OnAccept,new connection ,fd=%d,pid=%d \n", fd,int(getpid()));
#endif
            AddNewConn(fd,session);
        }
        else
        {
            //Warning
#if 1
            fprintf (stdout, "OnAccept,Warning ,fd=%d \n", fd);
#endif
        }
    }


    // load balance.
    /*         static RawFileRWMutex lk("/tmp/netserverload.lock");
            lk.WriteLock();
            LockGuardUnlock lg(lk);
            cf_int fd =lk.GetFd();
            cf_write(); */
    cf_uint32 conncount =(cf_uint32)_mapSession.size();
#if 1
    fprintf ( stdout,
              "OnAccept,_maxConnections{%lld},conncount{%lld},pid=%d\n",
              (cf_int64)_maxConnections,(cf_int64)conncount,int(getpid()) );
#endif
    if (_maxConnections>0&&conncount>=_maxConnections)
    {
        fprintf ( stdout,
                  "OnAccept,_maxConnections{%lld}>0&&conncount{%lld}>=_maxConnections{%lld} ,pid=%d\n",
                  (cf_int64)_maxConnections,(cf_int64)conncount,(cf_int64)_maxConnections,
                  int(getpid()) );
        _demux->DelConn(_listenfd);
    }


#if CF_SWITCH_PRINT
    fprintf (stdout, "OnAccept,conncount=%u,pid=%d\n",
             (cf_uint32)_mapSession.size(),int(getpid()));
#endif
}

cf_void EventHandler::OnRead(cf_fd fd)
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
        fprintf (stdout, "OnRead,Warning ,fd=%d \n", fd);
#endif
    }
#if 0
    printf("++++++++, OnRead, pid=%d \n",int(getpid()));
    DumpStatus();
#endif
}

cf_void EventHandler::OnWrite(cf_fd fd)
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
        fprintf (stdout, "OnWrite,Warning ,fd=%d \n", fd);
#endif
    }
}
cf_void EventHandler::OnClose(cf_fd fd)
{
    // TODO: Need test. Be careful about _rbpool.PutIntoPool() and removing k-v from _readBuf.
    CF_PRINT_FUNC;
    _demux->DelConn(fd);

    T_MAPSESSIONS::iterator it =_mapSession.find(fd);
    if( it!=_mapSession.end() )
    {
        cf::T_SESSION session =it->second;
        OnCloseComplete(session);
        ClearSessionAndBuffer(fd);
    }
    else
    {
        //Warning
#if CF_SWITCH_PRINT
        fprintf (stdout, "OnClose,Warning ,fd=%d \n", fd);
#endif
    }
}
cf_void EventHandler::OnTimeout()
{
    CF_PRINT_FUNC;
    OnTimeoutComplete();
#if 1
#ifdef _DEBUG
    printf("++++++++, OnTimeout, pid=%d \n",int(getpid()));
    DumpStatus();
#endif
#endif
}
cf_void EventHandler::OnError(cf_fd fd)
{
    // TODO :  Need test. Be careful about _rbpool.PutIntoPool() and removing k-v from _readBuf.
    CF_PRINT_FUNC;
    _demux->DelConn(fd);

    T_MAPSESSIONS::iterator it =_mapSession.find(fd);
    if( it!=_mapSession.end() )
    {
        cf::T_SESSION session =it->second;
        OnErrorComplete(session);
        ClearSessionAndBuffer(fd);
    }
    else
    {
        //Warning
#if CF_SWITCH_PRINT
        fprintf (stdout, "OnError,Warning ,fd=%d \n", fd);
#endif
    }
}

cf_void EventHandler::AddNewConn(cf_fd fd, cf::T_SESSION & session)
{
    _mapSession.insert( std::make_pair(fd,session) );
    cf::SetBlocking(fd,false);
    _demux->AddConn(fd, cf::networkdefs::EV_CLOSE);
    OnAcceptComplete(session);
}
cf_void EventHandler::AddNewConn(cf_fd fd,cf_cpstr ip,cf_uint32 port)
{
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);
    cf_inet_pton(AF_INET, ip, &servaddr.sin_addr);

    cf::T_SESSION ses;
    CF_NEWOBJ(p, cf::Socket , fd, servaddr.sin_addr, htons(servaddr.sin_port));
    if(NULL==p)
        _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
    ses.reset(p);
    AddNewConn(fd, ses);
}


cf_void EventHandler::ClearSessionAndBuffer(cf_fd fd)
{
    if (1!=_mapSession.erase(fd))
    {
#if CF_SWITCH_PRINT
        fprintf (stdout, "ClearSessionAndBuffer,Warning ,fd=%d not in _mapSession \n",
                 fd);
#endif
    }


    // load balance.
#if 1
    fprintf ( stdout,
              "ClearSessionAndBuffer,_maxConnections{%lld},conncount{%lld},pid=%d\n",
              (cf_int64)_maxConnections,
              (cf_int64)(_mapSession.size()),int(getpid()));
#endif
    if(_maxConnections>0&&_mapSession.size()<_maxConnections)
    {
        _demux->AddConn(_listenfd, cf::networkdefs::EV_READ);
    }


    T_MAPREADBUFFER::iterator itrd =_readBuf.find(fd);
    if( itrd!=_readBuf.end() )
    {
        std::shared_ptr < ReadBuffer > rb =itrd->second;
        _rbpool.PutIntoPool(rb);
        _readBuf.erase(itrd);
    }
    else
    {
#if CF_SWITCH_PRINT
        fprintf (stdout, "ClearSessionAndBuffer,Warning ,fd=%d not in _readBuf \n", fd);
#endif
    }

    T_MAPWRITEBUFFER::iterator itwr =_writeBuf.find(fd);
    if( itwr!=_writeBuf.end() )
    {
        std::shared_ptr < WriteBuffer > wb =itwr->second;
        _wbpool.PutIntoPool(wb);
        _writeBuf.erase(itwr);
    }
    else
    {
#if CF_SWITCH_PRINT
        fprintf (stdout, "ClearSessionAndBuffer,Warning ,fd=%d not in _writeBuf \n",
                 fd);
#endif
    }
}

cf_void EventHandler::DumpStatus()
{
    T_MAPSESSIONS::iterator it =_mapSession.begin();
    printf("_mapSession.size()=%u: \n",(cf_uint32)(_mapSession.size()));
    for(; it!=_mapSession.end(); )
    {
        printf("%d",it->first);
        it++;
        if(it!=_mapSession.end())
            printf(", ");
        else
            printf(" \n");
    }

    T_MAPREADBUFFER::iterator itrb =_readBuf.begin();
    printf("_readBuf.size()=%u: \n",(cf_uint32)(_readBuf.size()));
    for(; itrb!=_readBuf.end();)
    {
        printf("%d",itrb->first);
        itrb++;
        if(itrb!=_readBuf.end())
            printf(", ");
        else
            printf(" \n");
    }

    T_MAPWRITEBUFFER::iterator itwb =_writeBuf.begin();
    printf("_writeBuf.size()=%u: \n",(cf_uint32)(_writeBuf.size()));
    for(; itwb!=_writeBuf.end();)
    {
        printf("%d",itwb->first);
        itwb++;
        if(itwb!=_writeBuf.end())
            printf(", ");
        else
            printf(" \n");
    }

    printf("_rbpool: \n");
    _rbpool.DumpStatus();
    printf("_wbpool: \n");
    _wbpool.DumpStatus();
}


} // namespace cl

