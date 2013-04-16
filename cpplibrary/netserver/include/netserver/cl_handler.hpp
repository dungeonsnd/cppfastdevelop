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

#ifndef _HEADER_FILE_CFD_CL_HANDLER_HPP_
#define _HEADER_FILE_CFD_CL_HANDLER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_io_utility.hpp"
#include "cppfoundation/cf_memory.hpp"
#include "netserver/cl_channel_buffer.hpp"
#include "netserver/cl_io_complete.hpp"

namespace cl
{
namespace ns
{

class Handler : public cf::NonCopyable
{
public:
    typedef std::map < cf_int , std::shared_ptr<ChannelBuffer> > MapTypeBuffer;
    typedef MapTypeBuffer::iterator IterTypeBuffer;
    
    Handler(cf_int listenfd,cf_int epfd, IOComplete & iocomplete):
        _listenfd(listenfd),
        _epfd(epfd),
        _iocomplete(iocomplete)
    {
    }
    virtual ~Handler()
    {
    }
    
    cf_void AsyncRead(cf_int fd, cf_uint32 bytes)
    {    
        IterTypeBuffer it =_channel.find(fd);
        if(it!=_channel.end())
        {
            std::shared_ptr<ChannelBuffer> p =it->second;
            p->SetReadTotal(bytes);
        printf("AsyncRead fd=%d,GetHasRead=%d,,GetReadTotal=%d,GetReadLeft=%d \n",fd,p->GetHasRead(),p->GetReadTotal(),p->GetReadLeft());
        }
        else
        {
            CF_NEWOBJ(pbuf, ChannelBuffer)
            if(NULL==pbuf)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            std::shared_ptr<ChannelBuffer> p(pbuf);
            p->SetReadTotal(bytes);
            _channel.insert( std::make_pair(fd,p) );
        printf("AsyncRead ,new, fd=%d,GetHasRead=%d,,GetReadTotal=%d,GetReadLeft=%d \n",fd,p->GetHasRead(),p->GetReadTotal(),p->GetReadLeft());
        }
        cf::AddEventEpoll(_epfd, fd,_event,EPOLLIN|EPOLLONESHOT);
    }
    cf_void AsyncWrite(cf_int fd, cf_pvoid buff, cf_uint32 bytes)
    {
        IterTypeBuffer it =_channel.find(fd);
        if(it!=_channel.end())
        {
            std::shared_ptr<ChannelBuffer> p =it->second;
            p->SetWrittenTotal(buff,bytes);
        printf("++4++ fd=%d,GetHasRead=%d,GetWrittenLeft=%d \n",fd,p->GetHasRead(),p->GetWrittenLeft());
        }
        else
        {
            CF_NEWOBJ(pbuf, ChannelBuffer)
            if(NULL==pbuf)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            std::shared_ptr<ChannelBuffer> p(pbuf);
            p->SetWrittenTotal(buff,bytes);
            _channel.insert( std::make_pair(fd,p) );
        printf("++4++ fd=%d,GetWrittenLeft=%d \n",fd,p->GetWrittenLeft());
        }
        cf::AddEventEpoll(_epfd, fd,_event,EPOLLOUT|EPOLLONESHOT);
    }
    cf_void AsyncClose(cf_int fd)
    {
        IterTypeBuffer it =_channel.find(fd);
        if(it!=_channel.end())
        {
            std::shared_ptr<ChannelBuffer> p =it->second;
            p->SetAsyncClose();
        }
        else
        {
            CF_NEWOBJ(pbuf, ChannelBuffer)
            if(NULL==pbuf)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            std::shared_ptr<ChannelBuffer> p(pbuf);
            p->SetAsyncClose();
            _channel.insert( std::make_pair(fd,p) );
        }
    }
    
    cf_void Accept(cf_int epfd, cf_int fd)
    {
        struct sockaddr in_addr;
        socklen_t in_len =sizeof(in_addr);
        cf_int clientfd =0;
        cf_int rt =0;
        cf_char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        while (1)
        {
            clientfd =cf_accept(fd, &in_addr, &in_len);
            if(-1 == clientfd)
            {
                if ( (errno == EAGAIN)||(errno == EWOULDBLOCK) )
                  break;
                else
                {
                    _THROW(cf::SyscallExecuteError, "Failed to execute cf_accept !")
                    break;
                }
            }
            rt = getnameinfo (&in_addr,in_len,hbuf, sizeof(hbuf),sbuf, sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);
            if (0==rt)
            {
              printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", clientfd, hbuf, sbuf);
            }
            else
                ; // getnameinfo failed. Log.
            
            cf::SetBlocking(clientfd,false);
            _iocomplete.OnAcceptComplete(clientfd);
        }
    }
    cf_void Read(cf_int fd)
    {
        IterTypeBuffer it =_channel.find(fd);
        if(it==_channel.end())
            _THROW(cf::RuntimeWarning, "it==_channel.end() !")
        std::shared_ptr<ChannelBuffer> channel =it->second;

        ssize_t n =0;
        bool readComplete =true;
        bool hasClosed =false;
        printf("Read fd=%d,GetReadTotal=%d,GetHasRead=%d,GetReadLeft=%d \n",fd,channel->GetReadTotal(),channel->GetHasRead(),channel->GetReadLeft());
        while(channel->GetReadLeft()>0 && channel->GetReadTotal()>0 )
        {
            n =cf_recv( fd, channel->GetReadCurrentPtr(), size_t(channel->GetReadLeft()) ,0 );
            if(n>0)
            {
                channel->AppendReadCount(cf_uint32(n));
            }
            else if (0==n)
            {
                if(channel->GetHasRead()>0)
                    _iocomplete.OnReadComplete(fd, channel->GetReadBuffer(),channel->GetHasRead());
                Close(fd);
                hasClosed =true;
                break;
            }
            else
            {
                if (EAGAIN==errno || EWOULDBLOCK==errno)
                {
                    channel->AppendReadCount(cf_uint32(n));
                }
                else
                {
                    readComplete =false;
                    Error(fd);
                    hasClosed =true;
                }
                break;
            }
        }

        if(false==hasClosed)
        {
            if(0==channel->GetReadLeft())
            {
                if(channel->GetHasRead()>0)
                    _iocomplete.OnReadComplete(fd, channel->GetReadBuffer(),channel->GetHasRead());
                readComplete =false;
            }
            
            if(channel->GetAsyncClose())
            {
                if(channel->GetHasRead()>0)
                    _iocomplete.OnReadComplete(fd, channel->GetReadBuffer(),channel->GetHasRead());
                Close(fd);
            }
            else if(readComplete)
            {
                printf("++1++ fd=%d,GetReadLeft=%d \n",fd,channel->GetReadLeft());
                cf::AddEventEpoll(_epfd, fd,_event,EPOLLIN|EPOLLONESHOT);
                printf("--1-- fd=%d,GetReadLeft=%d \n",fd,channel->GetReadLeft());
            }
            else
                ; // Noting todo.
        }
    }
    cf_void Write(cf_int fd)
    {
    printf("@@@@@@ Write \n");
        IterTypeBuffer it =_channel.find(fd);
        if(it==_channel.end())
            _THROW(cf::RuntimeWarning, "it==_channel.end() !")
        std::shared_ptr<ChannelBuffer> channel =it->second;

        ssize_t n =0;
        bool goon =true;
        bool hasClosed =false;
        while(channel->GetWrittenLeft()>0 && channel->GetWrittenTotal()>0)
        {
            n =cf_send( fd, channel->GetWrittenCurrentPtr(), size_t(channel->GetWrittenLeft()) ,0 );
            if(n>0)
            {
                channel->RemoveWrittenCount(cf_uint32(n));
            }
            else
            {
                if (EAGAIN==errno || EWOULDBLOCK==errno)
                {
                    channel->RemoveWrittenCount(cf_uint32(n));
                }
                else
                {
                    goon =false;
                    Error(fd);
                    hasClosed =true;
                }
                break;
            }
        }
        
        if(false==hasClosed)
        {
            if(0==channel->GetHasWritten())
            {
                if(channel->GetHasRead()>0)
                    _iocomplete.OnWriteComplete(fd, channel->GetHasWritten());
                goon =false;
            }
            
            if(channel->GetAsyncClose())
                Close(fd);
            else if(goon)
            {
                printf("++2++ fd=%d,GetWrittenLeft=%d \n",fd,channel->GetWrittenLeft());
                cf::AddEventEpoll(_epfd, fd,_event,EPOLLOUT|EPOLLONESHOT);
                printf("--2-- fd=%d,GetWrittenLeft=%d \n",fd,channel->GetWrittenLeft());
            }
            else
                ; // Noting todo.
        }
    }
    cf_void Timeout(cf_int fd)
    {
        _iocomplete.OnTimeoutComplete(fd);
    }
    cf_void Close(cf_int fd)
    {
        _iocomplete.OnCloseComplete(fd);
        CloseChannel(fd);
    }
    cf_void Error(cf_int fd)
    {
        _iocomplete.OnErrorComplete(fd);
        CloseChannel(fd);
    }

private:
    cf_void CloseChannel(cf_int fd)
    {
        int rt =cf_close(fd);
        if(0!=rt)
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_close !")
        if(1!=_channel.erase(fd))
            _THROW(cf::RuntimeWarning, "1!=_channel.erase(fd) !")
    }
    
    cf_int _listenfd;
    cf_int _epfd;
    struct epoll_event _event;
    MapTypeBuffer _channel;
    IOComplete & _iocomplete;
};

} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_HANDLER_HPP_
