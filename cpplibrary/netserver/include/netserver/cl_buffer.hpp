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
#include "cppfoundation/cf_memory.hpp"

namespace cl
{

namespace bufferdefs
{
enum
{
    SIZE_BUFFER_DEFAULT =4096,
    SIZE_READBUFFER_DEFAULT =1024,
    SIZE_WRITEBUFFER_DEFAULT =1024
};
} // namespace bufferdefs


class ReadBuffer : public cf::NonCopyable
{
public:
    ReadBuffer()
        :_buf(bufferdefs::SIZE_BUFFER_DEFAULT,'\0'),_total(0),_already(0)
    {
    }
    ~ReadBuffer()
    {
    }

    cf_void Clear()
    {
        _buf.clear();
        _total =0;
        _already =0;
    }

    cf_int Read(cf::T_SESSION session, bool & peerClosedWhenRead)
    {
        cf_char * p =&_buf[0];
        cf_int rdn =session->RecvAsync(p+_already, GetLeft(),peerClosedWhenRead);
        _already +=rdn;

#if CFD_SWITCH_PRINT
        //        fprintf (stderr, "Read,fd=%d,addr=%s,rdn=%d,_total=%u,_already=%u,buf=%s \n",
        //                 session->Fd(),session->Addr().c_str(),rdn,_total,_already,p);
#endif
        return rdn;
    }
    cf_pvoid GetBuffer()
    {
        return &_buf[0];
    }
    cf_void SetTotal(cf_uint32 total)
    {
        if(_total)
            _THROW_FMT(cf::ValueError, "_total{%u}!=0 !", _total);
        _buf.resize(total);
        _total =total;
    }
    cf_uint32 GetTotal() cf_const
    {
        return _total;
    }
    cf_uint32 GetLeft() cf_const
    {
        if(_total>_already)
            return _total-_already;
        else
            _THROW_FMT(cf::ValueError, "_total{%u}<=_already{%u} !", _total,_already);
    }
    bool IsComplete() cf_const
    {
        if(0==_total)
            _THROW(cf::ValueError, "0==_total !");
        return _total==_already;
    }
private:
    std::string _buf;
    cf_uint32 _total;
    cf_uint32 _already;
};

class WriteBuffer : public cf::NonCopyable
{
public:
    WriteBuffer()
        :_buf(bufferdefs::SIZE_BUFFER_DEFAULT,'\0'),_total(0),_already(0)
    {
    }
    ~WriteBuffer()
    {
    }

    cf_void Clear()
    {
        _buf.clear();
        _total =0;
        _already =0;
    }

    cf_int Write(cf::T_SESSION session)
    {
        cf_char * p =&_buf[0];
        cf_int rdn =session->SendAsync(p+_already, GetLeft());
        _already +=rdn;

#if CFD_SWITCH_PRINT
        //        fprintf (stderr,"Write,fd=%d,addr=%s,rdn=%d,_total=%u,_already=%u,buf=%s \n",
        //                 session->Fd(),session->Addr().c_str(),rdn,_total,_already,p);
#endif
        return rdn;
    }
    cf_void SetBuffer(cf_cpvoid buffer, cf_uint32 total)
    {
        if(_total)
            _THROW_FMT(cf::ValueError, "_total{%u}!=0 !", _total);
        _buf.resize(total);
        memcpy(&_buf[0],buffer,total);
        _total =total;
    }
    cf_uint32 GetTotal() cf_const
    {
        return _total;
    }
    cf_uint32 GetLeft() cf_const
    {
        if(_total>_already)
            return _total-_already;
        else
            _THROW_FMT(cf::ValueError, "_total{%u}<=_already{%u} !", _total,_already);
    }
    bool IsComplete() cf_const
    {
        if(0==_buf.size())
            _THROW(cf::ValueError, "0==_total !");
        return _total==_already;
    }
private:
    std::string _buf;
    cf_uint32 _total;
    cf_uint32 _already;
private:
};


class ReadBufferPool : public cf::NonCopyable
{
public:
    ReadBufferPool(cf_int poolsize)
    {
        if(poolsize<1)
            _THROW_FMT(cf::ValueError, "poolsize{%u}!=0 !", poolsize);
        for(cf_int i=0; i<poolsize; i++)
        {
            _free.push_back(AllocOne());
        }
    }
    ~ReadBufferPool()
    {
    }

    static std::shared_ptr<ReadBuffer> AllocOne()
    {
        std::shared_ptr < ReadBuffer > rb;
        CF_NEWOBJ(p, ReadBuffer);
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        rb.reset(p);
        return rb;
    }

    std::shared_ptr < ReadBuffer > GetFromPool()
    {
        if(_free.empty())
        {
            std::shared_ptr<ReadBuffer> rb =AllocOne();
            return rb;
        }
        else
        {
            std::shared_ptr<ReadBuffer> rb =_free.front();
            _free.pop_front();
            return rb;
        }
    }
    cf_void PutIntoPool(std::shared_ptr < ReadBuffer > rb)
    {
        _free.push_back(rb);
    }

private:
    std::list < std::shared_ptr < ReadBuffer > > _free;
};

class WriteBufferPool : public cf::NonCopyable
{
public:
    WriteBufferPool()
    {
    }
    ~WriteBufferPool()
    {
    }
};


class ReadBufferPoolGuard : public cf::NonCopyable
{
public:
    ReadBufferPoolGuard(ReadBufferPool & rbp)
        :_rbp(rbp)
    {
        _rb =_rbp.GetFromPool();
    }
    ~ReadBufferPoolGuard()
    {
        _rbp.PutIntoPool(_rb);
    }

    std::shared_ptr < ReadBuffer > Get()
    {
        return _rb;
    }
private:
    ReadBufferPool & _rbp;
    std::shared_ptr<ReadBuffer> _rb;
};



} // namespace cl

#endif // _HEADER_FILE_CFD_CL_BUFFER_HPP_

