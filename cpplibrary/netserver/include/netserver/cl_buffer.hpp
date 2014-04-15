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
#include "cppfoundation/cf_utility.hpp"

namespace cl
{

namespace bufferdefs
{
enum
{
    SIZE_READBUFFER_DEFAULT =700,
    SIZE_WRITEBUFFER_DEFAULT =1300,
    SIZE_POOLSIZE_DEFAULT =512
};
} // namespace bufferdefs


class ReadBuffer : public cf::NonCopyable
{
public:
    ReadBuffer()
        :_buf(bufferdefs::SIZE_READBUFFER_DEFAULT,'\0'),_total(0),_already(0)
    {
    }
    ~ReadBuffer()
    {
    }

    cf_void Clear();
    cf_int Read(cf::T_SESSION session, bool & peerClosedWhenRead);
    cf_pvoid GetBuffer()
    {
        return &_buf[0];
    }
    cf_void SetTotal(cf_uint32 total);
    cf_uint32 GetTotal() cf_const
    {
        return _total;
    }
    cf_uint32 GetLeft() cf_const;
    bool IsComplete() cf_const;
private:
    std::string _buf;
    cf_uint32 _total;
    cf_uint32 _already;
};

class WriteBuffer : public cf::NonCopyable
{
public:
    WriteBuffer()
        :_buf(bufferdefs::SIZE_WRITEBUFFER_DEFAULT,'\0'),_total(0),_already(0)
    {
    }
    ~WriteBuffer()
    {
    }

    cf_void Clear();
    cf_int Write(cf::T_SESSION session);
    cf_void SetBuffer(cf_cpvoid buffer, cf_uint32 total);
    cf_uint32 GetTotal() cf_const
    {
        return _total;
    }
    cf_uint32 GetLeft() cf_const;
    bool IsComplete() cf_const;
private:
    std::string _buf;
    cf_uint32 _total;
    cf_uint32 _already;
private:
};

template < typename BufferType >
class BufferPool : public cf::NonCopyable
{
public:
    typedef std::list< std::shared_ptr< BufferType > > T_LISTFREE;

    BufferPool(cf_int poolsize =bufferdefs::SIZE_POOLSIZE_DEFAULT):
        _WaterMark_maxAllocCnt(0)
    {
        if(poolsize<1)
            _THROW_FMT(cf::ValueError, "poolsize{%u}!=0 !", poolsize);
        for(cf_int i=0; i<poolsize; i++)
        {
            _free.push_back(AllocOne());

            // Status logging.
            _WaterMark_maxAllocCnt++;
        }
    }
    ~BufferPool()
    {
    }

    static std::shared_ptr<BufferType> AllocOne()
    {
#if CF_SWITCH_PRINT
        fprintf (stdout, "BufferPool AllocOne,notice\n");
#endif
        std::shared_ptr < BufferType > rb;
        CF_NEWOBJ(p, BufferType);
        if(NULL==p)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        rb.reset(p);
        return rb;
    }

    std::shared_ptr < BufferType > GetFromPool()
    {
        if(_free.empty())
        {
#if CF_SWITCH_PRINT
            fprintf (stderr, "BufferTypePool,GetFromPool,AllocOne \n");
#endif
            std::shared_ptr<BufferType> rb =AllocOne();

            // Status logging.
            _WaterMark_maxAllocCnt++;
            return rb;
        }
        else
        {
#if CF_SWITCH_PRINT
            fprintf (stderr, "BufferTypePool,GetFromPool,pop_front \n");
#endif
            std::shared_ptr<BufferType> rb =_free.front();
            _free.pop_front();
            return rb;
        }
    }
    cf_void PutIntoPool(std::shared_ptr < BufferType > rb)
    {
#if CF_SWITCH_PRINT
        fprintf (stderr, "BufferTypePool,PutIntoPool \n");
#endif
        _free.push_back(rb);
    }

    cf_void DumpStatus()
    {
        printf("    _free.size=%u \n",(cf_uint32)(_free.size()));
        printf("    maxAllocCnt=%d \n",_WaterMark_maxAllocCnt);

    }
private:
    T_LISTFREE _free;

    // Status logging.
    int _WaterMark_maxAllocCnt;
};




} // namespace cl

#endif // _HEADER_FILE_CFD_CL_BUFFER_HPP_

