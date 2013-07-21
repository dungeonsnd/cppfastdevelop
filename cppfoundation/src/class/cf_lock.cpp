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

#include "cppfoundation/cf_lock.hpp"

namespace cf
{

class PthreadMutexAttrGuard : public NonCopyable
{
public:
    PthreadMutexAttrGuard(pthread_mutexattr_t & Attr):_Attr(Attr)
    {
        cf_int rt =pthread_mutexattr_init(&_Attr);
        if (0!=rt)
            _THROW_FMT(SyscallExecuteError,
                       "Failed to execute pthread_mutexattr_init , rt=%d !",rt)
        }
    ~PthreadMutexAttrGuard()
    {
        cf_int rt =pthread_mutexattr_destroy(&_Attr);
        if (0!=rt)
        {
        }
    }
private:
    pthread_mutexattr_t & _Attr;
};

PthreadMutex::PthreadMutex()
{
    cf_int rt =pthread_mutex_init(&_mutex, NULL);
    if ( 0!=rt )
        _THROW_FMT(SyscallExecuteError,
                   "Failed to execute pthread_mutex_init , rt=%d !",rt)
    }
PthreadMutex::~PthreadMutex()
{
    if ( 0!=pthread_mutex_destroy(&_mutex) )
    {
        // Don't throw exception in destructor! Maybe can write log here!
    }
}
cf_void PthreadMutex::Lock() cf_const
{
    cf_int rt =pthread_mutex_lock(&_mutex);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_mutex_lock , rt=%d !",rt)
    }
cf_void PthreadMutex::UnLock() cf_const
{
    cf_int rt =pthread_mutex_unlock(&_mutex);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_mutex_unlock , rt=%d !",rt)
    }
bool PthreadMutex::TryLock() cf_const
{
    cf_int rt =pthread_mutex_trylock(&_mutex);
    if (0!=rt)
    {
        if(EBUSY==rt)
            return false;
        else
        {
            _THROW_FMT(SyscallExecuteError,
            "Failed to execute pthread_mutex_trylock , rt=%d !",rt);
        }
    }
    return true;
}

pthread_mutex_t & PthreadMutex::GetMutex()
{
    return _mutex;
}


RawPthreadMutex::RawPthreadMutex(pthread_mutex_t * m)
    :_mutex(m)
{

}

RawPthreadMutex::~RawPthreadMutex()
{

}
cf_void RawPthreadMutex::Lock() cf_const
{
    cf_int rt =pthread_mutex_lock(_mutex);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_mutex_lock , rt=%d !",rt)
    }

cf_void RawPthreadMutex::UnLock() cf_const
{
    cf_int rt =pthread_mutex_unlock(_mutex);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_mutex_unlock , rt=%d !",rt)
    }
bool RawPthreadMutex::TryLock() cf_const
{
    cf_int rt =pthread_mutex_trylock(_mutex);
    if (0!=rt)
    {
        if(EBUSY==rt)
            return false;
        else
        {
            _THROW_FMT(SyscallExecuteError,
            "Failed to execute pthread_mutex_trylock , rt=%d !",rt);
        }
    }
    return true;
}


PosixSemaphore::PosixSemaphore(cf_const std::string & name, mode_t mode,
                               bool autoClose):
    _name(name),_autoClose(autoClose)
{
    this->open(name.c_str(), mode);
}
cf_void PosixSemaphore::open(cf_const std::string & name, mode_t mode,
                             cf_uint defaultValue)
{
    if ( SEM_FAILED==(_sem=cf_sem_open(name.c_str(), lockdefs::FLAG_CREATE,
                                       mode, defaultValue)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_sem_open !")
    }

cf_int PosixSemaphore::GetValue() cf_const
{
    cf_int val;
    cf_sem_getvalue(_sem, &val);
    return val;
}


cf_void PosixSemaphore::Lock() cf_const
{
    cf_sem_wait(_sem);
}


bool PosixSemaphore::TryLock() cf_const
{
    if (0 != cf_sem_trywait(_sem))
    {
        if(EAGAIN==errno)
            return false;
        else
        {
            _THROW(SyscallExecuteError, "Failed to execute cf_sem_trywait !");
        }
    }
    return true;
}

cf_void PosixSemaphore::UnLock() cf_const
{
    cf_sem_post(_sem);
}

cf_void PosixSemaphore::Unlink()
{
    if (0 != cf_sem_unlink(_name.c_str()))
        _THROW(SyscallExecuteError, "Failed to execute cf_sem_unlink !")
    }

cf_void PosixSemaphore::Close()
{
    if (0 != cf_sem_close(_sem))
        _THROW(SyscallExecuteError, "Failed to execute cf_sem_close !")
    }
PosixSemaphore::~PosixSemaphore()
{
    if(_autoClose)
        Close();
}


RawPthreadRWMutex::RawPthreadRWMutex()
{
    cf_int rt =pthread_rwlock_init(&_lock, NULL);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError,
                   "Failed to execute pthread_rwlock_init , rt=%d !",rt)
    }
RawPthreadRWMutex::~RawPthreadRWMutex()
{
    cf_int rt =pthread_rwlock_destroy(&_lock);
    if (0!=rt)
    {
        // Don't throw exception in destructor! Maybe can write log here!
    }
}

cf_void RawPthreadRWMutex::ReadLock() cf_const
{
    cf_int rt =pthread_rwlock_rdlock(&_lock);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_rwlock_rdlock , rt=%d !",rt)
    }
bool RawPthreadRWMutex::TryReadLock() cf_const
{
    cf_int rt =pthread_rwlock_tryrdlock(&_lock);
    if (0!=rt)
    {
        if(EBUSY==rt)
            return false;
        else
        {
            _THROW_FMT(SyscallExecuteError,
            "Failed to execute pthread_rwlock_tryrdlock , rt=%d !",rt);
        }
    }
    return true;
}

cf_void RawPthreadRWMutex::WriteLock() cf_const
{
    cf_int rt =pthread_rwlock_wrlock(&_lock);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_rwlock_wrlock , rt=%d !",rt)
    }
bool RawPthreadRWMutex::TryWriteLock() cf_const
{
    cf_int rt =pthread_rwlock_trywrlock(&_lock);
    if (0!=rt)
    {
        if(EBUSY==rt)
            return false;
        else
        {
            _THROW_FMT(SyscallExecuteError,
            "Failed to execute pthread_rwlock_trywrlock , rt=%d !",rt);
        }
    }
    return true;
}

cf_void RawPthreadRWMutex::UnLock() cf_const
{
    cf_int rt =pthread_rwlock_unlock(&_lock);
    if (0!=rt)
        _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_rwlock_unlock , rt=%d !",rt)
    }


RawFileRWMutex::RawFileRWMutex(cf_const std::string & file)
    :_fd(cf_open(file.c_str(), lockdefs::FLAG_CREATE, lockdefs::MODE_DEFAULT))
{
    if (_fd == -1)
        _THROW(SyscallExecuteError, "Failed to execute cf_open !")
    }

cf_void RawFileRWMutex::ReadLock()
{
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if(-1==cf_fcntl(_fd, F_SETLKW, &lock))
        _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !")
    }

bool RawFileRWMutex::TryReadLock()
{
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    cf_int rt =cf_fcntl(_fd, F_SETLK, &lock);
    if(-1==rt)
    {
        if(EACCES==rt || EAGAIN==rt)
            return false;
        else
        {
            _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !")
        }
    }
    return true;
}
cf_void RawFileRWMutex::WriteLock()
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if(-1==cf_fcntl(_fd, F_SETLKW, &lock))
        _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !");
}
bool RawFileRWMutex::TryWriteLock()
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    cf_int rt =cf_fcntl(_fd, F_SETLK, &lock);
    if(-1==rt)
    {
        if(EACCES==rt || EAGAIN==rt)
        {
            return false;
        }
        else
        {
            _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !");
        }
    }
    return true;
}
cf_void RawFileRWMutex::UnLock()
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if(-1==cf_fcntl(_fd, F_SETLK, &lock))
        _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !")
    }
bool RawFileRWMutex::IsLock()
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    cf_int rt = cf_fcntl(_fd, F_GETLK, &lock);
    bool locked =true;
    if (-1==rt)
        _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !")
        else
        {
            if(F_UNLCK == lock.l_type)
                locked =false;
        }
    return locked;
}
bool RawFileRWMutex::IsLock(pid_t  lockedpid)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = -1;
    cf_int rt = cf_fcntl(_fd, F_GETLK, &lock);
    bool locked =true;
    if (-1==rt)
        _THROW(SyscallExecuteError, "Failed to execute cf_fcntl !")
        else
        {
            if(F_UNLCK == lock.l_type)
                locked =false;
            else
                lockedpid =lock.l_pid;
        }
    return locked;
}
RawFileRWMutex::~RawFileRWMutex()
{
    cf_close(_fd);
    _fd = -1;
}


} // namespace cf
