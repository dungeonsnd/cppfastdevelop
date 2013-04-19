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

#include "circularqueue/cl_circular_queue.hpp"

namespace cl
{
namespace cq
{

CircularQueue::CircularQueue(cf_pvoid pMem,bool creator,cf_uint64 memSize)
{
    _qMetaOp.reset( new QMetaOp(pMem,creator,memSize) );
    _elementOp.reset( new ElementOp(_qMetaOp->GetPQMeta()) );
    _cond.reset( new cf::RawPthreadCondition(_qMetaOp->Cond(),_qMetaOp->Mutex()) );
}
CircularQueue::~CircularQueue()
{
}

cf_void CircularQueue::StopPut()
{
    CheckProcessAndRestoreLock();
    {
        cf::LockGuard<cf::RawPthreadMutex> lock( _cond->GetLock() );
    }
    _cond->Broadcast();
}

cf_void CircularQueue::StopPutAndGet()
{
    CheckProcessAndRestoreLock();
    {
        cf::LockGuard<cf::RawPthreadMutex> lock( _cond->GetLock() );
    }
    _cond->Broadcast();
}

cf_void CircularQueue::Put(cf_cpvoid pData,cf_uint64 size)
{
    TimedPut(pData,size,-1);
}

cf_void CircularQueue::TryPut(cf_cpvoid pData,cf_uint64 size)
{
    TimedPut(pData,size,0);
}
cf_void CircularQueue::TimedPut(cf_cpvoid pData,cf_uint64 size,
                                cf_int32 timeoutMilliseconds)
{
    CheckPointer();
    CheckProcessAndRestoreLock();
    {
        cf::LockGuard<cf::RawPthreadMutex> lock( _cond->GetLock() );

        if ( _qMetaOp->IsStopPut() || _qMetaOp->IsStopPutAndGet() )
            _THROW(StopPutException,
                   "_qMetaOp->IsStopPut() || _qMetaOp->IsStopPutAndGet() !");

        cf_uint64 elementSize =size+_qMetaOp->HeaderSize();
        cf_uint64 & leftSize =_qMetaOp->LeftSize();
        if( elementSize >_qMetaOp->TotalSize()  )
            _THROW_FMT(SizeTooSmallException,
                       "elementSize >_qMetaOp->TotalSize() ,elementSize =%ld ,qTotalSize =%ld !",
                       elementSize,_qMetaOp->TotalSize())

            if( elementSize >leftSize  )
            {
                if(timeoutMilliseconds==0)
                    _THROW(QueueFullException, "Queue is full, elementSize > leftSize !")
                    else if(timeoutMilliseconds<0)
                    {
                        while(  (false==_qMetaOp->IsStopPut() || false==_qMetaOp->IsStopPutAndGet()) &&
                                elementSize > leftSize )
                            _cond->Wait();
                    }
                    else
                    {
                        time_t start = cf_time(NULL);
                        time_t now = cf_time(NULL);
                        while(  (false==_qMetaOp->IsStopPut() || false==_qMetaOp->IsStopPutAndGet()) &&
                                elementSize > leftSize )
                        {
                            if ( timeoutMilliseconds <= 0 || false==_cond->Wait(timeoutMilliseconds) )
                                _THROW(cf::TimeoutInfo, "Time out of _cond->Wait !")
                                else
                                {
                                    now = cf_time(NULL);
                                    timeoutMilliseconds -= (now - start);
                                }
                        }
                    }
            }

        if ( _qMetaOp->IsStopPut() || _qMetaOp->IsStopPutAndGet() )
            _THROW(StopPutException,
                   "_qMetaOp->IsStopPut() || _qMetaOp->IsStopPutAndGet() !")

            if( elementSize > leftSize )
                _THROW(cf::ValueError, "elementSize > leftSize wait !")

                _elementOp->Write(pData,size);
    }
    _cond->Signal();
}


cf_void CircularQueue::Get(std::string & data)
{
    TimedGet(data,-1);
}
cf_void CircularQueue::TryGet(std::string & data)
{
    TimedGet(data,0);
}
cf_void CircularQueue::TimedGet(std::string & data,cf_int32 timeoutMilliseconds)
{
    CheckPointer();
    CheckProcessAndRestoreLock();
    {
        cf::LockGuard<cf::RawPthreadMutex> lock( _cond->GetLock() );

        if ( _qMetaOp->IsStopPutAndGet() )
            _THROW(StopPutAndGetException, "_qMetaOp->IsStopPutAndGet() !")

            cf_uint64 & elementCount =_qMetaOp->ElementCount();
        if( 0==elementCount )
        {
            if(timeoutMilliseconds==0)
                _THROW(QueueFullException, "Queue is full, 0==elementCount !")
                else if(timeoutMilliseconds<0)
                {
                    while(  false==_qMetaOp->IsStopPutAndGet() && 0==elementCount )
                        _cond->Wait();
                }
                else
                {
                    time_t start = cf_time(NULL);
                    time_t now = cf_time(NULL);
                    while(  false==_qMetaOp->IsStopPutAndGet() && 0==elementCount )
                    {
                        if ( timeoutMilliseconds <= 0 || false==_cond->Wait(timeoutMilliseconds) )
                            _THROW(cf::TimeoutInfo, "Time out of _cond->Wait !")
                            else
                            {
                                now = cf_time(NULL);
                                timeoutMilliseconds -= (now - start);
                            }
                    }
                }
        }

        if ( _qMetaOp->IsStopPutAndGet() )
            _THROW(StopPutAndGetException, "_qMetaOp->IsStopPutAndGet() !")

            if( 0==elementCount )
                _THROW(cf::ValueError, "0==elementCount before Read() !")

                _elementOp->Read(data);
    }
    _cond->Signal();
}


cf_void CircularQueue::CheckProcessAndRestoreLock()
{
    pid_t & pid =_qMetaOp->OperationPid();
    if(pid<=0)
        return ; // ok

    cf_char proc[64] = {0};
    cf_snprintf(proc, sizeof(proc), "/proc/%d/",pid);
    if (0!=cf_access(proc, F_OK)) // Lock owner dead!
    {
    }
}

cf_void CircularQueue::CheckPointer() cf_const
{
    if(NULL==_qMetaOp.get())
        _THROW(cf::SyscallExecuteError, "NULL==_qMetaOp.get() !")
        if(NULL==_elementOp.get())
            _THROW(cf::SyscallExecuteError, "NULL==_elementOp.get() !")
            if(NULL==_cond.get())
                _THROW(cf::SyscallExecuteError, "NULL==_cond.get() !")
            }

} // namespace cq
} // namespace cl

