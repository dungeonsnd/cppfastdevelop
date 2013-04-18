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

#ifndef _HEADER_FILE_CFD_CF_CONCURRENCY_QUEUE_H_
#define _HEADER_FILE_CFD_CF_CONCURRENCY_QUEUE_H_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_utility.hpp" // type traits.
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"
#include "cppfoundation/cf_condition.hpp"

namespace cf
{

// Queue
DEFINECLASS_CHILD_EXCEPTION(QueueStopToGetException, Info);
DEFINECLASS_CHILD_EXCEPTION(QueueEmptyAndStopToPutException, Info);
DEFINECLASS_CHILD_EXCEPTION(QueueEmptyException, Info);
DEFINECLASS_CHILD_EXCEPTION(QueueStopToPutException, Info);

//RawQueue
template <typename QueueType, typename ElementType, typename MutexType>
class RawQueue: public NonCopyable
{
public :
   RawQueue():
      _stopToPut(false),
      _stopToGet(false),
      _waitCount(0)
   {
   }
   ~RawQueue()
   {
   }

   ElementType Get()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      while (_queue.empty() && false==_stopToPut)
      {
         ++_waitCount;
         _cond.Wait();
      }
      if (_stopToGet)
         _THROW(QueueStopToGetException, "This queue has been stopped to get from it!");
      if (_queue.empty() && _stopToPut)
         _THROW(QueueEmptyAndStopToPutException,
                "This queue is empty and stopped to put!");

      ElementType temp = _queue.front();
      _queue.pop();
      return temp;
   }
   ElementType TryGet()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      if (_stopToGet)
         _THROW(QueueStopToGetException, "This queue has been stopped to get!");
      if ( _queue.empty())
         _THROW(CQueueEmptyException, "This queue is empty!");

      ElementType temp(_queue.front());
      _queue.pop();
      return temp;
   }
   cf_void Put(cf_const ElementType & el)
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      if (_stopToPut)
         _THROW(QueueStopToPutException, "This queue has been stopped to put!");

      _queue.push(el);
      if (_waitCount != 0)
      {
         --_waitCount;
         _cond.Signal();
      }
   }
   cf_void StopPutAndGet()
   {
      _stopToGet = true;
      _stopToPut = true;
      _cond.Broadcast();
   }
   cf_void StopPut()
   {
      _stopToPut = true;
      _cond.Broadcast();
   }
   bool IsEmpty()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      return _queue.empty();
   }
   size_t Size()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      return _queue.size();
   }

private :
   volatile bool _stopToPut;
   volatile bool _stopToGet;
   cf_int _waitCount;
   PthreadCond _cond;
   QueueType _queue;
};

//RawFastFindQueue
template <typename QueueType, typename ElementType, typename SetType, typename MutexType>
class RawFastFindQueue: public NonCopyable
{
public :
   typedef typename SetType::iterator IteratorType;

   RawFastFindQueue():
      _stopToPut(false),
      _stopToGet(false),
      _waitCount(0)
   {
   }
   ~RawFastFindQueue()
   {
   }

   ElementType Get()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      while (_queue.empty() && false==_stopToPut)
      {
         ++_waitCount;
         _cond.Wait();
      }
      if (_stopToGet)
         _THROW(QueueStopToGetException, "This queue has been stopped to get from it!");
      if (_queue.empty() && _stopToPut)
         _THROW(QueueEmptyAndStopToPutException,
                "This queue is empty and stopped to put!");

      ElementType temp = _queue.front();
      _queue.pop();
      SetErase(temp);
      return temp;
   }
   ElementType TryGet()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      if (_stopToGet)
         _THROW(QueueStopToGetException, "This queue has been stopped to get!");
      if ( _queue.empty())
         _THROW(CQueueEmptyException, "This queue is empty!");

      ElementType temp(_queue.front());
      _queue.pop();
      SetErase(temp);
      return temp;
   }
   cf_void Put(cf_const ElementType & el)
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      if (_stopToPut)
         _THROW(QueueStopToPutException, "This queue has been stopped to put!");

      _queue.push(el);
      SetInsert(el);

      if (_waitCount != 0)
      {
         --_waitCount;
         _cond.Signal();
      }
   }

   size_t Find(cf_const ElementType & temp)
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      return _set.count(temp);
   }

   cf_void StopPutAndGet()
   {
      _stopToGet = true;
      _stopToPut = true;
      _cond.Broadcast();
   }
   cf_void StopPut()
   {
      _stopToPut = true;
      _cond.Broadcast();
   }
   bool IsEmpty()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      return _queue.empty();
   }
   size_t Size()
   {
      LockGuard<MutexType> lock(_cond.GetLock());
      return _queue.size();
   }

private :
   cf_void SetErase(cf_const ElementType & temp)
   {
      IteratorType itFind =_set.find(temp);
      if(itFind!=_set.end())
         _set.erase(itFind);
      else
         _THROW(ElementNotFoundInfo, "itFind==__set.end() !");
   }
   cf_void SetInsert(cf_const ElementType & temp)
   {
      _set.insert(temp);
   }

   volatile bool _stopToPut;
   volatile bool _stopToGet;
   cf_int _waitCount;
   PthreadCond _cond;
   QueueType _queue;
   SetType _set;
};

template <typename ElementType,
         typename MutexType =PthreadMutex /* or =FakeMutex */>
struct TYPEQueue
{
   typedef RawQueue<std::queue<ElementType>, ElementType, MutexType > Queue;
   typedef RawFastFindQueue<std::queue<ElementType>, ElementType, std::set<ElementType>, MutexType >
   FastFindQueue;
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_CONCURRENCY_QUEUE_H_
