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
#include "cppfoundation/cf_exception.hpp"

namespace cl
{

class CheckTime
{
public:
    CheckTime(cf_cpstr msg):seconds(0),useconds(0),_msg(msg)
    {
        cf::Gettimeofday(seconds, useconds);
    }
    ~CheckTime()
    {
        cf_uint64 seconds1 =0;
        cf_uint32 useconds1 =0;
        cf::Gettimeofday(seconds1, useconds1);

        cf_uint64 u =seconds1*1000000+useconds1-(seconds*1000000+useconds);

        fprintf (stdout, "%s ,%llu sec %llu usec , pid=%u\n",
                 _msg,u/1000000,u%1000000,
                 (cf_uint32)getpid());
    }
private:
    cf_uint64 seconds;
    cf_uint32 useconds;
    cf_cpstr _msg;
};

void EventHandler::Init(const T_LISTENFD_MAXCONN & listenfd_maxconn,
                        std::shared_ptr < cf::Demux > demux,
                        cf_int processid)
{
    for(T_LISTENFD_MAXCONN::const_iterator it =listenfd_maxconn.begin();
        it!=listenfd_maxconn.end(); it++)
    {
        CF_NEWOBJ(p, ListenInfo);
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        std::shared_ptr < ListenInfo > li(p);
        li->maxconns =it->second;
        li->currentconns =0;
        _listenfd_listeninfo.insert(std::make_pair(it->first,li));
    }

    _demux =demux;
    _processid =processid;
}

std::shared_ptr < EventHandler::ClientInfo > EventHandler::GetClientInfo(
    cf_fd fd,bool eraseWhenFound)
{
    CF_PRINT_FUNC;
    T_CLIENTFD_CLIENTINFO::iterator it =_clientfd_clientinfo.find(fd);
    if(it==_clientfd_clientinfo.end())
        _THROW_FMT(cf::ValueError,
                   "GetClientInfo ,client{%d} not found in _clientfd_clientinfo !",
                   fd);
    std::shared_ptr < EventHandler::ClientInfo > ci =it->second;
    if(eraseWhenFound)
        _clientfd_clientinfo.erase(it);
    return ci;
}
std::shared_ptr < EventHandler::ListenInfo > EventHandler::GetListenInfo(
    cf_fd listenfd)
{
    CF_PRINT_FUNC;
    T_LISTENFD_LISTENINFO::iterator it =_listenfd_listeninfo.find(listenfd);
    if(it==_listenfd_listeninfo.end())
        _THROW_FMT(cf::ValueError,
                   "GetListenInfo ,listenfd{%d} not found in _listenfd_listeninfo !",
                   listenfd);
    return it->second;
}
std::shared_ptr < ReadBuffer > EventHandler::GetReadBuffer(cf_uint32 sizeToRead)
{
    CF_PRINT_FUNC;
    std::shared_ptr < ReadBuffer > rb =_rbpool.GetFromPool();
    rb->Clear();
    rb->SetTotal(sizeToRead);
    return rb;
}
std::shared_ptr < WriteBuffer > EventHandler::GetWriteBuffer(cf_cpvoid buf,
        cf_uint32 bufSize)
{
    CF_PRINT_FUNC;
    std::shared_ptr < WriteBuffer > wb =_wbpool.GetFromPool();
    wb->Clear();
    wb->SetBuffer(buf,bufSize);
    return wb;
}

cf_void EventHandler::AsyncRead(cf_fd fd, cf_uint32 sizeToRead)
{
    CF_PRINT_FUNC;
#if CF_SWITCH_PRINT
    fprintf (stdout, "AsyncRead ,fd=%d,sizeToRead=%u \n",fd,sizeToRead);
#endif
    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd);
    T_READBUFFER_CHAIN & rbChain =ci->rbChain;

    std::shared_ptr < ReadBuffer > rb =GetReadBuffer(sizeToRead);
    rbChain.push_back(rb);
#if CF_SWITCH_PRINT
    fprintf (stdout, "AsyncRead , notice , add one to chain. \n");
#endif
    _demux->AddEvent(fd, cf::networkdefs::CFEV_READ);

    // logging.
    if( ci->rbChainSize > _WaterMark_maxRbChainLen )
        _WaterMark_maxRbChainLen =ci->rbChainSize;
}
cf_void EventHandler::AsyncWrite(cf_fd fd, cf_cpvoid buf,cf_uint32 bufSize)
{
    CF_PRINT_FUNC;
    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd);
    T_WRITEBUFFER_CHAIN & wbChain =ci->wbChain;

    std::shared_ptr < WriteBuffer > wb =GetWriteBuffer(buf,bufSize);
    wbChain.push_back(wb);
#if 1
    fprintf (stdout, "AsyncWrite , notice , add one to chain.wbChain.size=%d \n",
             ci->wbChainSize);
#endif
    _demux->AddEvent(fd, cf::networkdefs::CFEV_WRITE);

    // logging.
    if( ci->wbChainSize>_WaterMark_maxWbChainLen )
        _WaterMark_maxWbChainLen =ci->wbChainSize;
}
cf_void EventHandler::AsyncClose(cf_fd fd)
{
    CF_PRINT_FUNC;
    // TODO: Need test. Be careful about _rbpool.PutIntoPool() and removing k-v from _clientfd_rbchain.
    // Shall we support half-close ?
    OnClose(fd);
    /*
            T_CLIENTFD_SESSION::iterator it =_clientfd_session.find(fd);
            if( it!=_clientfd_session.end() )
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

cf_void EventHandler::StartListening(cf_fd listenfd)
{
    CF_PRINT_FUNC;
    _demux->AddConn(listenfd, cf::networkdefs::CFEV_READ);
}
cf_void EventHandler::StopListening(cf_fd listenfd)
{
    CF_PRINT_FUNC;
    _demux->DelConn(listenfd);
}

cf_void EventHandler::OnAccept(cf_fd listenfd)
{
    CF_PRINT_FUNC;
    typedef std::vector < cf::T_SESSION > T_VECCLIENTS;
    T_VECCLIENTS clients;
    if(false==cf::AcceptAsync(listenfd, clients))
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
        if(_clientfd_clientinfo.find(fd)==_clientfd_clientinfo.end())
        {
#if CF_SWITCH_PRINT
            fprintf (stdout, "OnAccept,new connection ,fd=%d,pid=%d \n", fd,int(getpid()));
#endif
            AddNewConn(session,true,listenfd);
        }
        else
        {
            //Warning
#if 1
            fprintf (stdout,
                     "OnAccept,Warning ,fd=%d is already in _clientfd_clientinfo \n", fd);
#endif
        }
    }


    // load balance.
    /*         static RawFileRWMutex lk("/tmp/netserverload.lock");
            lk.WriteLock();
            LockGuardUnlock lg(lk);
            cf_int fd =lk.GetFd();
            cf_write(); */

    CheckMaxConnection(listenfd,true);

#if CF_SWITCH_PRINT
    fprintf (stdout, "OnAccept,conncount=%u,pid=%d\n",
             (cf_uint32)_clientfd_clientinfo.size(),int(getpid()));
#endif
}

cf_void EventHandler::OnRead(cf_fd fd)
{
    CF_PRINT_FUNC;
    CheckTime("OnRead");
    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd);
    cf::T_SESSION session =ci->session;
    T_READBUFFER_CHAIN & rbChain =ci->rbChain;
    T_READBUFFER_CHAIN::iterator iterLastToRemove =rbChain.end();
    T_READBUFFER_CHAIN::iterator iterEnd =rbChain.end();
    --iterEnd;
    std::vector < std::shared_ptr<ReadBuffer> > rbComplete;
    // Read data.
    for(T_READBUFFER_CHAIN::iterator iter =rbChain.begin();
        iter!=rbChain.end(); iter++)
    {
        std::shared_ptr<ReadBuffer> rb =*iter;
        bool peerClosedWhenRead =false;
        rb->Read(session, peerClosedWhenRead);
        if(peerClosedWhenRead)
        {
            _demux->DelEvent(fd, cf::networkdefs::CFEV_READ);
            break;
        }
        if(rb->IsComplete())
        {
            iterLastToRemove =iter;
            rbComplete.push_back(rb);
        }
        if(iter==iterEnd && rb->IsComplete()) // All in the chain is complete.
        {
            _demux->DelEvent(fd, cf::networkdefs::CFEV_READ);
        }
        if(!rb->IsComplete())
            break;
    }

    // Remove chains.
    if(iterLastToRemove==iterEnd) // clear all , so remove value directly.
    {
        rbChain.clear();
    }
    else if(iterLastToRemove!=rbChain.end())
    {
        rbChain.erase(rbChain.begin(),++iterLastToRemove);
    }

    // Call complete functions.
    for ( std::vector < std::shared_ptr<ReadBuffer> >::iterator iter=
              rbComplete.begin();
          iter!=rbComplete.end(); iter++)
    {
        std::shared_ptr<ReadBuffer> rb =*iter;
        OnReadComplete(session, rb);
        _rbpool.PutIntoPool(rb);
    }

#if 0
    printf("++++++++, OnRead, pid=%d \n",int(getpid()));
    DumpStatus();
#endif
}

cf_void EventHandler::OnWrite(cf_fd fd)
{
    CF_PRINT_FUNC;
    CheckTime("OnWrite");
    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd);
    cf::T_SESSION session =ci->session;
    T_WRITEBUFFER_CHAIN & wbChain =ci->wbChain;

    T_WRITEBUFFER_CHAIN::iterator iterLastToRemove =wbChain.end();
    T_WRITEBUFFER_CHAIN::iterator iterEnd =wbChain.end();
    --iterEnd;

    std::vector < std::shared_ptr<WriteBuffer> > wbComplete;

    // Write data.
    for(T_WRITEBUFFER_CHAIN::iterator iter =wbChain.begin();
        iter!=wbChain.end(); iter++)
    {
        std::shared_ptr<WriteBuffer> wb =*iter;
        wb->Write(session);
        if(wb->IsComplete())
        {
            iterLastToRemove =iter;
            wbComplete.push_back(wb);
        }
        if(iter==iterEnd && wb->IsComplete()) // All in the chain is complete.
        {
            _demux->DelEvent(fd, cf::networkdefs::CFEV_WRITE);
        }
        if(!wb->IsComplete())
            break;
    }

    // Remove chains.
    if(iterLastToRemove==iterEnd) // clear all , so remove value directly.
    {
        wbChain.clear();
    }
    else if(iterLastToRemove!=wbChain.end())
    {
        wbChain.erase(wbChain.begin(),++iterLastToRemove);
    }

    // Call complete functions.
    for ( std::vector < std::shared_ptr<WriteBuffer> >::iterator iter=
              wbComplete.begin();
          iter!=wbComplete.end(); iter++)
    {
        std::shared_ptr<WriteBuffer> wb =*iter;
        OnWriteComplete(session);
        _wbpool.PutIntoPool(wb);
    }
}
cf_void EventHandler::OnClose(cf_fd fd)
{
    // TODO: Need test. Be careful about _rbpool.PutIntoPool() and removing k-v from _clientfd_rbchain.
    CF_PRINT_FUNC;
    _demux->DelConn(fd);

    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd);
    cf::T_SESSION session =ci->session;
    OnCloseComplete(session);
    ClearSessionAndBuffer(session);
}
cf_void EventHandler::OnTimeout()
{
    CF_PRINT_FUNC;
    OnTimeoutComplete();
    DumpStatus();
}
cf_void EventHandler::OnError(cf_fd fd)
{
    // TODO :  Need test. Be careful about _rbpool.PutIntoPool() and removing k-v from _clientfd_rbchain.
    CF_PRINT_FUNC;
    _demux->DelConn(fd);

    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd);
    cf::T_SESSION session =ci->session;
    OnErrorComplete(session);
    ClearSessionAndBuffer(session);
}

cf_void EventHandler::AddNewConn(cf::T_SESSION session,
                                 bool callback,cf_fd listenfd)
{
    CF_PRINT_FUNC;
    cf_fd fd =session->Fd();

    CF_NEWOBJ(p, ClientInfo);
    if(NULL==p)
        _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
    std::shared_ptr < ClientInfo > ci(p);
    p->session =session;
    p->listenfd =listenfd;

    T_READBUFFER_CHAIN rbChain;
    p->rbChain =rbChain;
    T_WRITEBUFFER_CHAIN wbChain;
    p->wbChain =wbChain;

    _clientfd_clientinfo.insert( std::make_pair(fd,ci) );

    cf::SetBlocking(fd,false);
    _demux->AddConn(fd, cf::networkdefs::CFEV_CLOSE);
    if(callback)
    {
        OnAcceptComplete(listenfd,session);
    }

    std::shared_ptr < ListenInfo > li =GetListenInfo(listenfd);
    li->currentconns++;

    // logging.
    if(int(_clientfd_clientinfo.size())>_WaterMark_maxConnCnt)
        _WaterMark_maxConnCnt =int(_clientfd_clientinfo.size());
}
cf::T_SESSION EventHandler::AddNewConn(cf_fd fd,cf_cpstr ip,cf_uint32 port,
                                       bool callback)
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
    AddNewConn(ses, callback,-1);
    return ses;
}


cf_void EventHandler::ClearSessionAndBuffer(cf::T_SESSION session)
{
    CF_PRINT_FUNC;
    cf_fd fd =session->Fd();
    std::shared_ptr < ClientInfo > ci =GetClientInfo(fd,true);
    cf_fd listenfd =ci->listenfd;

    std::shared_ptr < ListenInfo > li =GetListenInfo(listenfd);
    li->currentconns--;

    // load balance.
    CheckMaxConnection(listenfd,false);

    //Clear buffer
    T_READBUFFER_CHAIN & rbChain =ci->rbChain;
    for(T_READBUFFER_CHAIN::iterator iter =rbChain.begin();
        iter!=rbChain.end(); iter++)
    {
        std::shared_ptr < ReadBuffer > rb =*iter;
        _rbpool.PutIntoPool(rb);
    }

    T_WRITEBUFFER_CHAIN & wbChain =ci->wbChain;
    for(T_WRITEBUFFER_CHAIN::iterator iter =wbChain.begin();
        iter!=wbChain.end(); iter++)
    {
        std::shared_ptr < WriteBuffer > wb =*iter;
        _wbpool.PutIntoPool(wb);
    }
}


cf_void EventHandler::CheckMaxConnection(cf_fd listenfd,bool newConnCheck)
{
    CF_PRINT_FUNC;
    std::shared_ptr < ListenInfo > li =GetListenInfo(listenfd);
    cf_uint32 maxconns =li->maxconns;
    cf_uint32 currentconns =li->currentconns;

#if 0
    fprintf ( stdout,
              "CheckMaxConnection,maxconns{%lld},currentconns{%lld},pid=%d\n",
              (cf_int64)maxconns,(cf_int64)currentconns,int(getpid()) );
#endif

    if (newConnCheck && maxconns>0&&currentconns>=maxconns)
    {
#if 0
        fprintf ( stdout,
                  "CheckMaxConnection,DelConn(listenfd=%d),currentconns{%lld}>=maxconns{%lld} ,pid=%d\n",
                  listenfd,(cf_int64)currentconns,(cf_int64)maxconns,
                  int(getpid()) );
#endif
        _demux->DelConn(listenfd);
    }

    if(false==newConnCheck && maxconns>0&&currentconns<maxconns)
    {
#if 0
        fprintf ( stdout,
                  "CheckMaxConnection,AddConn(listenfd=%d),currentconns{%lld}>=maxconns{%lld} ,pid=%d\n",
                  listenfd,(cf_int64)currentconns,(cf_int64)maxconns,
                  int(getpid()) );
#endif
        _demux->AddConn(listenfd, cf::networkdefs::CFEV_READ);
    }
}


cf_void EventHandler::DumpStatus()
{
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");

    printf("maxRbChainLen=%d, maxWbChainLen=%d, maxConnCnt=%d  ,pid=%d\n",
           _WaterMark_maxRbChainLen,
           _WaterMark_maxWbChainLen,
           _WaterMark_maxConnCnt,
           int(getpid()));

    T_LISTENFD_LISTENINFO::iterator it1 =_listenfd_listeninfo.begin();
    printf("_listenfd_listeninfo.size()=%u  ,pid=%d: \n",
           (cf_uint32)(_listenfd_listeninfo.size()),
           int(getpid()));
    for(; it1!=_listenfd_listeninfo.end(); )
    {
        std::shared_ptr < ListenInfo > li =it1->second;
        printf("    listenfd=%d --> [currentconns=%u,maxconns=%u]  ,pid=%d",it1->first,
               li->currentconns,li->maxconns,
               int(getpid()));
        it1++;
        printf("\n");
    }

    T_CLIENTFD_CLIENTINFO::iterator it2 =_clientfd_clientinfo.begin();
    printf("_clientfd_clientinfo.size()=%u  ,pid=%d: \n",
           (cf_uint32)(_clientfd_clientinfo.size()),
           int(getpid()));
    for(; it2!=_clientfd_clientinfo.end(); )
    {
        printf("    %d",it2->first);
        it2++;
        if(it2!=_clientfd_clientinfo.end())
            printf(", ");
        else
            printf(" \n");
    }

    printf("_rbpool  ,pid=%d: \n",
           int(getpid()));
    _rbpool.DumpStatus();
    printf("_wbpool  ,pid=%d: \n",
           int(getpid()));
    _wbpool.DumpStatus();
    printf("--------------------------------------------- \n");
}

} // namespace cl

