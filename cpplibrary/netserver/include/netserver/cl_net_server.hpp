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

#ifndef _HEADER_FILE_CFD_CL_NET_SERVER_HPP_
#define _HEADER_FILE_CFD_CL_NET_SERVER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_io_utility.hpp"
#include "netserver/cl_event_loop.hpp"

namespace cl
{
namespace ns
{


template < typename CompleteType >
class NetServer : public cf::NonCopyable
{
public:
   NetServer(const int port,const int backlog =32):
      _epoll(NULL),
      _complete()
   {
      int listenfd =cf::CreateServerSocket(port,SOCK_STREAM,backlog);
      if(listenfd<0)
         _THROW(cf::SyscallExecuteError, "CreateServerSocket failed !")

         _epoll =new Epoll(listenfd,_complete);
      _complete.Initial(_epoll);
   }
   ~NetServer()
   {
   }
   cf_void Start()
   {
      _epoll->Wait(-1);
   }
private:
   Epoll * _epoll;
   CompleteType _complete;
};


} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_NET_SERVER_HPP_
