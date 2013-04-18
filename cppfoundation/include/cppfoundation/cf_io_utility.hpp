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

#ifndef _HEADER_FILE_CFD_CF_IO_UTILITY_HPP_
#define _HEADER_FILE_CFD_CF_IO_UTILITY_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

namespace ioutilitydefs
{
enum
{
   SEND_FDS_SUMMAX_TINY = 4,
   SEND_FDS_SUMMAX_SMALL = 16,
   SEND_FDS_SUMMAX_MEDIUM = 32,
   SEND_FDS_SUMMAX_LARGE = 64,
   SEND_FDS_SUMMAX_VERYLARGE = 128
};
} // namespace ioutilitydefs

// Create socket and bind it to the port.
// return created socket, >0 successful, <0 Could not bind, or throw exception .
int CreateServerSocket(const int port,const int socktype =SOCK_STREAM,
                       const int backlog =32);

// Create socket and bind it to the port.
// return created socket, >0 successful, <0 Could not bind, or throw exception .
int CreateLocalServerSocket(const std::string & path,
                            const int socktype =SOCK_STREAM,const int backlog =32);

cf_void SetBlocking(cf_int sockfd,bool blocking);

// Just test socket fd is broken approximately!
// Generally,return true means fd is broken , else means the socket connection maybe ok.
bool IsSocketBroken(cf_int fd);

/**
 Waitting for io being available. Can only be used in socket fd!

 On success, a positive number is returned
 A value  of  0  indicates that  the call timed out and no file descriptors were ready.
 On error, throw exception.
**/
cf_int OutputWait(cf_int sockfd,cf_int32 timeoutMilliSeconds);
cf_int InputWait(cf_int sockfd,cf_int32 timeoutMilliSeconds);


/**
Send data synchronously in specified milliseconds. Can only be used in socket fd!
return true,send successfully.hasDone bytes have sent.
return false,timeout,hasDone bytes have sent.
**/
bool SendSegmentSync(cf_int sockfd,cf_cpstr buf, ssize_t totalLen,
                     ssize_t & hasDone,cf_int32 timeoutMilliSeconds =-1,
                     ssize_t segsize =-1);

/**
Receive data synchronously in specified milliseconds. Can only be used in socket fd!
return true,recv successfully.hasDone bytes have received.
return false,timeout,hasDone bytes have received.
**/
bool RecvSegmentSync(cf_int sockfd,cf_char * buf, ssize_t totalLen,
                     ssize_t & hasDone,cf_int32 timeoutMilliSeconds =-1,
                     ssize_t segsize =-1);

/**
Send/Receive fd to/from other process.
dataExtra is extra data to send.
**/

ssize_t SendFds(const int fd, const int * sendfdarray,const size_t fdarraylen,
                void * dataExtra, const size_t dataExtraBytes);
ssize_t RecvFds(const int fd, int * recvfdarray, const size_t fdarraylen,
                void * dataExtra, size_t dataExtraBytes);

#ifdef __linux__
void AddEventEpoll(cf_int epfd, cf_int fd,struct epoll_event & event,
                   cf_uint32 ev);
void AddEventEpoll(cf_int epfd, cf_int fd, cf_uint32 ev);
void DelEventEpoll(cf_int epfd, cf_int fd,struct epoll_event & event);
void DelEventEpoll(cf_int epfd, cf_int fd);
#endif // __linux__

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_IO_UTILITY_HPP_

