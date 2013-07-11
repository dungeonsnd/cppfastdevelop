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

#ifndef _HEADER_FILE_CFD_CL_BUFFER_HPP_
#define _HEADER_FILE_CFD_CL_BUFFER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_socket.hpp"

namespace cl
{

class ReadBuffer : public cf::NonCopyable
{
public:
    ReadBuffer()
        :_buf(0,'\0'),_already(0)
    {
    }
    ~ReadBuffer()
    {
    }

    cf_void Clear()
    {
        _buf.clear();
        _already =0;
    }

    cf_int Read(cf::T_SESSION session, bool & peerClosedWhenRead)
    {
        cf_char * p =&_buf[0];
        cf_int rdn =session->RecvAsync(p+_already, GetLeft(),peerClosedWhenRead);
        _already +=rdn;

#if CFD_SWITCH_PRINT
        fprintf (stderr, "Read,fd=%d,addr=%s,rdn=%d,_total=%u,_already=%u,buf=%s \n",
                 session->Fd(),session->Addr().c_str(),rdn,(cf_uint32)(_buf.size()),_already,p);
#endif
        return rdn;
    }
    cf_pvoid GetBuffer()
    {
        return &_buf[0];
    }
    cf_void SetTotal(cf_uint32 total)
    {
        if(_buf.size())
            _THROW_FMT(cf::ValueError, "_buf.size(){%u}!=0 !", (cf_uint32)(_buf.size()));
        _buf.resize(total);
    }
    cf_uint32 GetTotal() cf_const
    {
        return (cf_uint32)(_buf.size());
    }
    cf_uint32 GetLeft() cf_const
    {
        if((cf_uint32)(_buf.size())>_already)
            return _buf.size()-_already;
        else
            _THROW_FMT(cf::ValueError, "_buf.size(){%u}<=_already{%u} !", (cf_uint32)(_buf.size()),_already);
    }
    bool IsComplete() cf_const
    {
        if(0==_buf.size())
            _THROW(cf::ValueError, "0==_buf.size() !");
        return (cf_uint32)(_buf.size())==_already;
    }
private:
    std::string _buf;
    cf_uint32 _already;
};

class WriteBuffer : public cf::NonCopyable
{
public:
    WriteBuffer()
        :_buf(0,'\0'),_already(0)
    {
    }
    ~WriteBuffer()
    {
    }

    cf_void Clear()
    {
        _buf.clear();
        _already =0;
    }

    cf_int Write(cf::T_SESSION session)
    {
        cf_char * p =&_buf[0];
        cf_int rdn =session->SendAsync(p+_already, GetLeft());
        _already +=rdn;

#if CFD_SWITCH_PRINT
        fprintf (stderr,
                 "Read,fd=%d,addr=%s,rdn=%d,_buf.size()=%u,_already=%u,buf=%s \n",
                 session->Fd(),session->Addr().c_str(),rdn,(cf_uint32)(_buf.size()),_already,p);
#endif
        return rdn;
    }
    cf_void SetBuffer(cf_cpvoid buffer, cf_uint32 total)
    {
        if(_buf.size())
            _THROW_FMT(cf::ValueError, "_buf.size(){%u}!=0 !", (cf_uint32)(_buf.size()));
        _buf.resize(total);
        memcpy(&_buf[0],buffer,total);
    }
    cf_uint32 GetTotal() cf_const
    {
        return (cf_uint32)(_buf.size());
    }
    cf_uint32 GetLeft() cf_const
    {
        if((cf_uint32)(_buf.size())>_already)
            return (cf_uint32)(_buf.size())-_already;
        else
            _THROW_FMT(cf::ValueError, "_buf.size(){%u}<=_already{%u} !", (cf_uint32)(_buf.size()),_already);
    }
    bool IsComplete() cf_const
    {
        if(0==_buf.size())
            _THROW(cf::ValueError, "0==_buf.size() !");
        return (cf_uint32)(_buf.size())==_already;
    }
private:
    std::string _buf;
    cf_uint32 _already;
private:
};


} // namespace cl

#endif // _HEADER_FILE_CFD_CL_BUFFER_HPP_

