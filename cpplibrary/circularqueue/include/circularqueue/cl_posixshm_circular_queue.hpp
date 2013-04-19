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

#ifndef _HEADER_FILE_CFD_CL_POSIXSHM_CIRCULAR_QUEUE_HPP_
#define _HEADER_FILE_CFD_CL_POSIXSHM_CIRCULAR_QUEUE_HPP_

#include "cppfoundation/cf_ipc.hpp"
#include "circularqueue/cl_circular_queue.hpp"

namespace cl
{
namespace cq
{

class PosixShmCircularQueue : public cf::NonCopyable
{
public:
    PosixShmCircularQueue(cf_const std::string & name,bool creator,
                          cf_uint64 memSize,cf_int oflag = cf::ipcdefs::FLAG_RDWR,
                          mode_t mode =cf::ipcdefs::MODE_DEFAULT)
    {
        cf_int prot =cf::ipcdefs::PROT_DEFAULT;

        CF_NEWOBJ(pshm,cf::PosixShM,name.c_str(),memSize,oflag,prot,mode,creator);
        if(NULL==pshm)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        _shm.reset( pshm );

        CF_NEWOBJ(pqueue,CircularQueue,_shm->Get(),creator,memSize);
        if(NULL==pqueue)
            _THROW(cf::AllocateMemoryError, "Allocate memory failed !");
        _queue.reset( pqueue );
    }
    ~PosixShmCircularQueue() {}

    cf_void StopPut()
    {
        _queue->StopPut();
    }
    cf_void StopPutAndGet()
    {
        _queue->StopPutAndGet();
    }

    cf_void Put(cf_cpvoid pData,cf_uint64 size)
    {
        _queue->Put(pData,size);
    }
    cf_void TryPut(cf_cpvoid pData,cf_uint64 size)
    {
        _queue->TryPut(pData,size);
    }
    cf_void TimedPut(cf_cpvoid pData,cf_uint64 size,cf_int32 timeoutMilliseconds)
    {
        _queue->TimedPut(pData,size,timeoutMilliseconds);
    }

    cf_void Get(std::string & data)
    {
        _queue->Get(data);
    }
    cf_void TryGet(std::string & data)
    {
        _queue->TryGet(data);
    }
    cf_void TimedGet(std::string & data,cf_int32 timeoutMilliseconds)
    {
        _queue->TimedGet(data,timeoutMilliseconds);
    }
protected:
    std::shared_ptr < cf::PosixShM > _shm;
    std::shared_ptr < CircularQueue > _queue;
};

} // namespace cq
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_POSIXSHM_CIRCULAR_QUEUE_HPP_
