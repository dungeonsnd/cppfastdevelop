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

namespace eventhandlerdefs
{
enum
{
    SIZE_MAXCONN = 200000
};
} // namespace eventhandlerdefs

class EventHandler : public cf::NonCopyable
{
public:
    typedef std::list < std::shared_ptr < ReadBuffer > > T_READBUFFER_CHAIN;
    typedef std::list < std::shared_ptr < WriteBuffer > > T_WRITEBUFFER_CHAIN;

    typedef struct _tagClientInfo
    {
        cf::T_SESSION session;
        cf_fd listenfd;
        T_READBUFFER_CHAIN rbChain;
        T_WRITEBUFFER_CHAIN wbChain;
        int rbChainSize;
        int wbChainSize;
    } ClientInfo;
    typedef struct _tagListenInfo
    {
        cf_uint32 maxconns;
        cf_uint32 currentconns;
    } ListenInfo;

    typedef std::unordered_map < cf_fd,std::shared_ptr < ClientInfo >  >
    T_CLIENTFD_CLIENTINFO;
    typedef std::unordered_map < cf_fd,std::shared_ptr < ListenInfo >  >
    T_LISTENFD_LISTENINFO;

    typedef std::unordered_map < cf_fd,cf_uint32  > T_LISTENFD_MAXCONN;

    EventHandler():
        _processid(0),
        _WaterMark_maxRbChainLen(0),
        _WaterMark_maxWbChainLen(0),
        _WaterMark_maxConnCnt(0)
    {
    }
    ~EventHandler()
    {
    }
    void Init(  const T_LISTENFD_MAXCONN & listenfd_maxconn,
                std::shared_ptr < cf::Demux > demux,
                cf_int processid);

    virtual cf_void OnAcceptComplete(cf_fd listenfd,cf::T_SESSION session)
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

    cf_void AsyncRead(cf_fd fd, cf_uint32 sizeToRead);
    cf_void AsyncWrite(cf_fd fd, cf_cpvoid buf,cf_uint32 bufSize);
    cf_void AsyncClose(cf_fd fd);

    cf_void OnAccept(cf_fd listenfd);
    cf_void OnRead(cf_fd fd);
    cf_void OnWrite(cf_fd fd);
    cf_void OnClose(cf_fd fd);
    cf_void OnTimeout();
    cf_void OnError(cf_fd fd);

protected:
    cf::T_SESSION AddNewConn(cf_fd fd,cf_cpstr ip,cf_uint32 port,bool callback);

    cf_void StartListening(cf_fd listenfd);
    cf_void StopListening(cf_fd listenfd);

private:
    std::shared_ptr < ClientInfo > GetClientInfo(cf_fd fd,
            bool eraseWhenFound=false);
    std::shared_ptr < ListenInfo > GetListenInfo(cf_fd listenfd);
    std::shared_ptr < ReadBuffer > GetReadBuffer(cf_uint32 sizeToRead);
    std::shared_ptr < WriteBuffer > GetWriteBuffer(cf_cpvoid buf,cf_uint32 bufSize);

    cf_void AddNewConn(cf::T_SESSION session,bool callback,cf_fd listenfd);
    cf_void ClearSessionAndBuffer(cf::T_SESSION session);
    cf_void CheckMaxConnection(cf_fd listenfd,bool newConnCheck);
    cf_void DumpStatus();
private:
    std::shared_ptr < cf::Demux > _demux;
    cf_int _processid;

    T_CLIENTFD_CLIENTINFO _clientfd_clientinfo;
    T_LISTENFD_LISTENINFO _listenfd_listeninfo;

    BufferPool < ReadBuffer > _rbpool;
    BufferPool < WriteBuffer > _wbpool;

    // Status logging.
    int _WaterMark_maxRbChainLen;
    int _WaterMark_maxWbChainLen;
    int _WaterMark_maxConnCnt;
};

} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

