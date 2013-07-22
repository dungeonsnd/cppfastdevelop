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

#ifndef _HEADER_FILE_CFD_CF_LOCK_GUARD_HPP_
#define _HEADER_FILE_CFD_CF_LOCK_GUARD_HPP_

#include "cppfoundation/cf_root.hpp"

namespace cf
{

template <typename T>
class LockGuard : public NonCopyable
{
public:
    LockGuard(cf_const T & lock):_lock(lock)
    {
        _lock.Lock();
    }
    ~LockGuard()
    {
        _lock.UnLock();
    }
private:
    cf_const T & _lock;
};


template <typename T>
class PtrLockGuard : public NonCopyable
{
public:
    PtrLockGuard(cf_const T * lock):_lock(lock)
    {
        if(NULL == _lock)
            _THROW(NullPointerError, "NULL == _lock !");
        _lock->Lock();
    }
    ~PtrLockGuard()
    {
        _lock->UnLock();
    }
private:
    cf_const T * _lock;
};


template <typename T>
class LockGuardUnlock : public NonCopyable
{
public:
    LockGuardUnlock(cf_const T & lock):_lock(lock)
    {
    }
    ~LockGuardUnlock()
    {
        _lock.UnLock();
    }
private:
    cf_const T & _lock;
};

} // namespace cf
#endif // _HEADER_FILE_CFD_CF_LOCK_GUARD_HPP_

