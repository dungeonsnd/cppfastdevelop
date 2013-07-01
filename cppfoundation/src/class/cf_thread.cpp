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

#include "cppfoundation/cf_thread.hpp"

namespace cf
{

Thread::Thread(Runnable * run,cf_int id):
    _status(threaddefs::STATUS_NEW),
    _name(typeid(*run).name()),
    _id(id),
    _tid(0),
    _pRun(run)
{
    if (NULL == _pRun)
        _THROW(NullPointerError, "NULL == _pRun !");
    if (0!=cf_pthread_attr_init(&_attr))
        _THROW(SyscallExecuteError, "Failed to execute cf_pthread_attr_init !");
}
cf_void Thread::SetStackSize(size_t size) cf_const
{
    if (0!=cf_pthread_attr_setstacksize(&_attr, size))
        _THROW(SyscallExecuteError, "Failed to execute cf_pthread_attr_setstacksize !");
}
cf_void Thread::Start()
{
    if (0!=cf_pthread_create(&_tid, &_attr, Thread::ThreadProc, (cf_void *)this))
        _THROW(SyscallExecuteError, "Failed to execute cf_pthread_attr_setstacksize !");
    _status =threaddefs::STATUS_UNBLOCKED;
}

cf_void Thread::Join(cf_ppvoid ptr) cf_const
{
    if (0!=cf_pthread_join(_tid, ptr))
        _THROW(SyscallExecuteError, "Failed to execute cf_pthread_join !");
}
cf_void Thread::Detach() cf_const
{
    if (0!=cf_pthread_detach(_tid))
        _THROW(SyscallExecuteError, "Failed to execute cf_pthread_detach !");
}

cf_void * Thread::ThreadProc(cf_void * self)
{
    Thread * _self = (Thread *)self;
    _self->_pRun->Initialize(self);
    cf_void * ret = _self->_pRun->Run(self);
    _self->_pRun->Finalize(self);
    return ret;
}

ThreadPool::ThreadPool(Runnable * task, size_t poolSize, size_t stacksize,
                       bool joinable ):
    _task(task),
    _poolSize(poolSize),
    _stacksize(stacksize),
    _joinable(joinable),
    _joined(false)
{
    _threads.reserve(_poolSize);
    for (size_t i=0; i!=_poolSize; i++)
        _threads.push_back(new Thread(_task, i));
}

cf_void ThreadPool::Start()
{
    for (ThreadsPoolIteratorType it = _threads.begin(); it!=_threads.end(); it++)
    {
        (*it)->SetStackSize(_stacksize);
        if (false==_joinable)
            (*it)->Detach();
        (*it)->Start();
    }
}

cf_void ThreadPool::Join()
{
    if (_joinable && false==_joined)
    {
        for (ThreadsPoolIteratorType it = _threads.begin(); it!=_threads.end(); it++)
            (*it)->Join(NULL);
        _joined = true;
    }
}

}

