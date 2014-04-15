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
#include "cppfoundation/cf_utility.hpp"

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

cf_fd CreateServerSocket (const cf_int port,const cf_int socktype,
                          bool reuseAddr,bool blocking,
                          const cf_int backlog)
{
    cf_fd listenfd = cf_socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_socket !");

    int option_value =1;
    if( reuseAddr &&
        -1==cf_setsockopt(listenfd,SOL_SOCKET,
                          SO_REUSEADDR,&option_value,sizeof(int)) )
    {
        cf_close (listenfd);
        _THROW(cf::SyscallExecuteError,
               "Failed to execute cf_setsockopt (SO_REUSEADDR)! ");
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (port);

    if (-1 == cf_bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
    {
        cf_close (listenfd);
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_bind !");
    }

    if(listenfd>0&&false==blocking)
        SetBlocking(listenfd,blocking);

    if (-1 == cf_listen(listenfd, backlog) )
    {
        cf_close (listenfd);
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_listen !");
    }
    return listenfd;
}

cf_int CreateLocalServerSocket(const std::string & path,const cf_int socktype,
                               const cf_int backlog)
{
    cf_int listenfd = cf_socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listenfd != 0)
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_socket !");
    if ( 0!=cf_unlink(path.c_str()) )
        _THROW_FMT(cf::SyscallExecuteError, "Failed to execute cf_unlink !");
    struct sockaddr_un servaddr;
    cf_memset(&servaddr,0,sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    cf_strcpy(servaddr.sun_path, path.c_str());
    if ( 0!=cf_bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) )
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_unlink !");
    if ( 0!=cf_listen(listenfd, backlog) )
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_listen !");
    return listenfd;
}


bool IsSocketBroken(cf_fd fd)
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

cf_void ConnectToServer(cf_cpstr ip,cf_uint32 port,cf_uint32 sum,
                        std::vector<cf_fd> & clientfds)
{
    for(cf_uint32 i=0; i<sum; i++)
    {
        cf_fd sockfd;
        struct sockaddr_in servaddr;
        sockfd=cf_socket(AF_INET, SOCK_STREAM, 0);
        if(-1==sockfd)
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_socket !");
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_port=htons(port);
        cf_inet_pton(AF_INET, ip, &servaddr.sin_addr);
        cf_int rt =cf_connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (-1==rt)
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_connect !");
        clientfds.push_back(sockfd);
    }
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

cf_int OutputWait(cf_fd sockfd,cf_int32 timeoutMilliSeconds)
{
    struct pollfd pfd = {sockfd,POLLOUT,0};
    return IOWaitting(pfd,timeoutMilliSeconds);
}
cf_int InputWait(cf_fd sockfd,cf_int32 timeoutMilliSeconds)
{
    struct pollfd pfd = {sockfd,POLLIN,0};
    return IOWaitting(pfd,timeoutMilliSeconds);
}

/**
return true,send successfully.hasDone bytes have sent.
return false,timeout,hasDone bytes have sent.
**/
bool SendSegmentSync(cf_fd sockfd,cf_cpstr buf, ssize_t totalLen,
                     ssize_t & hasDone,cf_int32 timeoutMilliSeconds,ssize_t segsize)
{
    if(totalLen == 0)
        return true;
    ssize_t len=0;
    ssize_t lenLeft =0;
    cf_uint64 end,begin;
    for(hasDone=0; hasDone < totalLen;)
    {
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        begin =seconds*1000+useconds%1000;

        if(0==OutputWait(sockfd,timeoutMilliSeconds)) // timeout!
            return false;
        lenLeft = totalLen-hasDone;
        if (segsize>0 && lenLeft > segsize)
            lenLeft = segsize;
        len =cf_write(sockfd,&(buf[hasDone]),lenLeft);
        if(-1==len)
            _THROW(SyscallExecuteError, "Failed to execute cf_write !");

        hasDone +=len;
        //        printf("after cf_write , totalLen=%u,hasDone=%u,len=%u,lenLeft=%u \n",
        //            (cf_uint32)totalLen,(cf_uint32)hasDone,(cf_uint32)len,(cf_uint32)lenLeft);

        cf::Gettimeofday(seconds, useconds);
        end =seconds*1000+useconds%1000;

        timeoutMilliSeconds -=(end-begin);
        if (timeoutMilliSeconds <= 0)
            return false;
    }
    return true;
}

bool RecvSegmentSync(cf_fd sockfd,cf_char * buf, ssize_t totalLen,
                     ssize_t & hasDone, bool & peerClosedWhenRead,
                     cf_int32 timeoutMilliSeconds,ssize_t segsize)
{
    if(totalLen == 0)
        return true;
    peerClosedWhenRead =false;
    ssize_t len=0;
    ssize_t lenLeft =0;
    cf_uint64 end,begin;
    for(hasDone=0; hasDone < totalLen;)
    {
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        begin =seconds*1000+useconds%1000;

        if(0==InputWait(sockfd,timeoutMilliSeconds)) // timeout!
            return false;
        lenLeft = totalLen-hasDone;
        if (segsize>0 && lenLeft > segsize)
            lenLeft = segsize;
        len =cf_read(sockfd,&(buf[hasDone]),lenLeft);
        if(-1==len)
            _THROW(SyscallExecuteError, "Failed to execute cf_write !");

        hasDone +=len;
        //        printf("after cf_read , totalLen=%u,hasDone=%u,len=%u,lenLeft=%u \n",
        //            (cf_uint32)totalLen,(cf_uint32)hasDone,(cf_uint32)len,(cf_uint32)lenLeft);

        if ( 0==len ) // len==0 means reaching end.
        {
            peerClosedWhenRead =true;
            printf("after cf_read , 0==len \n");
            return true;
        }

        cf::Gettimeofday(seconds, useconds);
        end =seconds*1000+useconds%1000;

        timeoutMilliSeconds -=(end-begin);
        if (timeoutMilliSeconds <= 0)
            return false;
    }
    return true;
}

#define CF_NETWORK_ACCEPT_BREAK \
    ((errno == EAGAIN)||(errno == EWOULDBLOCK)||(errno==ECONNABORTED))
#define CF_NETWORK_NULLFD \
    ((errno == EMFILE)||(errno == ENFILE)||(errno == ENOBUFS)||(errno == ENOMEM))
bool AcceptAsync(cf_fd listenfd, std::vector < T_SESSION > & clients)
{
    static cf_fd nullfd =cf_open("/dev/null",O_RDONLY|O_CLOEXEC,0);
    //    if(nullfd<0) // Don't test for performance.
    //        _THROW(cf::SyscallExecuteError, "Failed to execute cf_open !");

    bool rt =false;
    while (true)
    {
        struct sockaddr_in in_addr;
        socklen_t in_len;
        in_len = sizeof in_addr;
        cf_int infd = cf_accept (listenfd, (sockaddr *)&in_addr, &in_len);
        if (infd == -1)
        {
            if (CF_NETWORK_ACCEPT_BREAK)
            {
#if 0
                fprintf (stderr, "CF_NETWORK_ACCEPT_BREAK ,errno=%d,%s, pid=%d \n\n",
                         errno,strerror(errno),cf_int(getpid()));
#endif
                break;
            }
            else if (CF_NETWORK_NULLFD)
            {
#if CF_SWITCH_PRINT
                fprintf (stderr, "Warning,accept return CF_NETWORK_NULLFD,"
                         "accept return -1 and errno=%d,%s!\n"
                         "going to close fd(%d),and close this client! pid=%d \n",
                         errno,strerror(errno),nullfd,cf_int(getpid()));
#endif

                cf_close(nullfd);
                nullfd =cf_accept(listenfd, NULL, NULL);
                if ( nullfd==-1&&CF_NETWORK_ACCEPT_BREAK )
                {
                    nullfd =open("/dev/null",O_RDONLY|O_CLOEXEC,0);
                    if(nullfd<0)
                        _THROW(cf::SyscallExecuteError, "Failed to execute cf_open !");
                    break;
                }
                if(nullfd<0)
                    _THROW(cf::SyscallExecuteError, "Failed to execute cf_accept !");

                cf_close(nullfd);
                nullfd =open("/dev/null",O_RDONLY|O_CLOEXEC,0);
                if(nullfd<0)
                    _THROW(cf::SyscallExecuteError, "Failed to execute cf_open !");
                continue;
            }
            else
            {
                _THROW(cf::SyscallExecuteError, "Failed to execute accept !");
                break;
            }
        }
        else
        {
            T_SESSION ses;
            CF_NEWOBJ(p, Socket , infd, in_addr.sin_addr, htons(in_addr.sin_port));
            if(NULL==p)
                _THROW(AllocateMemoryError, "Allocate memory failed !");
            ses.reset(p);

            clients.push_back(ses);
            rt =true;
        }
    }
    return rt;
}

cf_int SendSegmentAsync(cf_fd sockfd,cf_cpstr buf, ssize_t totalLen,
                        ssize_t segsize)
{
    if(totalLen == 0)
        return 0;
    ssize_t len=0;
    ssize_t lenLeft =0;
    ssize_t alreadyDone=0;
    while(alreadyDone < totalLen)
    {
        lenLeft = totalLen-alreadyDone;
        if (segsize>0 && lenLeft > segsize)
            lenLeft = segsize;
        len =cf_send(sockfd,&(buf[alreadyDone]),lenLeft,MSG_DONTWAIT);
        if(-1==len)
        {
            if(EAGAIN==errno||EWOULDBLOCK==errno)
                break;
            else
                _THROW(SyscallExecuteError, "Failed to execute cf_write !");

        }
        alreadyDone +=len;
        /*         fprintf (stderr,
                     "+++ cf_send,totalLen=%u,alreadyDone=%u,lenLeft=%u,len=%u\n",
                     (cf_uint32)totalLen,(cf_uint32)alreadyDone,
                     (cf_uint32)lenLeft,(cf_uint32)len); */
    }
    return cf_int(alreadyDone);
}

cf_int RecvSegmentAsync(cf_fd sockfd,cf_char * buf, ssize_t totalLen,
                        bool & peerClosedWhenRead, ssize_t segsize)
{
    if(totalLen == 0)
        return 0;
    peerClosedWhenRead =false;
    ssize_t len=0;
    ssize_t lenLeft =0;
    ssize_t alreadyDone=0;
    while(alreadyDone < totalLen)
    {
        lenLeft = totalLen-alreadyDone;
        if (segsize>0 && lenLeft > segsize)
            lenLeft = segsize;
        len =cf_recv(sockfd,&(buf[alreadyDone]),lenLeft,MSG_DONTWAIT);
        if(-1==len)
        {
            if(EAGAIN==errno||EWOULDBLOCK==errno)
                break;
            else
                _THROW(SyscallExecuteError, "Failed to execute cf_write !");

        }
        alreadyDone +=len;
        /*          fprintf (stderr,
                    "+++ cf_recv,totalLen=%u,alreadyDone=%u,lenLeft=%u,len=%u \n",
                   (cf_uint32)totalLen,(cf_uint32)alreadyDone,
                   (cf_uint32)lenLeft,(cf_uint32)len); */
        if ( 0==len ) // len==0 means reaching end.
        {
            peerClosedWhenRead =true;
            break;
        }
    }
    return cf_int(alreadyDone);
}


// Send fd over unix domain socket.
namespace networkdefs
{
union _tagcontrol_un_single
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(cf_int)*1) ];
} control_un_single;
const size_t sizeof_control_un_single =sizeof(control_un_single.control);

union _tagcontrol_un_small
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(cf_int)*SEND_FDS_SUMMAX_SMALL) ];
} control_un_small;
const size_t sizeof_control_un_small =sizeof(control_un_small.control);

union _tagcontrol_un_large
{
    struct cmsghdr cm;
    char control[ CMSG_SPACE(sizeof(cf_int)*SEND_FDS_SUMMAX_LARGE) ];
} control_un_large;
const size_t sizeof_control_un_large =sizeof(control_un_large.control);

} // namespace networkdefs

ssize_t SendFds(const cf_int fd, const cf_int * sendfdarray,
                const size_t fdarraylen,
                void * dataExtra, const size_t dataExtraBytes)
{
    std::string control_un(0,'\0');
    if(fdarraylen==1)
        control_un.resize(networkdefs::sizeof_control_un_single);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_SMALL)
        control_un.resize(networkdefs::sizeof_control_un_small);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_LARGE)
        control_un.resize(networkdefs::sizeof_control_un_large);
    else
        _THROW_FMT(ValueError, "fdarraylen{%llu} is too large !",(cf_uint64)fdarraylen);

    struct msghdr   msg;
    msg.msg_control = (void *)(&control_un[0]);
    msg.msg_controllen = control_un.size();

    struct cmsghdr * cmptr;
    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len  =CMSG_LEN( sizeof(cf_int)*fdarraylen );
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    for(size_t i=0; i<fdarraylen; i++)
        *( (cf_int *) (CMSG_DATA(cmptr)+sizeof(cf_int)*i) ) = sendfdarray[i];

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    struct iovec    iov[1];
    iov[0].iov_base = dataExtra;
    iov[0].iov_len = dataExtraBytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    return (sendmsg(fd, &msg, 0));
}

ssize_t RecvFds(const cf_int fd, cf_int * recvfdarray, const size_t fdarraylen,
                void * dataExtra, size_t dataExtraBytes)
{
    std::string control_un(0,'\0');
    if(fdarraylen==1)
        control_un.resize(networkdefs::sizeof_control_un_single);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_SMALL)
        control_un.resize(networkdefs::sizeof_control_un_small);
    else if(fdarraylen<=networkdefs::SEND_FDS_SUMMAX_LARGE)
        control_un.resize(networkdefs::sizeof_control_un_large);
    else
        _THROW_FMT(ValueError, "fdarraylen{%llu} is too large !",(cf_uint64)fdarraylen);
    struct msghdr   msg;
    msg.msg_control = (void *)(&control_un[0]);
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
         &&  cmptr->cmsg_len == CMSG_LEN(sizeof(cf_int)*fdarraylen)
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
            recvfdarray[i] =*( (cf_int *) (CMSG_DATA(cmptr)+sizeof(cf_int)*i) );

    }
    else
        recvfdarray[0] = -1;        // descriptor was not passed

    return n;
}


} // namespace cf
