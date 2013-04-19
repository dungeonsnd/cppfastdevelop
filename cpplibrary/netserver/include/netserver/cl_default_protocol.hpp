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

#ifndef _HEADER_FILE_CFD_CL_DEFAULT_PROTOCOL_HPP_
#define _HEADER_FILE_CFD_CL_DEFAULT_PROTOCOL_HPP_

#include "cppfoundation/cf_root.hpp"
#include "netserver/cl_event_loop.hpp"

namespace cl
{
namespace ns
{

namespace protocoldefs
{

enum
{
    DEFAULT_HEADLEN = 4
};
} // namespace protocoldefs

class DefaultProtocol : public cf::NonCopyable
{
public:
    DefaultProtocol():
        _headlen(protocoldefs::DEFAULT_HEADLEN),
        _bodylen(0)
    {
    }
    virtual ~DefaultProtocol()
    {
    }
    cf_uint32 HeadLen() const
    {
        return _headlen;
    }
    cf_void SetBodyLen(cf_uint32 bodylen)
    {
        _bodylen =bodylen;
    }
    cf_uint32 BodyLen() const
    {
        return _bodylen;
    }

    virtual cf_void OnAcceptProcess(Epoll & epoll,cf_int fd)
    {
        printf("OnAcceptProcess \n");
    }
    virtual cf_void OnReadProcess(Epoll & epoll,cf_int fd, cf_void * buff,
                                  cf_uint32 bytes)
    {
        printf("OnReadProcess \n");
        cf_char * b =static_cast < cf_char * > (buff);
        for(cf_uint32 i =0; i<bytes; i++)
        {
            if(b[i]>='a')
                b[i] -=32;
        }
        printf("---buff=%llu,buff=%s,b=%s---- \n",(cf_uint64)buff,(cf_char *)buff,
               (cf_char *)b);
        epoll.AsyncWrite(fd, b, bytes);
    }
    virtual cf_void OnWriteProcess(Epoll & epoll,cf_int fd, cf_uint32 bytes)
    {
        printf("OnWriteProcess \n");
    }
    virtual cf_void OnCloseProcess(cf_int fd)
    {
        printf("OnCloseProcess \n");
    }
    virtual cf_void OnTimeoutProcess(Epoll & epoll,cf_int fd)
    {
        printf("OnTimeoutProcess \n");
    }
    virtual cf_void OnErrorProcess(cf_int fd)
    {
        printf("OnErrorProcess \n");
    }
private:
    cf_uint32 _headlen;
    cf_uint32 _bodylen;
};


} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_DEFAULT_PROTOCOL_HPP_
