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

#ifndef _HEADER_FILE_CFD_CL_SERVER_HPP_
#define _HEADER_FILE_CFD_CL_SERVER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_memory.hpp"
#include "cppfoundation/cf_network.hpp"
#include "cppfoundation/cf_event_loop.hpp"
#include "netserver/cl_event_handler.hpp"

namespace cl
{
namespace serverdefs
{
enum
{
    WAIT_TIMEOUTMSEC_DEFAULT =4000
};
} // namespace serverdefs

typedef std::unordered_map < cf_fd, cf_uint32  > T_LISTENFD_MAXCONN;

template <typename EventHandlerType>
class Server : public cf::NonCopyable
{
public:

    Server()
    {
    }
    ~Server()
    {
    }

    cf_void Init(const T_LISTENFD_MAXCONN & listenfd_maxconn,
                 EventHandlerType & handler,
                 cf_int processid,
                 cf_int timeoutMilliseconds)
    {
        CF_PRINT_FUNC;
        CF_NEWOBJ(p, cf::EventLoop< EventHandlerType > ,
                  listenfd_maxconn, handler, processid);
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        _eventloop.reset(p);
        _timeoutMilliseconds =timeoutMilliseconds;
    }
    cf_void Final(cf_fd listenfd)
    {
    }
    cf_void Start()
    {
        CF_PRINT_FUNC;
        _eventloop->WaitEvent(_timeoutMilliseconds);
    }
    cf_void Stop()
    {
        CF_PRINT_FUNC;
        _eventloop->Stop();
    }
private:
    std::shared_ptr < cf::EventLoop < EventHandlerType > > _eventloop;
    cf_int _timeoutMilliseconds;
};

template <typename EventHandlerType>
class TcpServer : public cf::NonCopyable
{
public:
    static cf_fd CreateListenSocket(cf_uint32 port,bool reuseAddr =false,
                                    const int backlog =4096)
    {
        CF_PRINT_FUNC;
        cf_fd listenfd =cf::CreateServerSocket(port,SOCK_STREAM,reuseAddr,false,
                                               backlog);
#if CF_SWITCH_PRINT
        fprintf (stderr, "CreateServerSocket return , listenfd=%d \n",listenfd);
#endif
        return listenfd;
    }

    TcpServer(const T_LISTENFD_MAXCONN & listenfd_maxconn,
              EventHandlerType & handler,
              cf_int processid,
              cf_int timeoutMilliseconds =serverdefs::WAIT_TIMEOUTMSEC_DEFAULT)
        :_listenfd_maxconn(listenfd_maxconn)
    {
#if CF_SWITCH_PRINT
        printf("TcpServer::TcpServer \n");
#endif
        CF_PRINT_FUNC;
        CF_NEWOBJ(p, Server < EventHandlerType > );
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        _server.reset(p);

        _server->Init(_listenfd_maxconn, handler, processid,
                      timeoutMilliseconds);
    }
    ~TcpServer()
    {
        for (T_LISTENFD_MAXCONN::iterator it=
                 _listenfd_maxconn.begin();
             it!=_listenfd_maxconn.end(); it++)
            close(it->first);
    }
    cf_void Start()
    {
        _server->Start();
    }
    cf_void Stop()
    {
        _server->Stop();
    }
private:
    std::shared_ptr < Server < EventHandlerType > > _server;
    T_LISTENFD_MAXCONN _listenfd_maxconn;
};


} // namespace cl

#endif // _HEADER_FILE_CFD_CL_SERVER_HPP_

