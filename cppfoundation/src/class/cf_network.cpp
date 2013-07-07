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

#include "cppfoundation/cf_network.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"

namespace cf
{

cf_int GetHostByName(cf_const std::string & name, struct hostent * phe)
{
    static PthreadMutex mutex;
    LockGuard<PthreadMutex> lock(mutex);
    struct hostent * _phe = cf_gethostbyname(name.c_str());
    if (_phe == NULL)
        return -1;
    *phe = *_phe;
    return 0;
}

int CreateServerSocket (const int port,const int socktype,const int backlog)
{
    struct addrinfo hints;
    struct addrinfo * result, *rp;
    memset (&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;/* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;/* All interfaces */

    std::string portstr(8,'\0');
    snprintf(&portstr[0],portstr.size(),"%d",port);
    int rt = cf_getaddrinfo (NULL, portstr.c_str(), &hints, &result);
    if (rt != 0)
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_getaddrinfo ! %s.",
                   gai_strerror (rt));

    int listenfd;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        listenfd = cf_socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listenfd == -1)
            continue;

        rt = cf_bind (listenfd, rp->ai_addr, rp->ai_addrlen);
        if (rt == 0)
            break;/* We managed to bind successfully! */
        cf_close (listenfd);
    }

    if (rp == NULL)
    {
        //fprintf (stderr, "Could not bind\n");
        cf_freeaddrinfo (result);
        return -1;
    }
    cf_freeaddrinfo (result);
    if ( 0!=cf_listen(listenfd, backlog) )
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_listen !");
    return listenfd;
}

int CreateLocalServerSocket(const std::string & path,const int socktype,
                            const int backlog)
{
    int listenfd = cf_socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listenfd != 0)
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_socket !");
    if ( 0!=cf_unlink(path.c_str()) )
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_unlink !");
    struct sockaddr_un servaddr;
    cf_memset(&servaddr,0,sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    cf_strcpy(servaddr.sun_path, path.c_str());
    if ( 0!=cf_bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) )
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_unlink !");
    if ( 0!=cf_listen(listenfd, backlog) )
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_listen !");
    return listenfd;
}

cf_void SetBlocking(cf_int sockfd,bool blocking)
{
    cf_int flags = cf_fcntl(sockfd, F_GETFL, 0);
    if (-1==flags)
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_fcntl !")
        cf_int rt =0;
    if ( (flags&O_NONBLOCK) && blocking )
        rt = ::fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
    else if ( !(flags&O_NONBLOCK) && false==blocking )
        rt = ::fcntl(sockfd, F_SETFL, flags|O_NONBLOCK);
    else
        ; // Already set.
    if (-1==rt)
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_fcntl !");
}

bool IsSocketBroken(cf_int fd)
{
    std::string buf(8, 0);
    cf_int flags = cf_fcntl(fd, F_GETFL, 0);
    cf_int rt = cf_fcntl(fd, F_SETFL, flags|O_NONBLOCK);
    cf_int rc = ::recv(fd, &buf[0], buf.size(), MSG_PEEK);
    rt = cf_fcntl(fd, F_SETFL, flags);
    bool broken =false;
    if (rc == 0)
        broken =true;
    else if (rc == -1 && (errno == EINTR || errno == EWOULDBLOCK
                          || errno == EAGAIN))
        broken =false;
    else if (rc == -1)
        broken =true;
    else
        broken =false;
    return broken;
}


cf_int IOWaitting(struct pollfd & pfd,cf_int32 timeoutMilliSeconds)
{
    cf_int rt =0;
    if(timeoutMilliSeconds < 0)
        timeoutMilliSeconds =-1;
    rt=cf_poll(&pfd, 1, timeoutMilliSeconds);
    if (-1==rt)
        _THROW(SyscallExecuteError, "Failed to execute cf_poll !");
    return rt;
}

cf_int OutputWait(cf_int sockfd,cf_int32 timeoutMilliSeconds)
{
    struct pollfd pfd = {sockfd,POLLOUT,0};
    return IOWaitting(pfd,timeoutMilliSeconds);
}
cf_int InputWait(cf_int sockfd,cf_int32 timeoutMilliSeconds)
{
    struct pollfd pfd = {sockfd,POLLIN,0};
    return IOWaitting(pfd,timeoutMilliSeconds);
}

/**
return true,send successfully.hasDone bytes have sent.
return false,timeout,hasDone bytes have sent.
**/
bool SendSegmentSync(cf_int sockfd,cf_cpstr buf, ssize_t totalLen,
                     ssize_t & hasDone,cf_int32 timeoutMilliSeconds,ssize_t segsize)
{
    if(totalLen == 0)
        return true;
    ssize_t len=0;
    ssize_t lenLeft =0;
    time_t end,begin;
    for(ssize_t alreadyDone=0; alreadyDone < totalLen;)
    {
        if ( (time_t)(-1)==(begin=cf_time(NULL)) )
            _THROW(SyscallExecuteError, "Failed to execute cf_time !");

        if(0==OutputWait(sockfd,timeoutMilliSeconds)) // timeout!
            return false;
        lenLeft = totalLen-alreadyDone;
        if (segsize>0 && lenLeft > segsize)
            lenLeft = segsize;
        len =cf_write(sockfd,&buf[alreadyDone],lenLeft);
        if(-1==len)
            _THROW(SyscallExecuteError, "Failed to execute cf_write !");

        alreadyDone +=len;
        hasDone =alreadyDone;

        if ( (time_t)(-1)==(end=cf_time(NULL)) )
            _THROW(SyscallExecuteError, "Failed to execute cf_time !");
        timeoutMilliSeconds -=(end-begin);
        if (timeoutMilliSeconds <= 0)
            return false;
    }
    return true;
}

bool RecvSegmentSync(cf_int sockfd,cf_char * buf, ssize_t totalLen,
                     ssize_t & hasDone,cf_int32 timeoutMilliSeconds,ssize_t segsize)
{
    if(totalLen == 0)
        return true;
    ssize_t len=0;
    ssize_t lenLeft =0;
    time_t end,begin;
    for(ssize_t alreadyDone=0; alreadyDone < totalLen;)
    {
        if ( (time_t)(-1)==(begin=cf_time(NULL)) )
            _THROW(SyscallExecuteError, "Failed to execute cf_time !");

        if(0==InputWait(sockfd,timeoutMilliSeconds)) // timeout!
            return false;
        lenLeft = totalLen-alreadyDone;
        if (segsize>0 && lenLeft > segsize)
            lenLeft = segsize;
        len =cf_read(sockfd,&buf[alreadyDone],lenLeft);
        if(-1==len)
            _THROW(SyscallExecuteError, "Failed to execute cf_write !");

        alreadyDone +=len;
        hasDone =alreadyDone;

        if ( 0==len ) // len==0 means reaching end.
            return true;

        if ( (time_t)(-1)==(end=cf_time(NULL)) )
            _THROW(SyscallExecuteError, "Failed to execute cf_time !");
        timeoutMilliSeconds -=(end-begin);
        if (timeoutMilliSeconds <= 0)
            return false;
    }
    return true;
}


// Send fd over unix domain socket.
namespace networkdefs
{
union control_un_single
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(int)*1) ];
} ;
const size_t sizeof_control_un_single =sizeof(control_un_single);

union control_un_tiny
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(int)*SEND_FDS_SUMMAX_TINY) ];
} ;
const size_t sizeof_control_un_tiny =sizeof(control_un_single);

union control_un_small
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(int)*SEND_FDS_SUMMAX_SMALL) ];
} ;
const size_t sizeof_control_un_small =sizeof(control_un_single);

union control_un_medium
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(int)*SEND_FDS_SUMMAX_MEDIUM) ];
} ;
const size_t sizeof_control_un_medium =sizeof(control_un_single);

union control_un_large
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(int)*SEND_FDS_SUMMAX_LARGE) ];
} ;
const size_t sizeof_control_un_large =sizeof(control_un_single);

union control_un_verylarge
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(int)*SEND_FDS_SUMMAX_VERYLARGE) ];
} ;
const size_t sizeof_control_un_verylarge =sizeof(control_un_single);

} // namespace networkdefs

ssize_t SendFds(const int fd, const int * sendfdarray,const size_t fdarraylen,
                void * dataExtra, const size_t dataExtraBytes)
{
    std::string control_un(0,'\0');
    if(fdarraylen==1)
        control_un.resize(networkdefs::sizeof_control_un_single);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_TINY)
        control_un.resize(networkdefs::sizeof_control_un_tiny);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_SMALL)
        control_un.resize(networkdefs::sizeof_control_un_small);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_MEDIUM)
        control_un.resize(networkdefs::sizeof_control_un_medium);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_LARGE)
        control_un.resize(networkdefs::sizeof_control_un_large);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_VERYLARGE)
        control_un.resize(networkdefs::sizeof_control_un_verylarge);
    else
        _THROW_FMT(ValueError, "fdarraylen{%llu} is too large !",(cf_uint64)fdarraylen);

    struct msghdr   msg;
    msg.msg_control = (void *)(control_un[0]);
    msg.msg_controllen = control_un.size();

    struct cmsghdr * cmptr;
    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len  =CMSG_LEN( sizeof(int)*fdarraylen );
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    for(size_t i=0; i<fdarraylen; i++)
        *( (int *) (CMSG_DATA(cmptr)+sizeof(int)*i) ) = sendfdarray[i];

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    struct iovec    iov[1];
    iov[0].iov_base = dataExtra;
    iov[0].iov_len = dataExtraBytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    return (sendmsg(fd, &msg, 0));
}

ssize_t RecvFds(const int fd, int * recvfdarray, const size_t fdarraylen,
                void * dataExtra, size_t dataExtraBytes)
{
    std::string control_un(0,'\0');
    if(fdarraylen==1)
        control_un.resize(networkdefs::sizeof_control_un_single);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_TINY)
        control_un.resize(networkdefs::sizeof_control_un_tiny);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_SMALL)
        control_un.resize(networkdefs::sizeof_control_un_small);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_MEDIUM)
        control_un.resize(networkdefs::sizeof_control_un_medium);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_LARGE)
        control_un.resize(networkdefs::sizeof_control_un_large);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_VERYLARGE)
        control_un.resize(networkdefs::sizeof_control_un_verylarge);
    else
        _THROW_FMT(ValueError, "fdarraylen{%llu} is too large !",(cf_uint64)fdarraylen);
    struct msghdr   msg;
    msg.msg_control = (void *)(control_un[0]);
    msg.msg_controllen = control_un.size();

    struct cmsghdr * cmptr;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    struct iovec    iov[1];
    iov[0].iov_base = dataExtra;
    iov[0].iov_len = dataExtraBytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    ssize_t n =recvmsg(fd, &msg, 0);
    if (n<=0)
        return(n);

    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL
         &&  cmptr->cmsg_len == CMSG_LEN(sizeof(int)*fdarraylen)
       )
    {
        if (cmptr->cmsg_level != SOL_SOCKET)
        {
            //printf("control level != SOL_SOCKET");
            return -1;
        }
        if (cmptr->cmsg_type != SCM_RIGHTS)
        {
            //err_quit("control type != SCM_RIGHTS");
            return -2;
        }
        for(size_t i=0; i<fdarraylen; i++)
            recvfdarray[i] =*( (int *) (CMSG_DATA(cmptr)+sizeof(int)*i) );

    }
    else
        recvfdarray[0] = -1;        // descriptor was not passed

    return n;
}


} // namespace cf
