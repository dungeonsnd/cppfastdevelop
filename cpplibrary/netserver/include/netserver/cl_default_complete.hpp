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

#ifndef _HEADER_FILE_CFD_CL_DEFAULT_COMPLETE_HPP_
#define _HEADER_FILE_CFD_CL_DEFAULT_COMPLETE_HPP_

#include "cppfoundation/cf_root.hpp"
#include "netserver/cl_event_loop.hpp"

namespace cl
{
namespace ns
{

template < typename ProtocolType >
class DefaultComplete : public IOComplete
{
public:
    DefaultComplete():
        _proto()
    {
    }
    ~DefaultComplete()
    {
    }
    void Initial(cl::ns::Epoll * epoll)
    {
        _epoll =epoll;
        if(NULL==_epoll)
            _THROW(cf::NullPointerError, "NULL==_epoll !")
        }
    cf_void OnAcceptComplete(cf_int fd)
    {
        printf("OnAcceptComplete \n");
        _epoll->AsyncRead(fd, _proto.HeadLen());
        _proto.OnAcceptProcess(*_epoll, fd);
    }
    cf_void OnReadComplete(cf_int fd, cf_void * buff, cf_uint32 bytes)
    {
        printf("OnReadComplete \n");
        if(_proto.HeadLen()==bytes)
        {
            cf_uint t =*((cf_uint *)buff);
            cf_uint bodylen =ntohl(t);
            printf("Read head finished , bodylen=%u \n",bodylen);
            _proto.SetBodyLen(bodylen);
            if(bodylen>10240)
            {
                _epoll->AsyncClose(fd);
            }
            else if (bodylen > 0)
            {
                _epoll->AsyncRead(fd,bodylen);
            }
            else
            {
                _epoll->AsyncRead(fd,_proto.HeadLen());
            }
        }
        else
        {
            printf("Read body finished .bytes=%u,buff=%s \n",bytes,(cf_char *)buff);
            _proto.OnReadProcess(*_epoll, fd, buff, bytes);
            _epoll->AsyncRead(fd,_proto.HeadLen());
        }
    }
    cf_void OnWriteComplete(cf_int fd, cf_uint32 bytes)
    {
        printf("OnWriteComplete \n");
        _proto.OnWriteProcess(*_epoll, fd, bytes);
    }
    cf_void OnCloseComplete(cf_int fd)
    {
        printf("OnCloseComplete \n");
        _proto.OnCloseProcess(fd);
    }
    cf_void OnTimeoutComplete(cf_int fd)
    {
        printf("OnTimeoutComplete \n");
        _proto.OnTimeoutProcess(*_epoll, fd);
    }
    cf_void OnErrorComplete(cf_int fd)
    {
        printf("OnErrorComplete \n");
        _proto.OnErrorProcess(fd);
    }
private:
    Epoll * _epoll;
    ProtocolType _proto;
};


} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_DEFAULT_COMPLETE_HPP_
