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

#ifndef _HEADER_FILE_CFD_CL_CIRCULAR_QUEUE_HPP_
#define _HEADER_FILE_CFD_CL_CIRCULAR_QUEUE_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"
#include "cppfoundation/cf_condition.hpp"
#include "circularqueue/cl_queue_operation.hpp"

namespace cl
{
namespace cq
{

class CircularQueue : public cf::NonCopyable
{
public:
    CircularQueue(cf_pvoid pMem,bool creator =false,cf_uint64 memSize =0);
    ~CircularQueue();

    cf_void StopPut();
    cf_void StopPutAndGet();

    cf_void Put(cf_cpvoid pData,cf_uint64 size);
    cf_void TryPut(cf_cpvoid pData,cf_uint64 size);
    cf_void TimedPut(cf_cpvoid pData,cf_uint64 size,cf_int32 timeoutMilliseconds);

    cf_void Get(std::string & data);
    cf_void TryGet(std::string & data);
    cf_void TimedGet(std::string & data,cf_int32 timeoutMilliseconds);

protected:
    cf_void CheckProcessAndRestoreLock();
    cf_void CheckPointer() cf_const;

    std::shared_ptr<QMetaOp> _qMetaOp;
    std::shared_ptr<ElementOp> _elementOp;
    std::shared_ptr<cf::RawPthreadCondition> _cond;
};

} // namespace cq
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_CIRCULAR_QUEUE_HPP_

