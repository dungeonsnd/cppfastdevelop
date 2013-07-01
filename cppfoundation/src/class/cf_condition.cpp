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

#include "cppfoundation/cf_condition.hpp"

namespace cf
{

class PthreadCondAttrGuard : public NonCopyable
{
public:
    PthreadCondAttrGuard(pthread_condattr_t  & Attr):_Attr(Attr)
    {
        cf_int rt =pthread_condattr_init(&_Attr);
        if (0!=rt)
            _THROW_FMT(SyscallExecuteError,
                       "Failed to execute pthread_condattr_init , rt=%d !",rt);
    }
    ~PthreadCondAttrGuard()
    {
        if(0!=pthread_condattr_destroy(&_Attr))
            ;
    }
private:
    pthread_condattr_t & _Attr;
};

PthreadCond::PthreadCond()
{
    cf_int rt =pthread_cond_init(&_cond, NULL);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_cond_init , rt=%d !",
                   rt);
}
PthreadCond::~PthreadCond()
{
    cf_int rt =pthread_cond_destroy(&_cond);
    if (0!=rt)
    {
        // Don't throw exception in destructor! Maybe can write log here!
    }
}
cf_void PthreadCond::Wait() cf_const
{
    cf_int rt =pthread_cond_wait(&_cond, &_pthreadMutex.GetMutex());
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_cond_wait , rt=%d !",rt);
}
bool PthreadCond::Wait(cf_uint32 timeoutMilliseconds)
{
    struct timespec ts;
    ts.tv_sec =timeoutMilliseconds/1000;
    ts.tv_nsec =timeoutMilliseconds%1000 * 1000000;
    cf_int rt = pthread_cond_timedwait(&_cond, &_pthreadMutex.GetMutex(), &ts);
    if (0==rt)
        return true;
    else if (0!=rt&& ETIMEDOUT==errno)
        return false;
    else
        _THROW_FMT(SyscallExecuteError,
                   "Failed to execute pthread_cond_timedwait , rt=%d !",rt);
}
cf_void PthreadCond::Signal() cf_const
{
    cf_int rt =pthread_cond_signal(&_cond);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_cond_signal , rt=%d !",rt);
}
cf_void PthreadCond::Broadcast() cf_const
{
    cf_int rt =pthread_cond_broadcast(&_cond);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_cond_broadcast , rt=%d !",rt);
}
PthreadMutex & PthreadCond::GetLock()
{
    return _pthreadMutex;
}


RawPthreadCondition::RawPthreadCondition(pthread_cond_t * cond,
        pthread_mutex_t * mutex)
    :_cond(cond),
     _rawPthreadMutex(mutex)
{
}
RawPthreadCondition::~RawPthreadCondition()
{
}
cf_void RawPthreadCondition::Wait()
{
    cf_int rt =pthread_cond_wait(_cond, &_rawPthreadMutex.GetMutex());
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_cond_wait , rt=%d !",
                   rt);
}
bool RawPthreadCondition::Wait(cf_uint32 timeoutMilliseconds)
{
    struct timespec ts;
    ts.tv_sec =timeoutMilliseconds/1000;
    ts.tv_nsec =timeoutMilliseconds%1000 * 1000000;
    cf_int rt = pthread_cond_timedwait(_cond, &_rawPthreadMutex.GetMutex(), &ts);
    if (0==rt)
        return true;
    else if (0!=rt&& ETIMEDOUT==errno)
        return false;
    else
        _THROW_FMT(SyscallExecuteError,
                   "Failed to execute pthread_cond_timedwait , rt=%d !",rt);
}
cf_void RawPthreadCondition::Signal()
{
    cf_int rt =pthread_cond_signal(_cond);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError,
                   "Failed to execute pthread_cond_signal , rt=%d !",rt);
}
cf_void RawPthreadCondition::Broadcast()
{
    cf_int rt =pthread_cond_broadcast(_cond);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError,
                   "Failed to execute pthread_cond_broadcast , rt=%d !",rt);
}
RawPthreadMutex & RawPthreadCondition::GetLock()
{
    return _rawPthreadMutex;
}


} // namespace cf

