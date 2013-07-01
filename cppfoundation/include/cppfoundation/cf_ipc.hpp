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

#ifndef _HEADER_FILE_CFD_CF_IPC_HPP_
#define _HEADER_FILE_CFD_CF_IPC_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

namespace ipcdefs
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

enum
{
    PROT_DEFAULT=PROT_READ|PROT_WRITE
};
enum
{
    MMAPFLAG_DEFAULT=MAP_SHARED
};
} // namespace ipcdefs

template < typename PipeType >
class Pipe : public NonCopyable
{
public:

    Pipe(cf_int type=SOCK_STREAM,bool autoClose=true):
        _pipe(type,autoClose)
    {
    }
    ~Pipe()
    {
    }

    inline cf_int Get0() cf_const
    {
        return _pipe.Get0();
    }
    inline cf_int Get1() cf_const
    {
        return _pipe.Get1();
    }
    inline cf_int operator[](cf_int index)
    {
        return _pipe[index];
    }
private:
    PipeType _pipe;
};

class SocketpairPipe : public NonCopyable
{
public:
    enum
    {
        SOCKETPROTOCOL_DEFAULT=0
    };

    SocketpairPipe(cf_int type=SOCK_STREAM,bool autoClose=true):
        _autoClose(autoClose)
    {
        if(0!=cf_socketpair(AF_LOCAL, type, SOCKETPROTOCOL_DEFAULT, _fds) )
            _THROW(SyscallExecuteError, "Failed to execute cf_socketpair !");
    }
    ~SocketpairPipe()
    {
        if(_autoClose)
        {
            int rt =cf_close(_fds[0]);
            if(0!=rt)
                ;
            rt =cf_close(_fds[1]);
            if(0!=rt)
                ;
        }
    }

    inline cf_int Get0() cf_const
    {
        return _fds[0];
    }
    inline cf_int Get1() cf_const
    {
        return _fds[1];
    }
    inline cf_int operator[](cf_int index)
    {
        if(0==index)
            return Get0();
        else if(1==index)
            return Get1();
        else
        {
            _THROW(ValueError, "Index error !");
            return NULL;
        }
    }
private:
    cf_int _fds[2];
    bool _autoClose;
};

template < typename MsgQType >
class MsgQ : public NonCopyable
{
public:
    MsgQ()
    {
        _THROW(UnimplementedError, "Unsupported temporarily !");
    }
    ~MsgQ()
    {
    }
    cf_void Send(cf_cpstr msg_ptr,size_t msg_len)
    {
    }
    ssize_t Recv(cf_pvoid buf, size_t len)
    {
        return 0;
    }
private:
    MsgQType _msgq;
};

class PosixMsgQ : public NonCopyable
{
public:
    PosixMsgQ(cf_cpstr name,cf_int oflag = ipcdefs::FLAG_CREATE,
              mode_t mode = ipcdefs::MODE_DEFAULT,bool autoUnlink=false,bool autoClose=true);
    ~PosixMsgQ();

    cf_void Getattr(struct mq_attr * attr);
    cf_void Setattr(struct mq_attr * newattr,struct mq_attr * oldattr);
    cf_void Send(cf_cpstr msg_ptr,size_t msg_len, cf_uint msg_prio);
    cf_void Send(cf_cpstr msg_ptr,size_t msg_len, cf_uint msg_prio,
                 cf_const struct timespec * abs_timeout);

    ssize_t Recv(cf_char * msg_ptr,size_t msg_len, cf_uint  * msg_prio);
    ssize_t Recv(cf_char * msg_ptr,size_t msg_len, cf_uint  * msg_prio,
                 cf_const struct timespec * abs_timeout);

    cf_void Notify(cf_const struct sigevent * sevp);
private:
    mqd_t _fd;
    std::string _name;
    bool _autoUnlink;
    bool _autoClose;
};

class SysVMsgQ : public NonCopyable
{
public:
    SysVMsgQ(cf_const std::string & name,
             cf_int flag = IPC_CREAT|ipcdefs::MODE_DEFAULT,bool autoRemove=false);
    ~SysVMsgQ();

    cf_void Send(cf_cpvoid buf, size_t len) cf_const;
    cf_void TrySend(cf_cpvoid buf, size_t len) cf_const;
    ssize_t Recv(cf_pvoid buf, size_t len, cf_long type = 0) cf_const;
    ssize_t TryRecv(cf_pvoid buf, size_t len, cf_long type = 0) cf_const;
private:
    cf_int _msgId;
    std::string _name;
    key_t _key;
    bool _autoRemove;
};


class MemMappedFile : public NonCopyable
{
public:
    MemMappedFile(cf_const std::string & name, size_t size,
                  cf_int prot =ipcdefs::PROT_DEFAULT,
                  bool lock = false,bool autoUnmap =true);
    ~MemMappedFile();
    cf_pvoid Get() cf_const;
    off_t GetSize() cf_const;
    cf_void Msync(size_t fromIndex,size_t len, cf_int flag = MS_ASYNC);
    cf_void Msync(cf_int flag = MS_ASYNC);
    cf_void  Munmap();
private:
    cf_pvoid Mmap(cf_int fd, cf_int prot);

private:
    cf_pvoid _pShm;
    off_t _size;
    bool   _isLocked;
    bool _autoUnmap;
};

class PosixShM : public NonCopyable
{
public:
    PosixShM(cf_cpstr name, size_t size, cf_int oflag = ipcdefs::FLAG_CREATE,
             cf_int prot =ipcdefs::PROT_DEFAULT,
             mode_t mode = ipcdefs::MODE_DEFAULT,
             bool autoUnlink =false,bool autoClose =true);
    ~PosixShM();
    cf_pvoid Get() cf_const;
    cf_const std::string & GetName() cf_const;
    cf_void Ftruncate(off_t length);
    cf_void Fstat(struct stat * buf);
    cf_void Unlink();
    cf_void Close();
private:
    cf_pvoid Mmap(cf_int fd, cf_int prot);

    cf_pvoid _pShm;
    cf_int _fd;
    std::string _name;
    size_t _size;
    bool _autoUnlink;
    bool _autoClose;
};

class SysVShM : public NonCopyable
{
public:
    SysVShM(key_t key, size_t size,
            cf_int shmFlag = IPC_CREAT|ipcdefs::MODE_DEFAULT,
            bool autoRemove =false,bool autoDetach =true);
    ~SysVShM();
    cf_pvoid GetShm() cf_const;
    cf_void  Detach();
    cf_void  Remove();
private:
    key_t  _key;
    cf_int    _shmid;
    cf_pvoid  _pShmAddr;
    bool _autoRemove;
    bool _autoDetach;
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_IPC_HPP_

