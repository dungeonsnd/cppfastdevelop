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

class Server : public cf::NonCopyable
{
public:
    Server()
    {
    }
    ~Server()
    {
    }
    cf_void Init(cf_fd listenfd)
    {
        CF_NEWOBJ(p, cf::EventLoop < EventHandler > , listenfd );
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        _eventloop.reset(p);
        _timeoutMilliseconds =8000;
    }
    cf_void Final(cf_fd listenfd)
    {
    }
    cf_void Start()
    {
        _eventloop->WaitEvent(_timeoutMilliseconds);
    }
    cf_void Stop()
    {
        _eventloop->Stop();
    }
private:
    std::shared_ptr < cf::EventLoop < EventHandler > > _eventloop;
    cf_int _timeoutMilliseconds;
};

class TcpServer : public cf::NonCopyable
{
public:
    TcpServer(cf_uint32 port,const int backlog =32)
    {
        _listenfd =cf::CreateServerSocket(port,SOCK_STREAM,false,backlog);
        if(_listenfd<0)
        {
        }
#if CFD_SWITCH_PRINT
        fprintf (stderr, "CreateServerSocket return , _listenfd=%d \n",_listenfd);
#endif

        CF_NEWOBJ(p, Server );
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        _server.reset(p);

        _server->Init(_listenfd);
    }
    ~TcpServer()
    {
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
    std::shared_ptr < Server > _server;
    cf_fd _listenfd;
};


} // namespace cl

#endif // _HEADER_FILE_CFD_CL_SERVER_HPP_

