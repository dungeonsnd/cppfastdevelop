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


#include "circularqueue/cl_queue_operation.hpp"

namespace cl
{
namespace cq
{

QMetaOp::QMetaOp(cf_pvoid pMem,bool creator,cf_uint64 memSize):
    _pMeta((PQMeta)(pMem)),
    _creator(creator)
{
    if(_creator)
    {
        if( memSize<=MetaSize()+HeaderSize() )
            _THROW_FMT(SizeTooSmallException,
                       "memSize{%ld}<=MetaSize(){%ld}+HeaderSize(){%ld} !",memSize,MetaSize() ,
                       HeaderSize())

            memset(_pMeta,0,MetaSize());
        cf_memcpy(_pMeta->magic,"0CFD",sizeof(_pMeta->magic));
        cf_memcpy(_pMeta->version,"0001",sizeof(_pMeta->version));

        _pMeta->qTotalSize =memSize - MetaSize();
        _pMeta->qLeftSize =memSize - MetaSize();
        _pMeta->elementCount =0;

        _pMeta->headIndex =0;
        _pMeta->tailIndex =0;

        _pMeta->serialNO =0;
        initMutex();
        initCond();

        _pMeta->operationTime =(time_t)-1;
        _pMeta->operationPid =0;
        cf_memset(_pMeta->extention,0,sizeof(_pMeta->extention));
        _pMeta->status =Q_NOMAL;
    }
}


cf_void QMetaOp::initMutex()
{
    pthread_mutexattr_t attr;
    cf_int rt =pthread_mutexattr_init(&attr);
    if (0!=rt)
        _THROW_FMT(cf::SyscallExecuteError,
                   "Failed to execute pthread_mutexattr_init , rt=%d !",rt)

        rt =pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    if (0!=rt)
    {
        pthread_mutexattr_destroy(&attr);
        _THROW_FMT(cf::SyscallExecuteError,
                   "Failed to execute pthread_mutexattr_setpshared , rt=%d !",rt)
    }

    rt =pthread_mutex_init(&_pMeta->mutex, &attr);
    if (0!=rt)
    {
        pthread_mutexattr_destroy(&attr);
        _THROW_FMT(cf::SyscallExecuteError,
                   "Failed to execute pthread_mutex_init , rt=%d !",rt)
    }

    pthread_mutexattr_destroy(&attr);
}
cf_void QMetaOp::finalizeMutex()
{
}
cf_void QMetaOp::initCond()
{
    pthread_condattr_t attr;
    cf_int rt =pthread_condattr_init(&attr);
    if (0!=rt)
        _THROW_FMT(cf::SyscallExecuteError,
                   "Failed to execute pthread_condattr_init , rt=%d !",rt)

        rt =pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    if (0!=rt)
    {
        pthread_condattr_destroy(&attr);
        _THROW_FMT(cf::SyscallExecuteError,
                   "Failed to execute pthread_condattr_setpshared , rt=%d !",rt)
    }

    rt =pthread_cond_init(&_pMeta->cond, &attr);
    if (0!=rt)
    {
        pthread_condattr_destroy(&attr);
        _THROW_FMT(cf::SyscallExecuteError,
                   "Failed to execute pthread_cond_init , rt=%d !",rt)
    }

    pthread_condattr_destroy(&attr);
}
cf_void QMetaOp::finalizeCond()
{
}

ElementOp::eQueueScene ElementOp::QueueScene() cf_const
{
    checkPointer();
    eQueueScene s;
    if (_pMeta->headIndex > _pMeta->tailIndex)
        s =QUEUE_SCENE1;
    else if(_pMeta->headIndex < _pMeta->tailIndex)
        s =QUEUE_SCENE2;
    else if( 0==_pMeta->elementCount )
        s =QUEUE_SCENE3;
    else if( 0!=_pMeta->elementCount )
        s =QUEUE_SCENE4;
    else
        _THROW(cf::ValueError, "eQueueScene error ! ")

        return s;
}
ElementOp::eWriteCase ElementOp::QueueWriteCase(cf_uint64 bodySize) cf_const
{
    checkPointer();

    if( _pMeta->qTotalSize < _pMeta->headIndex+1 )
        _THROW_FMT(cf::ValueError, "_pMeta->qTotalSize{%ld} < _pMeta->headIndex{%ld}+1 !",_pMeta->qTotalSize,_pMeta->headIndex)

        eWriteCase c;
    if (_pMeta->qTotalSize > _pMeta->headIndex + HeaderSize()+bodySize)
        c =QUEUE_CASE1;
    else if (_pMeta->qTotalSize == _pMeta->headIndex + HeaderSize()+bodySize)
        c =QUEUE_CASE2;
    else if(_pMeta->qTotalSize < _pMeta->headIndex + HeaderSize())
        c =QUEUE_CASE3;
    else if (  (_pMeta->qTotalSize > _pMeta->headIndex + HeaderSize()) &&
    (_pMeta->qTotalSize < _pMeta->headIndex + HeaderSize()+bodySize)
            )
        c =QUEUE_CASE4;
    else if ( _pMeta->qTotalSize == _pMeta->headIndex + HeaderSize() )
        c =QUEUE_CASE5;
    else
        _THROW(cf::ValueError, "eWriteCase error !");
    return c;
}

cf_void ElementOp::read(std::string & data)
{
    ////
    //printf("** Before read:qTotalSize=%lu, qLeftSize=%lu, elementCount=%lu, headIndex=%lu, tailIndex=%lu, serialNO=%lu, operationTime=%d, operationPid=%d, status=%d,  sizeof(QMeta)=%lu, sizeof(ElementHeader)=%lu \n",    _pMeta->qTotalSize,_pMeta->qLeftSize,_pMeta->elementCount,_pMeta->headIndex,_pMeta->tailIndex,_pMeta->serialNO,int(_pMeta->operationTime),_pMeta->operationPid,int(_pMeta->status),(cf_uint64)(sizeof(QMeta)),(cf_uint64)(sizeof(ElementHeader)) );
    ////

    eQueueScene scene =QueueScene();
    eWriteCase cs;

    cf_uint64 size =0;
    ElementHeader h;
    cf_byte * ph =(cf_byte *)(&h);

    if( _pMeta->qTotalSize < _pMeta->headIndex+1 )
        _THROW_FMT(cf::ValueError,
                   "_pMeta->qTotalSize{%lu} < _pMeta->headIndex{%lu}+1 ", _pMeta->qTotalSize ,
                   _pMeta->headIndex)

        if(QUEUE_SCENE1==scene)
        {
            if( _pMeta->headIndex< _pMeta->tailIndex+sizeof(h) )
                _THROW_FMT(cf::ValueError,
                           "_pMeta->headIndex{%lu}< _pMeta->tailIndex{%lu}+sizeof(h){%lu}",
                           _pMeta->headIndex,_pMeta->tailIndex,sizeof(h))
                else
                {
                    //    printf("==== read route 1 ==== \n");
                    cf_memcpy(ph,_pQueue+_pMeta->tailIndex,sizeof(h));
                    if(0==h.bodyLen)
                        _THROW(cf::ValueError, " 0==h.bodyLen !");
                    if( _pMeta->headIndex < _pMeta->tailIndex+sizeof(h)+h.bodyLen ) ////
                        _THROW_FMT(cf::ValueError,
                                   "_pMeta->headIndex{%lu} < _pMeta->tailIndex{%lu}+sizeof(h){%lu}+h.bodyLen{%lu}",
                                   _pMeta->headIndex ,_pMeta->tailIndex ,sizeof(h) ,h.bodyLen)

                        size =h.bodyLen;
                    data.resize(size,'0');
                    cf_memcpy(&(data[0]),_pQueue+_pMeta->tailIndex+sizeof(h),size);
                    _pMeta->tailIndex +=(sizeof(h)+size);
                }
        }
        else if(QUEUE_SCENE2==scene||QUEUE_SCENE4==scene)
        {
            if( _pMeta->qTotalSize < _pMeta->tailIndex+1 )
                _THROW_FMT(cf::ValueError,
                           " _pMeta->qTotalSize{%lu} < _pMeta->tailIndex{%lu}+1", _pMeta->qTotalSize ,
                           _pMeta->tailIndex)

                cf_memcpy(ph,_pQueue+_pMeta->tailIndex,1);
            cs =eWriteCase(h.writeCase);
            if(QUEUE_CASE1==cs || QUEUE_CASE2==cs)
            {
                //    printf("==== read route 2 ==== \n");
                cf_memcpy(ph,_pQueue+_pMeta->tailIndex,sizeof(h));
                if(0==h.bodyLen)
                    _THROW(cf::ValueError, " 0==h.bodyLen !");
                if( _pMeta->qTotalSize < _pMeta->tailIndex+sizeof(h)+h.bodyLen )
                    _THROW_FMT(cf::ValueError,
                               " _pMeta->qTotalSize{%lu} < _pMeta->tailIndex{%lu}+sizeof(h){%lu}+h.bodyLen{%lu}",
                               _pMeta->qTotalSize , _pMeta->tailIndex,sizeof(h),h.bodyLen)

                    size =h.bodyLen;
                data.resize(size,'0');
                cf_memcpy(&(data[0]),_pQueue+_pMeta->tailIndex+sizeof(h),size);
                if(QUEUE_CASE1==cs)
                    _pMeta->tailIndex +=sizeof(h)+size;
                else
                    _pMeta->tailIndex =0;
            }
            else if(QUEUE_CASE3==cs)
            {
                //    printf("==== read route 3 ==== \n");
                cf_uint64 size0 =_pMeta->qTotalSize-_pMeta->tailIndex;
                cf_uint64 size1 =sizeof(h)-size0;
                cf_memcpy(ph,_pQueue+_pMeta->tailIndex,size0);
                cf_memcpy(ph+size0,_pQueue+0,size1);
                if(0==h.bodyLen)
                    _THROW(cf::ValueError, " 0==h.bodyLen !");
                if( _pMeta->headIndex-0 < size1+h.bodyLen )
                    _THROW_FMT(cf::ValueError,
                               " _pMeta->headIndex{%lu}-0 < size1{%lu}+h.bodyLen{%lu}", _pMeta->headIndex ,
                               size1,h.bodyLen)

                    size =h.bodyLen;
                data.resize(size,'0');
                cf_memcpy(&(data[0]),_pQueue+size1,size);
                _pMeta->tailIndex =size1+size;
            }
            else if(QUEUE_CASE4==cs)
            {
                //    printf("==== read route 4 ==== \n");
                cf_memcpy(ph,_pQueue+_pMeta->tailIndex,sizeof(h));
                if(0==h.bodyLen)
                    _THROW_FMT(cf::ValueError, " 0==h.bodyLen !");
                if( _pMeta->qTotalSize < _pMeta->tailIndex+sizeof(h)+1 )
                    _THROW_FMT(cf::ValueError,
                               " _pMeta->qTotalSize{%lu} < _pMeta->tailIndex{%lu}+sizeof(h){%lu}+1 ",
                               _pMeta->qTotalSize , _pMeta->tailIndex, sizeof(h))

                    size =h.bodyLen;
                data.resize(size,'0');
                cf_uint64 size0 =_pMeta->qTotalSize-_pMeta->tailIndex-sizeof(h);
                cf_uint64 size1 =size-size0;
                cf_byte * p =(cf_byte *)(&(data[0]));
                cf_memcpy(p,_pQueue+_pMeta->tailIndex+sizeof(h),size0);
                cf_memcpy(p+size0,_pQueue+0,size1);
                _pMeta->tailIndex =size1;
            }
            else if(QUEUE_CASE5==cs)
            {
                //    printf("==== read route 5 ==== \n");
                if( _pMeta->qTotalSize != _pMeta->tailIndex+sizeof(h) )
                    _THROW_FMT(cf::ValueError,
                               "_pMeta->qTotalSize{%lu} != _pMeta->tailIndex{%lu}+sizeof(h){%lu} ",
                               _pMeta->qTotalSize , _pMeta->tailIndex , sizeof(h))

                    cf_memcpy(ph,_pQueue+_pMeta->tailIndex,sizeof(h));
                if(0==h.bodyLen)
                    _THROW(cf::ValueError, " 0==h.bodyLen !");

                size =h.bodyLen;
                data.resize(size,'0'); ////
                cf_memcpy(&(data[0]),_pQueue+0,size);
                _pMeta->tailIndex =size;
            }
            else
                _THROW_FMT(cf::ValueError, "read QueueWriteCase error ! cs=%d", int(cs))
            }
        else
            _THROW_FMT(cf::ValueError, "read QueueScene error ! scene=%d", int(scene))

            _pMeta->qLeftSize +=(HeaderSize()+size);
    _pMeta->elementCount --;
    _pMeta->serialNO ++;
    cf_time(&(_pMeta->operationTime));
    _pMeta->operationPid =cf_getpid();
    cf_memset(_pMeta->extention,0,sizeof(_pMeta->extention));

    if( _pMeta->qTotalSize < _pMeta->headIndex+1 )
        _THROW_FMT(cf::ValueError,
                   "_pMeta->qTotalSize{%lu} < _pMeta->headIndex{%lu}+1 ", _pMeta->qTotalSize ,
                   _pMeta->headIndex)

        ////
        //printf("** After read:qTotalSize=%lu, qLeftSize=%lu, elementCount=%lu, headIndex=%lu, tailIndex=%lu, serialNO=%lu, operationTime=%d, operationPid=%d, staus=%d, sizeof(QMeta)=%lu, sizeof(ElementHeader)=%lu \n",    _pMeta->qTotalSize,_pMeta->qLeftSize,_pMeta->elementCount,_pMeta->headIndex,_pMeta->tailIndex,_pMeta->serialNO,int(_pMeta->operationTime),_pMeta->operationPid,int(_pMeta->status),(cf_uint64)(sizeof(QMeta)),(cf_uint64)(sizeof(ElementHeader)) );
        ////
    }

cf_void ElementOp::write(cf_cpvoid pData,cf_const cf_uint64 size)
{
    ////
    //printf("** Before write:qTotalSize=%lu, qLeftSize=%lu, elementCount=%lu, headIndex=%lu, tailIndex=%lu, serialNO=%lu, operationTime=%d, operationPid=%d, status=%d, sizeof(QMeta)=%lu, sizeof(ElementHeader)=%lu \n",    _pMeta->qTotalSize,_pMeta->qLeftSize,_pMeta->elementCount,_pMeta->headIndex,_pMeta->tailIndex,_pMeta->serialNO,int(_pMeta->operationTime),_pMeta->operationPid,int(_pMeta->status),(cf_uint64)(sizeof(QMeta)),(cf_uint64)(sizeof(ElementHeader)) );
    ////
    if(0==size)
        _THROW(cf::ValueError, "0==size !");

    if( _pMeta->qTotalSize < _pMeta->headIndex+1 )
        _THROW_FMT(cf::ValueError,
                   "_pMeta->qTotalSize{%lu} < _pMeta->headIndex{%lu}+1 ", _pMeta->qTotalSize ,
                   _pMeta->headIndex)

        eQueueScene scene =QueueScene();
    eWriteCase cs =QueueWriteCase(size);

    ElementHeader h;
    cf_byte bcs =(cf_byte)cs;
    cf_memcpy(&(h.writeCase),&bcs,sizeof(cf_byte));
    cf_memcpy(h.extention,"000",sizeof(h.extention));
    h.bodyLen =size;
    cf_time(&(h.operationTime));
    cf_byte * ph =(cf_byte *)(&h);

    if(QUEUE_SCENE1==scene || QUEUE_SCENE3==scene)
    {
        if(QUEUE_CASE1==cs||QUEUE_CASE2==cs)
        {
            //    printf("---- write route 1 ---- \n");
            cf_memcpy(_pQueue+_pMeta->headIndex,ph,sizeof(h));
            cf_memcpy(_pQueue+_pMeta->headIndex+sizeof(h),pData,size);
            if(QUEUE_CASE1==cs)
                _pMeta->headIndex +=(size+sizeof(h));
            else
                _pMeta->headIndex =0;
        }
        else if(QUEUE_CASE3==cs)
        {
            //    printf("---- write route 2 ---- \n");
            cf_uint64 size0 =_pMeta->qTotalSize-_pMeta->headIndex;
            cf_uint64 size1 =sizeof(h)-size0;
            cf_memcpy(_pQueue+_pMeta->headIndex,ph,size0);
            cf_memcpy(_pQueue+0,ph+size0,size1);
            cf_memcpy(_pQueue+size1,pData,size);
            _pMeta->headIndex =size+size1;
        }
        else if(QUEUE_CASE4==cs)
        {
            //    printf("---- write route 3 ---- \n");
            cf_memcpy(_pQueue+_pMeta->headIndex,ph,sizeof(h));
            cf_uint64 size0 =_pMeta->qTotalSize-_pMeta->headIndex-sizeof(h);
            cf_uint64 size1 =size-size0;
            cf_memcpy(_pQueue+_pMeta->headIndex+sizeof(h),pData,size0);
            cf_byte * p=(cf_byte *)pData;
            cf_memcpy(_pQueue+0,p+size0,size1);
            _pMeta->headIndex =size1;
        }
        else if(QUEUE_CASE5==cs)
        {
            //    printf("---- write route 4 ---- \n");
            cf_memcpy(_pQueue+_pMeta->headIndex,ph,sizeof(h));
            cf_memcpy(_pQueue+0,pData,size);
            _pMeta->headIndex =size;
        }
        else
            _THROW_FMT(cf::ValueError, "write QueueWriteCase error ! cs=%d", int(cs))
        }
    else if(QUEUE_SCENE2==scene)
    {
        if( _pMeta->tailIndex < _pMeta->headIndex+sizeof(h)+size )
            _THROW_FMT(cf::ValueError,
                       " _pMeta->tailIndex{%lu} < _pMeta->headIndex{%lu}+sizeof(h){%lu}+size{%lu} ",
                       _pMeta->tailIndex , _pMeta->headIndex,sizeof(h),size)
            else
            {
                //    printf("---- write route 5 ---- \n");
                cf_memcpy(_pQueue+_pMeta->headIndex,ph,sizeof(h));
                cf_memcpy(_pQueue+_pMeta->headIndex+sizeof(h),pData,size);
                _pMeta->headIndex +=sizeof(h)+size;
            }
    }
    else
        _THROW_FMT(cf::ValueError, "write QueueScene error ! scene=%d", int(scene))

        _pMeta->qLeftSize -=(HeaderSize()+size);
    _pMeta->elementCount ++;
    _pMeta->serialNO ++;
    cf_time(&(_pMeta->operationTime));
    _pMeta->operationPid =cf_getpid();
    cf_memset(_pMeta->extention,0,sizeof(_pMeta->extention));

    if( _pMeta->qTotalSize < _pMeta->headIndex+1 )
        _THROW_FMT(cf::ValueError,
                   "_pMeta->qTotalSize{%lu} < _pMeta->headIndex{%lu}+1 ", _pMeta->qTotalSize ,
                   _pMeta->headIndex)

        ////
        //printf("** After write:qTotalSize=%lu, qLeftSize=%lu, elementCount=%lu, headIndex=%lu, tailIndex=%lu, serialNO=%lu, operationTime=%d, operationPid=%d, status=%d, sizeof(QMeta)=%lu, sizeof(ElementHeader)=%lu, size=%lu, pData=%s \n",   _pMeta->qTotalSize,_pMeta->qLeftSize,_pMeta->elementCount,_pMeta->headIndex,_pMeta->tailIndex,_pMeta->serialNO,int(_pMeta->operationTime),_pMeta->operationPid,int(_pMeta->status),(cf_uint64)(sizeof(QMeta)),(cf_uint64)(sizeof(ElementHeader)),size,(char*)pData );
        ////

    }

} // namespace cq
} // namespace cl



