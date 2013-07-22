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

#ifndef _HEADER_FILE_CFD_CF_LOCK_HPP_
#define _HEADER_FILE_CFD_CF_LOCK_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_memory.hpp"

namespace cf
{

namespace lockdefs
{
enum
{
    MODE_DEFAULT = (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH)
};
enum
{
    FLAG_CREATE_EXCL = (O_RDWR|O_CREAT|O_EXCL),
    FLAG_CREATE = (O_RDWR|O_CREAT),
    FLAG_RDWR = O_RDWR,
    FLAG_RD = O_RDONLY,
    FLAG_WR = O_WRONLY
};
enum LOCK_TYPE
{
    READ,
    WRITE
};
} // namespace lockdefs


// Used to add no-operation.
class FakeMutex : public NonCopyable
{
public:
    FakeMutex() {};
    ~FakeMutex() {};
    cf_void Lock() cf_const {}
    cf_void UnLock() cf_const {}
    bool TryLock() cf_const {return true;}
};
// Used to add no-operation.
class FakeRWMutex : public NonCopyable
{
public:
    FakeRWMutex() {};
    ~FakeRWMutex() {};
    cf_void ReadLock() cf_const {}
    bool TryReadLock() cf_const {return true;}
    cf_void WriteLock() cf_const {}
    cf_void TryWriteLock() cf_const {}
    bool UnLock() cf_const {return true;}
};


class PthreadMutex : public NonCopyable
{
public:
    PthreadMutex();
    ~PthreadMutex();
    cf_void Lock() cf_const;
    cf_void UnLock() cf_const;
    bool TryLock() cf_const;
    pthread_mutex_t & GetMutex();
private:
    mutable pthread_mutex_t _mutex;
};

class RawPthreadMutex : public NonCopyable
{
public:
    RawPthreadMutex(pthread_mutex_t * m);
    cf_void Lock() cf_const;
    cf_void UnLock() cf_const;
    bool TryLock() cf_const;
    pthread_mutex_t & GetMutex()
    {
        return *_mutex;
    }
    ~RawPthreadMutex();
private:
    pthread_mutex_t * _mutex;
};


class PosixSemaphore : public NonCopyable
{
public:

    PosixSemaphore(cf_const std::string & name,
                   mode_t mode = lockdefs::MODE_DEFAULT,
                   bool autoClose=true);
    ~PosixSemaphore();
    cf_int GetValue() cf_const;
    cf_void Lock() cf_const;
    bool TryLock() cf_const;
    cf_void UnLock() cf_const;
    cf_void Unlink();
    cf_void Close();
private:
    cf_void open(cf_const std::string & name, mode_t mode,
                 cf_uint defaultValue = 1);

    sem_t * _sem;
    std::string _name;
    bool _autoClose;
};


class RawPthreadRWMutex : public NonCopyable
{
public:
    RawPthreadRWMutex();
    ~RawPthreadRWMutex();
    cf_void ReadLock() cf_const;
    bool TryReadLock() cf_const;
    cf_void WriteLock() cf_const;
    bool TryWriteLock() cf_const;
    cf_void UnLock() cf_const;
private:
    mutable pthread_rwlock_t _lock;
};

class RawFileRWMutex : public NonCopyable
{
public:
    RawFileRWMutex(cf_const std::string & file);
    ~RawFileRWMutex();
    cf_void ReadLock();
    bool TryReadLock();
    cf_void WriteLock();
    bool TryWriteLock();
    cf_void UnLock();
    bool IsLock();
    bool IsLock(pid_t  lockedpid);
    cf_int GetFd()
    {
        if(_fd<=0)
            _THROW(ValueError, "_fd<=0 !");
        return _fd;
    }
private:
    cf_int _fd;
};

template < typename T_RawRWMutex =RawPthreadRWMutex >
class RWMutex : public NonCopyable
{
public:
    RWMutex(T_RawRWMutex & lock, lockdefs::LOCK_TYPE type=lockdefs::WRITE)
        :_lock(lock),_type(type)
    {
    }
    ~RWMutex()
    {
    }
    cf_void Lock() cf_const
    {
        if (_type == lockdefs::READ)
        {
            _lock.ReadLock();
        }
        else if (_type == lockdefs::WRITE)
        {
            _lock.WriteLock();
        }
        else
        {
            _THROW_FMT(RuntimeWarning, "_type{%d} unexpected !",_type);
        }
    }
    cf_void UnLock() cf_const
    {
        _lock.UnLock();
    }
    bool TryLock() cf_const
    {
        if (_type == lockdefs::READ)
        {
            return _lock.TryReadLock();
        }
        else if (_type == lockdefs::WRITE)
        {
            return _lock.TryWriteLock();
        }
        else
        {
            _THROW_FMT(RuntimeWarning, "_type{%d} unexpected !",_type);
        }
        return true;
    }
protected:
    T_RawRWMutex & _lock;
    lockdefs::LOCK_TYPE _type;
};


class FileWriteLock : public RWMutex <RawFileRWMutex>
{
public:
    static RawFileRWMutex & NewWithException(cf_const std::string & fileName)
    {
        CF_NEWOBJ(p, RawFileRWMutex, fileName);
        if(NULL==p)
            _THROW(AllocateMemoryError, "Allocate memory failed !");
        return * p;
    }

    FileWriteLock(cf_const std::string & fileName)
        : RWMutex<RawFileRWMutex>(NewWithException(fileName)),
          _fileLock((RawFileRWMutex *)&_lock)
    {
    }
    ~FileWriteLock()
    {
    }
    bool IsLocked(cf_const pid_t & pid) cf_const
    {
        return _fileLock->IsLock(pid);
    }
    bool IsLocked() cf_const
    {
        return _fileLock->IsLock();
    }

private:
    std::shared_ptr<RawFileRWMutex> _fileLock;
};

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_LOCK_HPP_
