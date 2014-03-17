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
    SIZE_MAXCONN = 50
};
} // namespace eventhandlerdefs

class EventHandler : public cf::NonCopyable
{
public:

    typedef std::unordered_map < cf_fd, cf::T_SESSION  > T_MAPSESSIONS;

    typedef std::vector < cf::T_SESSION > T_VECCLIENTS;

    typedef std::list < std::shared_ptr < ReadBuffer > > T_READBUFFER_CHAIN;
    typedef std::unordered_map <  cf_fd,T_READBUFFER_CHAIN  > T_MAPREADBUFFER;

    typedef std::list < std::shared_ptr < WriteBuffer > > T_WRITEBUFFER_CHAIN;
    typedef std::unordered_map <  cf_fd,T_WRITEBUFFER_CHAIN  > T_MAPWRITEBUFFER;

    EventHandler()
    {
    }
    ~EventHandler()
    {
    }
    void Init(cf_fd listenfd, std::shared_ptr < cf::Demux > demux,
              cf_int index, cf_uint maxConnections =eventhandlerdefs::SIZE_MAXCONN);

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

    cf_void AsyncRead(cf_fd fd, cf_uint32 sizeToRead);
    cf_void AsyncWrite(cf_fd fd, cf_cpvoid buf,cf_uint32 bufSize);
    cf_void AsyncClose(cf_fd fd);

    cf_void OnAccept();
    cf_void OnRead(cf_fd fd);
    cf_void OnWrite(cf_fd fd);
    cf_void OnClose(cf_fd fd);
    cf_void OnTimeout();
    cf_void OnError(cf_fd fd);

protected:
    cf_void AddNewConn(cf_fd fd, cf::T_SESSION & session);
    cf_void AddNewConn(cf_fd fd,cf_cpstr ip,cf_uint32 port);

    cf_void StartListening();
    cf_void StopListening();

private:
    cf_void ClearSessionAndBuffer(cf_fd fd);
    cf_void DumpStatus();

    cf_fd _listenfd;
    std::shared_ptr < cf::Demux > _demux;
    cf_int _index;
    cf_uint _maxConnections;
    T_MAPSESSIONS _mapSession;

    T_MAPREADBUFFER _readBuf;
    T_MAPWRITEBUFFER _writeBuf;

    BufferPool < ReadBuffer > _rbpool;
    BufferPool < WriteBuffer > _wbpool;
};

} // namespace cl

#endif // _HEADER_FILE_CFD_CL_EVENT_HANDLER_HPP_

