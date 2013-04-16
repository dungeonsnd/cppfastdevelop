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

namespace cl
{
namespace ns
{

class IOComplete : public cf::NonCopyable
{
public:
    virtual ~IOComplete()
    {
    }
    virtual cf_void OnAcceptComplete(cf_int fd) =0;
    virtual cf_void OnReadComplete(cf_int fd, cf_void * buff, cf_uint32 bytes) =0;
    virtual cf_void OnWriteComplete(cf_int fd, cf_uint32 bytes) =0;
    virtual cf_void OnCloseComplete(cf_int fd) =0;
    virtual cf_void OnTimeoutComplete(cf_int fd) =0;
    virtual cf_void OnErrorComplete(cf_int fd) =0;
};

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
        }
        else
        {
            CF_NEWOBJ(pbuf, ChannelBuffer)
            if(NULL==pbuf)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            std::shared_ptr<ChannelBuffer> p(pbuf);
            p->SetReadTotal(bytes);
            _channel.insert( std::make_pair(fd,p) );
        }
        cf::AddEventEpoll(_epfd, _listenfd,_event,EPOLLIN|EPOLLONESHOT);
    }
    cf_void AsyncWrite(cf_int fd, cf_pvoid buff, cf_uint32 bytes)
    {
        IterTypeBuffer it =_channel.find(fd);
        if(it!=_channel.end())
        {
            std::shared_ptr<ChannelBuffer> p =it->second;
            p->SetWriteTotal(buff,bytes);
        }
        else
        {
            CF_NEWOBJ(pbuf, ChannelBuffer)
            if(NULL==pbuf)
                _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
            std::shared_ptr<ChannelBuffer> p(pbuf);
            p->SetWriteTotal(buff,bytes);
            _channel.insert( std::make_pair(fd,p) );
        }
        cf::AddEventEpoll(_epfd, _listenfd,_event,EPOLLOUT|EPOLLONESHOT);
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
        bool goon =true;
        while(channel->GetReadLeft()>0)
        {
            n =cf_recv( fd, channel->GetReadCurrentPtr(), size_t(channel->GetReadLeft()) ,0 );
            if(n>0)
            {
                channel->AppendReadCount(cf_uint32(n));
            }
            else if (0==n)
            {
                _iocomplete.OnReadComplete(fd, channel->GetReadBuffer(),channel->GetHasRead());
                Close(fd);
            }
            else
            {
                if (EAGAIN==errno || EWOULDBLOCK==errno)
                {
                    channel->AppendReadCount(cf_uint32(n));
                }
                else
                {
                    goon =false;
                    Error(fd);
                }
                break;
            }
        }
        if(channel->GetAsyncClose())
            Close(fd);
        else if(goon)
            cf::AddEventEpoll(_epfd, _listenfd,_event,EPOLLIN|EPOLLONESHOT);
        else
            ; // Noting todo.
    }
    cf_void Write(cf_int fd)
    {
        IterTypeBuffer it =_channel.find(fd);
        if(it==_channel.end())
            _THROW(cf::RuntimeWarning, "it==_channel.end() !")
        std::shared_ptr<ChannelBuffer> channel =it->second;

        ssize_t n =0;
        bool goon =true;
        while(channel->GetWriteLeft()>0)
        {
            n =cf_send( fd, channel->GetWriteCurrentPtr(), size_t(channel->GetWriteLeft()) ,0 );
            if(n>0)
            {
                channel->RemoveWriteCount(cf_uint32(n));
            }
            else
            {
                if (EAGAIN==errno || EWOULDBLOCK==errno)
                {
                    channel->RemoveWriteCount(cf_uint32(n));
                }
                else
                {
                    goon =false;
                    Error(fd);
                }
                break;
            }
        }
        if(channel->GetAsyncClose())
            Close(fd);
        else if(goon)
            cf::AddEventEpoll(_epfd, _listenfd,_event,EPOLLOUT|EPOLLONESHOT);
        else
            ; // Noting todo.
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
