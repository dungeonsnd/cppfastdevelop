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

#ifndef _HEADER_FILE_CFD_CF_CONCURRENCY_MAP_H_
#define _HEADER_FILE_CFD_CF_CONCURRENCY_MAP_H_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_utility.hpp" // type traits.
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"
#include "cppfoundation/cf_condition.hpp"

namespace cf
{

// Map

template <typename MapType,typename K, typename V, typename Raw_RWMutex>
class RawMap: public NonCopyable
{
public:
    typedef typename MapType::iterator IteratorType;
    typedef typename MapType::const_iterator ConstIteratorType;
    typedef typename MapType::value_type ElementType;
    typedef typename std::pair<IteratorType,bool> InsertResultType;

    typedef RWMutex < Raw_RWMutex > RWMutexType;

    RawMap()
        :_readLock(_rawRWLock, lockdefs::READ),
         _writeLock(_rawRWLock, lockdefs::WRITE)
    {
    }
    ~RawMap()
    {
    }

    IteratorType Find(cf_const K & key)
    {
        LockGuard < RWMutexType > lock(_readLock);
        return _map.find(key);
    }
    bool HasKey(cf_const K & key)
    {
        LockGuard < RWMutexType > lock(_readLock);
        ConstIteratorType it = _map.find(key);
        if (it == _map.end())
            return false;
        return true;
    }

    V FindEx(cf_const K & key) cf_const
    {
        LockGuard < RWMutexType > lock(_readLock);
        ConstIteratorType it = _map.find(key);
        if (it == _map.end())
            _THROW(ElementNotFoundInfo, "Element not found!");
        return it->second;
    }
    V Pop(cf_const K & key)
    {
        LockGuard < RWMutexType > lock(_writeLock);
        IteratorType it = _map.find(key);
        if (it == _map.end())
            _THROW(ElementNotFoundInfo, "Element not found!");

        V v = it->second;
        _map.erase(key);
        return v;
    }
    IteratorType Begin()
    {
        LockGuard < RWMutexType > lock(_readLock);
        return _map.begin();
    }
    IteratorType End()
    {
        LockGuard < RWMutexType > lock(_readLock);
        return _map.end();
    }
    size_t Size() cf_const
    {
        LockGuard < RWMutexType > lock(_readLock);
        return size_t(_map.size());
    }
    size_t Erase(cf_const K & key)
    {
        LockGuard < RWMutexType > lock(_writeLock);
        return _map.erase(key);
    }
    cf_void Erase(IteratorType _b, IteratorType _e)
    {
        LockGuard < RWMutexType > lock(_writeLock);
        _map.erase(_b, _e);
    }

    // Insert or update.
    InsertResultType Insert(cf_const ElementType & pair)
    {
        LockGuard <RWMutexType> lock(_writeLock);
        return _map.insert(pair);
    }

    // Only insert , never update.
    V OnlyInsert(cf_const K & key)
    {
        typedef typename PointerTraits<V>::TraitsValueType VT;
        LockGuard < RWMutexType > lock(_writeLock);
        ConstIteratorType it = _map.find(key);
        if (it == _map.end())
        {
            _map.insert(std::make_pair(key, new VT()));
            it = _map.find(key);

        }
        return it->second;
    }

private:
    Raw_RWMutex _rawRWLock;
    RWMutexType _readLock;
    RWMutexType _writeLock;
    MapType _map;
};

template <typename KeyType, typename ValueType,
         typename Raw_RWMutex=RawPthreadRWMutex /* or FakeRWMutex/RawFileRWMutex */>
struct TYPEMap
{
    typedef RawMap < std::map< KeyType,ValueType >,KeyType,ValueType,Raw_RWMutex >
    Map;
};


template <typename MapType, typename IteratorType = typename MapType::iterator>
class MapSmartManager
{
public:

    MapSmartManager()
    {
    }
    ~MapSmartManager()
    {
        for (IteratorType it=_socketConnectionMap.begin();
             it!=_socketConnectionMap.end(); ++it)
        {
            delete it->second;
            it->second = NULL;
        }
    }
    MapType * operator-> ()
    {
        return & _socketConnectionMap;
    }
private:
    MapType _socketConnectionMap;
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_CONCURRENCY_MAP_H_
