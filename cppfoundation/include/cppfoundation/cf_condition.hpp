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

#ifndef _HEADER_FILE_CFD_CF_CONDITION_HPP_
#define _HEADER_FILE_CFD_CF_CONDITION_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"

namespace cf
{

class PthreadCond : public NonCopyable
{
public:
    PthreadCond();
    virtual ~PthreadCond();
    cf_void Wait() cf_const;
    bool Wait(cf_uint32 timeoutMilliseconds);
    cf_void Signal() cf_const;
    cf_void Broadcast() cf_const;
    PthreadMutex & GetLock();
private:
    mutable pthread_cond_t _cond;
    mutable PthreadMutex _pthreadMutex;
};


class RawPthreadCondition:public NonCopyable
{
public:
    RawPthreadCondition(pthread_cond_t *, pthread_mutex_t *);
    cf_void Wait();
    bool Wait(cf_uint32 timeoutMilliseconds);
    cf_void Signal();
    cf_void Broadcast();
    RawPthreadMutex & GetLock();
    ~RawPthreadCondition();

private:
    pthread_cond_t * _cond;
    RawPthreadMutex _rawPthreadMutex;
};


}
#endif // _HEADER_FILE_CFD_CF_CONDITION_HPP_

