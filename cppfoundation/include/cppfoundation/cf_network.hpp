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

#ifndef _HEADER_FILE_CFD_CF_NETWORK_HPP_
#define _HEADER_FILE_CFD_CF_NETWORK_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_socket.hpp"

namespace cf
{

namespace networkdefs
{
enum
{
    SEND_FDS_SUMMAX_SMALL = 16,
    SEND_FDS_SUMMAX_LARGE = 128,
};

enum EV_TYPE
{
    CFEV_ACCEPT = 01,
    CFEV_READ = 02,
    CFEV_WRITE = 04,
    CFEV_CLOSE = 010,
    CFEV_ERROR = 020
};
} // namespace networkdefs

inline int IPString2Int(cf_cpstr ip,cf_uint32 & rsl)
{
    struct in_addr s;      // IPv4地址结构体
    int rt =inet_pton(AF_INET, ip, (void *)&s);
    rsl =cf_uint32(s.s_addr);
    return rt;
}

inline std::string IPInt2String(cf_uint32 ip)
{
    char rsl[20] = {0};
    inet_ntop(AF_INET, (void *)&ip, rsl, sizeof(rsl));
    return rsl;
}



cf_int GetHostByName(cf_const std::string &, struct hostent * phe);

cf_void SetBlocking(cf_fd sockfd,bool blocking);

// Create socket and bind it to the port.
// return created socket, >0 successful, or throw exception .
cf_fd CreateServerSocket(const cf_int port,const cf_int socktype =SOCK_STREAM,
                         bool reuseAddr =false,bool blocking=false,
                         const cf_int backlog =32);

// Create socket and bind it to the port.
// return created socket, >0 successful, <0 Could not bind, or throw exception .
cf_fd CreateLocalServerSocket(const std::string & path,
                              const cf_int socktype =SOCK_STREAM,const cf_int backlog =32);

// Just test socket fd is broken approximately!
// Generally,return true means fd is broken , else means the socket connection maybe ok.
bool IsSocketBroken(cf_fd fd);


cf_void ConnectToServer(cf_cpstr ip,cf_uint32 port,cf_uint32 sum,
                        std::vector<cf_fd> & clientfds);

/**
 Waitting for io being available. Can only be used in socket fd!

 On success, a positive number is returned
 A value  of  0  indicates that  the call timed out and no file descriptors were ready.
 On error, throw exception.
**/
cf_int OutputWait(cf_fd sockfd,cf_int32 timeoutMilliSeconds);
cf_int InputWait(cf_fd sockfd,cf_int32 timeoutMilliSeconds);


/**
Send data synchronously in specified milliseconds. Can only be used in socket fd!
return true,send successfully.hasDone bytes have sent.
return false,timeout,hasDone bytes have sent.
**/
bool SendSegmentSync(cf_fd sockfd,cf_cpstr buf, ssize_t totalLen,
                     ssize_t & hasDone,cf_int32 timeoutMilliSeconds =-1,
                     ssize_t segsize =-1);

/**
Receive data synchronously in specified milliseconds. Can only be used in socket fd!
return true,recv successfully.hasDone bytes have received.
return false,timeout,hasDone bytes have received.
**/
bool RecvSegmentSync(cf_fd sockfd,cf_char * buf, ssize_t totalLen,
                     ssize_t & hasDone, bool & peerClosedWhenRead,
                     cf_int32 timeoutMilliSeconds =-1, ssize_t segsize =-1);

bool AcceptAsync(cf_fd listenfd, std::vector < T_SESSION > & clients);
cf_int SendSegmentAsync(cf_fd sockfd,cf_cpstr buf, ssize_t totalLen,
                        ssize_t segsize =-1);
cf_int RecvSegmentAsync(cf_fd sockfd,cf_char * buf, ssize_t totalLen,
                        bool & peerClosedWhenRead, ssize_t segsize =-1);

/**
Send/Receive fd to/from other process.
dataExtra is extra data to send.
**/

ssize_t SendFds(const cf_fd fd, const cf_int * sendfdarray,
                const size_t fdarraylen,
                void * dataExtra, const size_t dataExtraBytes);
ssize_t RecvFds(const cf_fd fd, cf_int * recvfdarray, const size_t fdarraylen,
                void * dataExtra, size_t dataExtraBytes);


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_NETWORK_HPP_

