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

#ifndef _HEADER_FILE_CFD_CL_QUEUE_OPERATION_HPP_
#define _HEADER_FILE_CFD_CL_QUEUE_OPERATION_HPP_

#include "cl_queue_def.hpp"

// In this file ,now , it is thread-safety and has enough space to write.

namespace cl
{
namespace cq
{


class QueueOperation : public cf::NonCopyable
{
public:

    QueueOperation() {}
    virtual ~QueueOperation() {}

    virtual cf_uint64 MetaSize() cf_const { return (cf_uint64)(sizeof(QMeta));}
    virtual cf_uint64 HeaderSize() cf_const { return (cf_uint64)(sizeof(ElementHeader));}

private:
};

class QMetaOp : public QueueOperation
{
public:
    QMetaOp(cf_pvoid pMem,bool creator,cf_uint64 memSize);
    ~QMetaOp() {}

    cf_void StopPut()
    {
        _pMeta->status =Q_STOP2PUT;
    }
    cf_void StopPutAndGet()
    {
        _pMeta->status =Q_STOP2PUTANDGET;
    }
    bool IsStopPut()
    {
        return Q_STOP2PUT==_pMeta->status;
    }
    bool IsStopPutAndGet()
    {
        return Q_STOP2PUTANDGET==_pMeta->status;
    }

    PQMeta GetPQMeta()
    {
        checkPointer();
        return _pMeta;
    }

    cf_uint64 & TotalSize()
    {
        checkPointer();
        return _pMeta->qTotalSize;
    }
    cf_uint64 & LeftSize()
    {
        checkPointer();
        return _pMeta->qLeftSize;
    }
    cf_uint64 & ElementCount()
    {
        checkPointer();
        return _pMeta->elementCount;
    }

    pthread_mutex_t * Mutex()
    {
        checkPointer();
        return &(_pMeta->mutex);
    }
    pthread_cond_t * Cond()
    {
        checkPointer();
        return &(_pMeta->cond);
    }
    pid_t & OperationPid()
    {
        checkPointer();
        return _pMeta->operationPid;
    }

private:
    inline cf_void checkPointer() cf_const
    {
        if(NULL==_pMeta)
            _THROW(cf::NullPointerError, "NULL==_pMeta !")
        }

    cf_void initMutex();
    cf_void finalizeMutex();
    cf_void initCond();
    cf_void finalizeCond();

    PQMeta _pMeta;
    bool _creator;
};
class ElementOp : public QueueOperation
{
public:
    enum eQueueScene
    {
        QUEUE_SCENE1=1, // head > tail
        QUEUE_SCENE2=2, // head < tail
        QUEUE_SCENE3=3, // head == tail,count=0
        QUEUE_SCENE4=4 // head == tail,count!=0
    };

    enum eWriteCase
    {
        QUEUE_CASE1=1, // over-head>sizeof(Element) , write all element.
        QUEUE_CASE2=2, // over-head=sizeof(Element) , write all element.
        QUEUE_CASE3=3, // over-head<sizeof(ElementHeader) , only write some header.
        QUEUE_CASE4=4, // over-head>sizeof(ElementHeader) && over-head<sizeof(Element), only write some body.
        QUEUE_CASE5=5 // over-head=sizeof(ElementHeader) , only write header.
    };

    ElementOp(PQMeta pMeta):_pMeta(pMeta)
    {
        _pQueue =_pMeta->elementBegin;
    }
    ~ElementOp()
    {
    }
    cf_void Write(cf_cpvoid pData,cf_const cf_uint64 size)
    {
        checkPointer();
        write(pData,size);
    }
    cf_void Read(std::string & data)
    {
        checkPointer();
        read(data);
    }

private:
    inline cf_void checkPointer() cf_const
    {
        if(NULL==_pMeta)
            _THROW(cf::NullPointerError, "NULL==_pMeta !")
        }

    eQueueScene QueueScene() cf_const;
    eWriteCase QueueWriteCase(cf_uint64 bodySize) cf_const;

    cf_void read(std::string & data);
    cf_void write(cf_cpvoid pData,cf_const cf_uint64 size);

    PQMeta _pMeta;
    cf_byte * _pQueue;
};


} // namespace cq
} // namespace cl
#endif // _HEADER_FILE_CFD_CL_QUEUE_OPERATION_HPP_

