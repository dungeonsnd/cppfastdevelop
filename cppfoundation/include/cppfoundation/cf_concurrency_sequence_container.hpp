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

#ifndef _HEADER_FILE_CFD_CF_CONCURRENCY_SEQUENCE_CONTAINER_H_
#define _HEADER_FILE_CFD_CF_CONCURRENCY_SEQUENCE_CONTAINER_H_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_utility.hpp" // type traits.
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"
#include "cppfoundation/cf_condition.hpp"

namespace cf
{

template <typename SequenceContainerType,typename ElementType, typename Raw_RWMutex>
class CRawSequenceContainer: public NonCopyable
{
public:
    typedef typename SequenceContainerType::iterator IteratorType;

    typedef RWMutex < Raw_RWMutex > RWMutexType;

    CRawSequenceContainer() :
        _readLock(_rawRWLock, lockdefs::READ),
        _writeLock(_rawRWLock, lockdefs::WRITE)
    {
    }
    ~CRawSequenceContainer()
    {
    }

    size_t Size() cf_const
    {
        LockGuard<Raw_RWMutex> lock(_readLock);
        return _container.size();
    }
    bool IsEmpty () cf_const
    {
        LockGuard<Raw_RWMutex> lock(_readLock);
        return _container.empty();
    }

    cf_void PushBack ( cf_const ElementType & x )
    {
        LockGuard<Raw_RWMutex> lock(_writeLock);
        _container.push_back(x);
    }
    cf_void PopBack()
    {
        LockGuard<Raw_RWMutex> lock(_writeLock);
        _container.pop_back();
    }
    IteratorType Insert ( IteratorType position, cf_const ElementType & x )
    {
        LockGuard<Raw_RWMutex> lock(_writeLock);
        return _container.insert(x);
    }
    IteratorType Erase ( IteratorType position )
    {
        LockGuard<Raw_RWMutex> lock(_writeLock);
        return _container.erase(position);
    }
    IteratorType FindAndErase ( cf_const ElementType & x,bool & found )
    {
        LockGuard<Raw_RWMutex> lock(_writeLock);
        IteratorType position =LoopFind (x);
        if(position!=_container.end())
        {
            found =true;
            return _container.erase(position);
        }
        else
        {
            found =false;
            return _container.end();
        }
    }
    cf_void Clear ()
    {
        LockGuard<Raw_RWMutex> lock(_writeLock);
        _container.clear();
    }

private:
    IteratorType LoopFind ( cf_const ElementType & x )
    {
        for(IteratorType it=_container.begin(); it!=_container.end(); it++)
            if(*it==x)
                return it;
        return _container.end();
    }

    Raw_RWMutex _rawRWLock;
    RWMutexType _readLock;
    RWMutexType _writeLock;
    SequenceContainerType _container;
};

template <typename ElementType,
         typename Raw_RWMutex=RawPthreadRWMutex /* or FakeRWMutex/RawFileRWMutex */>
struct TYPESequenceContainer
{
    typedef CRawSequenceContainer
    < std::vector<ElementType> , ElementType, Raw_RWMutex > Vector;
    typedef CRawSequenceContainer
    < std::list<ElementType> , ElementType, Raw_RWMutex > List;
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_CONCURRENCY_SEQUENCE_CONTAINER_H_
