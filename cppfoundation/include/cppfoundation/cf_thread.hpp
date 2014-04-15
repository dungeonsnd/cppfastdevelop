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

#ifndef _HEADER_FILE_CFD_CF_THREAD_HPP_
#define _HEADER_FILE_CFD_CF_THREAD_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

namespace threaddefs
{
enum
{
    DEFAULT_STACKSIZE =1024*1024*2
};

enum ThreadStatus
{
    STATUS_NEW =0 ,
    STATUS_UNBLOCKED ,
    STATUS_BLOCKED ,
    STATUS_WILLEXIT ,
    STATUS_EXIT
};
} // namespace lockdefs

class Runnable:NonCopyable
{
public:
    Runnable() {}
    virtual ~Runnable() {}
    virtual cf_pvoid Run(cf_void *) =0;
    virtual cf_pvoid Initialize(cf_pvoid)
    {
        return NULL;
    }
    virtual cf_pvoid Finalize(cf_pvoid)
    {
        return NULL;
    }
};

class Thread: public NonCopyable
{
public:
    Thread(Runnable *, cf_int id=-1);
    ~Thread() {}
    cf_void SetStackSize(size_t) cf_const;
    cf_void Start();

    threaddefs::ThreadStatus GetStatus()
    {
        return _status;
    }
    cf_void SetStatus(threaddefs::ThreadStatus status)
    {
        _status =status;
    }
    cf_const threaddefs::ThreadStatus & GeStatus() cf_const
    {
        return _status;
    }
    cf_void SetName(cf_const std::string & name)
    {
        _name = name;
    }
    cf_const std::string & GetName() cf_const
    {
        return _name;
    }
    cf_int Getid() cf_const
    {
        return _id;
    }
    cf_const pthread_t & Gettid() cf_const
    {
        return  _tid;
    }

    cf_void Join(cf_ppvoid) cf_const;
    cf_void Detach() cf_const;
    cf_void Exit()
    {
        pthread_exit(NULL);
        SetStatus(threaddefs::STATUS_EXIT);
    }
private:
    static cf_pvoid ThreadProc(cf_pvoid);

    threaddefs::ThreadStatus _status;
    mutable std::string _name;
    cf_int _id;
    mutable pthread_t _tid;

    Runnable * _pRun;
    mutable pthread_attr_t _attr;
};


class ThreadPool: public NonCopyable
{
public:
    ThreadPool(Runnable * task, size_t poolSize,
               size_t stacksize =threaddefs::DEFAULT_STACKSIZE, bool joinable = true);
    ~ThreadPool() {}
    cf_void Start();
    cf_void Join();
private:
    Runnable * _task;
    size_t _poolSize;
    size_t _stacksize;
    bool _joinable;
    bool _joined;

    typedef std::vector < Thread * > ThreadsPoolType;
    typedef ThreadsPoolType::iterator ThreadsPoolIteratorType;
    ThreadsPoolType _threads;
};


class TaskPool: public NonCopyable
{
public:
    TaskPool(size_t num, size_t stack_size =threaddefs::DEFAULT_STACKSIZE)
    {
        _THROW(UnimplementedError, "Unsupported temporarily !");
    }
    ~TaskPool();
    cf_void AddTask(Runnable * task);
    cf_void StartAll();
    cf_void TerminateAll();
private:

    typedef std::list < Thread * > ThreadsPoolType;
    typedef ThreadsPoolType::iterator ThreadsPoolIteratorType;
    ThreadsPoolType _threadsFree;
    ThreadsPoolType _threadsBusy;
};

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_THREAD_HPP_
