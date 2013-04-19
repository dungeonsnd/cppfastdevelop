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

#ifndef _HEADER_FILE_CFD_CF_CONCURRENCY_SET_H_
#define _HEADER_FILE_CFD_CF_CONCURRENCY_SET_H_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_utility.hpp" // type traits.
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"
#include "cppfoundation/cf_condition.hpp"

namespace cf
{

// set
DEFINECLASS_CHILD_EXCEPTION(SetEmptyException, Info);
DEFINECLASS_CHILD_EXCEPTION(SetStopToPopException, Info);
DEFINECLASS_CHILD_EXCEPTION(SetStopToInsertException, Info);

template <typename SetType, typename Raw_RWMutex>
class RawSet: public NonCopyable
{
public:
    typedef typename SetType::iterator IteratorType;
    typedef typename SetType::value_type ElementType;
    typedef typename std::pair<IteratorType,bool> InsertResultType;

    typedef RWMutex < Raw_RWMutex > RWMutexType;

    RawSet() :
        _readLock(_rawRWLock, lockdefs::READ),
        _writeLock(_rawRWLock, lockdefs::WRITE)
    {
    }
    virtual ~RawSet()
    {
    }
    bool IsEmpty ( ) cf_const
    {
        LockGuard <RWMutexType> lock(_readLock);
        return _set.empty();
    }
    size_t Size() cf_const
    {
        LockGuard<RWMutexType> lock(_readLock);
        return size_t(_set.size());
    }
    size_t MaxSize () cf_const
    {
        LockGuard<RWMutexType> lock(_readLock);
        return size_t(_set.max_size());
    }

    IteratorType Find ( cf_const ElementType & x ) cf_const
    {
        LockGuard<RWMutexType> lock(_readLock);
        return _set.find(x);
    }

    InsertResultType Insert ( cf_const ElementType & x )
    {
        LockGuard<RWMutexType> lock(_writeLock);
        return _set.insert(x);
    }

    size_t Erase ( cf_const ElementType & x )
    {
        LockGuard<RWMutexType> lock(_writeLock);
        return size_t(_set.erase(x));
    }

    cf_void Pop(ElementType & x)
    {
        LockGuard<RWMutexType> lock(_writeLock);
        if(_set.empty())
            _THROW(SetEmptyException, "Set empty!");
        IteratorType it =_set.begin();
        x =*it;
        _set.erase(it);
    }

    cf_void Clear()
    {
        LockGuard<RWMutexType> lock(_writeLock);
        return _set.clear();
    }
private:
    Raw_RWMutex _rawRWLock;
    RWMutexType _readLock;
    RWMutexType _writeLock;
    SetType _set;
};


template <typename SetType, typename MutexType>
class RawNotifySet: public NonCopyable
{
public:
    typedef typename SetType::iterator IteratorType;
    typedef typename SetType::value_type ElementType;
    typedef typename std::pair<IteratorType,bool> InsertResultType;

    RawNotifySet() :
        _stopToInsert(false),
        _stopToPop(false)
    {
    }
    ~RawNotifySet()
    {
    }
    bool IsEmpty ( ) cf_const
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        return _set.empty();
    }
    size_t Size() cf_const
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        return size_t(_set.size());
    }
    size_t MaxSize () cf_const
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        return size_t(_set.max_size());
    }

    IteratorType Find ( cf_const ElementType & x ) cf_const
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        return _set.find(x);
    }

    InsertResultType Insert ( cf_const ElementType & x )
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        if (_stopToInsert)
            _THROW(SetStopToInsertException, "This set has been stopped to insert!");

        InsertResultType rt =_set.insert(x);
        if (_waitCount != 0)
        {
            --_waitCount;
            _cond.Signal();
        }
        return rt;
    }

    size_t Erase ( cf_const ElementType & x )
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        return size_t(_set.erase(x));
    }

    cf_void Pop(ElementType & x)
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        while (_set.empty() && false==_stopToPop)
        {
            ++_waitCount;
            _cond.Wait();
        }
        if (_stopToPop)
            _THROW(SetStopToPopException, "This set has been stopped to pop!");
        if(_set.empty() && _stopToPop)
            _THROW(SetEmptyException, "This set is empty and stopped to pop!");

        IteratorType it =_set.begin();
        x =*it;
        _set.erase(it);
    }
    cf_void TryPop(ElementType & x)
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        if (_stopToPop)
            _THROW(SetStopToPopException, "This set has been stopped to pop!");
        if(_set.empty())
            _THROW(SetEmptyException, "Set empty!");
        IteratorType it =_set.begin();
        x =*it;
        _set.erase(it);
    }

    cf_void Clear()
    {
        LockGuard<MutexType> lock(_cond.GetLock());
        return _set.clear();
    }

    cf_void StopInsertAndPop()
    {
        _stopToInsert = true;
        _stopToPop = true;
        _cond.Broadcast();
    }
    cf_void StopInsert()
    {
        _stopToInsert = true;
        _cond.Broadcast();
    }
private:
    PthreadCond _cond;
    SetType _set;
    volatile bool _stopToInsert;
    volatile bool _stopToPop;
    cf_int _waitCount;
};


template <typename KeyType, typename MutexType>
struct TYPESet
{
    // RawPthreadRWMutex/FakeRWMutex/RawFileRWMutex
    typedef RawSet < std::set<KeyType>, MutexType > Set;

    // PthreadMutex/FakeMutex
    typedef RawNotifySet < std::set<KeyType>, MutexType > NotifySet;
};



} // namespace cf

#endif // _HEADER_FILE_CFD_CF_CONCURRENCY_SET_H_
