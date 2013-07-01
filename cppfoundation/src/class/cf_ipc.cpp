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

#include "cppfoundation/cf_ipc.hpp"

namespace cf
{


PosixMsgQ::PosixMsgQ(cf_cpstr name,cf_int oflag, mode_t mode,
                     bool autoUnlink,bool autoClose):
    _name(name),_autoUnlink(autoUnlink),_autoClose(autoClose)
{
    if ( -1==(_fd=cf_mq_open(name,oflag,mode,NULL)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_mq_open !");
}
PosixMsgQ::~PosixMsgQ()
{
    if(_autoUnlink)
    {
        if(0!=cf_mq_unlink(_name.c_str()))
            ;//_THROW(SyscallExecuteError, "Failed to execute cf_mq_unlink !")
    }
    if(_autoClose)
    {
        if(0!=cf_mq_close(_fd))
            ;//_THROW(SyscallExecuteError, "Failed to execute cf_mq_close !")
    }
}

cf_void PosixMsgQ::Getattr(struct mq_attr * attr)
{
    if(0!=cf_mq_getattr(_fd,attr))
        _THROW(SyscallExecuteError, "Failed to execute mq_getattr !");
}

cf_void PosixMsgQ::Setattr(struct mq_attr * newattr,struct mq_attr * oldattr)
{
    if(0!=cf_mq_setattr(_fd,newattr,oldattr))
        _THROW(SyscallExecuteError, "Failed to execute mq_setattr !");
}

cf_void PosixMsgQ::Send(cf_cpstr msg_ptr,size_t msg_len, cf_uint msg_prio)
{
    if(0!=cf_mq_send(_fd, msg_ptr,msg_len, msg_prio))
        _THROW(SyscallExecuteError, "Failed to execute cf_mq_send !");
}
cf_void PosixMsgQ::Send(cf_cpstr msg_ptr,size_t msg_len, cf_uint msg_prio,
                        cf_const struct timespec * abs_timeout)
{
    if(0!=cf_mq_timedsend(_fd, msg_ptr,msg_len, msg_prio,abs_timeout))
        _THROW(SyscallExecuteError, "Failed to execute cf_mq_timedsend !");
}

ssize_t PosixMsgQ::Recv(cf_char * msg_ptr,size_t msg_len, cf_uint  * msg_prio)
{
    ssize_t rt =0;
    if( -1==(rt=cf_mq_receive(_fd, msg_ptr,msg_len, msg_prio)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_mq_receive !");
    return rt;
}
ssize_t PosixMsgQ::Recv(cf_char * msg_ptr,size_t msg_len, cf_uint  * msg_prio,
                        cf_const struct timespec * abs_timeout)
{
    ssize_t rt =0;
    if( -1==(rt=cf_mq_timedreceive(_fd, msg_ptr,msg_len, msg_prio,abs_timeout)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_mq_timedreceive !");
    return rt;
}

cf_void PosixMsgQ::Notify(cf_const struct sigevent * sevp)
{
    if( 0!=cf_mq_notify(_fd,sevp) )
        _THROW(SyscallExecuteError, "Failed to execute cf_mq_notify !");
}


SysVMsgQ::SysVMsgQ(cf_const std::string & name, cf_int flag,bool autoRemove):
    _name(name),_key(cf_ftok(_name.c_str(),1)),
    _autoRemove(autoRemove)
{
    if(_key == -1)
        _THROW(SyscallExecuteError, "Failed to execute cf_ftok !");
    _msgId = cf_msgget(_key, flag);
    if(_msgId == -1)
        _THROW(SyscallExecuteError, "Failed to execute cf_msgget !");
}
SysVMsgQ::~SysVMsgQ()
{
    if(_autoRemove)
    {
        if(0!=cf_msgctl(_msgId, IPC_RMID, NULL))
            ;//_THROW(SyscallExecuteError, "Failed to execute cf_msgctl !")
    }
}
cf_void SysVMsgQ::Send(cf_cpvoid buf, size_t len) cf_const
{
    if(0!=cf_msgsnd(_msgId, buf, len-sizeof(cf_long), 0))
        _THROW(SyscallExecuteError, "Failed to execute cf_msgsnd !");
}
cf_void SysVMsgQ::TrySend(cf_cpvoid buf, size_t len) cf_const
{
    if(0!=cf_msgsnd(_msgId, buf, len-sizeof(cf_long), IPC_NOWAIT))
        _THROW(SyscallExecuteError, "Failed to execute cf_msgsnd !");
}
ssize_t SysVMsgQ::Recv(cf_pvoid buf, size_t len, cf_long type) cf_const
{
    ssize_t rt =0;
    if( -1==(rt=cf_msgrcv(_msgId, buf, len-sizeof(cf_long), type, 0)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_msgrcv !");
    return rt;
}
ssize_t SysVMsgQ::TryRecv(cf_pvoid buf, size_t len, cf_long type) cf_const
{
    ssize_t rt =0;
    if( -1==(rt=cf_msgrcv(_msgId, buf, len-sizeof(cf_long), type, IPC_NOWAIT)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_msgrcv !");
    return rt;
}


MemMappedFile::MemMappedFile(cf_const std::string & name, size_t size,
                             cf_int prot, bool lock,bool autoUnmap)
    :_pShm(NULL),
     _size(size),
     _isLocked(lock),
     _autoUnmap(autoUnmap)
{
    if (size<=0)
        _THROW(ValueError, "size !");

    cf_int fd =cf_open(name.c_str(), ipcdefs::FLAG_CREATE, ipcdefs::MODE_DEFAULT);
    if(0!=fd)
        _THROW(SyscallExecuteError, "Failed to execute cf_open !");

    struct stat st_buf = {0};
    if(0!=cf_fstat(fd,&st_buf))
        _THROW(SyscallExecuteError, "Failed to execute cf_fstat !");

    if (st_buf.st_size < _size)
    {
        if(0!=cf_ftruncate(fd,size))
            _THROW(SyscallExecuteError, "Failed to execute cf_ftruncate !");
    }

    if (size == 0)
        _size = st_buf.st_size;
    _pShm = Mmap(fd, prot);
    if (lock && 0!=cf_mlock(_pShm, _size))
        _THROW(SyscallExecuteError, "Failed to execute cf_mlock !");
}
MemMappedFile::~MemMappedFile()
{
    if (_isLocked && 0!=cf_munlock(_pShm, _size))
        ;//_THROW(SyscallExecuteError, "Failed to execute cf_munlock !")
    if(_autoUnmap)
        Munmap();
}
cf_void MemMappedFile::Munmap()
{
    if (_pShm == MAP_FAILED)
        return;
    if (0!=cf_munmap(_pShm, _size))
        ;//_THROW(SyscallExecuteError, "Failed to execute cf_munmap !")
}

cf_pvoid MemMappedFile::Mmap(cf_int fd, cf_int prot)
{
    cf_pvoid pShm = NULL;
    pShm=cf_mmap(NULL,_size, prot,ipcdefs::MMAPFLAG_DEFAULT, fd, 0);
    if ( MAP_FAILED==pShm )
        _THROW(SyscallExecuteError, "Failed to execute cf_mmap !");
    return pShm;
}

cf_void MemMappedFile::Msync(size_t fromIndex, size_t len, cf_int flag)
{
    if(fromIndex+len>size_t(_size))
        _THROW_FMT(ValueError, "fromIndex{%lld}+len{%lld}>size_t(_size) {%lld}!",
                   (cf_llong)fromIndex,(cf_llong)len,(cf_llong)_size);
    cf_char * p =(cf_char *)_pShm;
    p +=fromIndex;
    if (  0!=cf_msync( (cf_pvoid)p, len, flag )  )
        _THROW(SyscallExecuteError, "Failed to execute cf_msync !");
}

cf_void MemMappedFile::Msync(cf_int flag)
{
    Msync(0,_size, flag);
}

cf_pvoid MemMappedFile::Get() cf_const
{
    if (_pShm == MAP_FAILED)
        _THROW(ValueError, "_pShm == MAP_FAILED !");
    return _pShm;
}
off_t MemMappedFile::GetSize() cf_const
{
    return _size;
}

PosixShM::PosixShM(cf_cpstr name, size_t size, cf_int oflag, cf_int prot ,
                   mode_t mode,
                   bool autoUnlink,bool autoClose):
    _name(name),
    _size(size),
    _autoUnlink(autoUnlink),
    _autoClose(autoClose)
{
    if (size<=0)
        _THROW(ValueError, "size !")
        if( -1==(_fd=cf_shm_open(_name.c_str(), oflag, mode)) )
            _THROW(SyscallExecuteError, "Failed to execute cf_shm_open !");
    Ftruncate(_size);
    _pShm =Mmap(_fd, prot);
}
PosixShM::~PosixShM()
{
    if(_autoUnlink)
    {
        if( 0!=cf_shm_unlink(_name.c_str()) )
            ;//_THROW(SyscallExecuteError, "Failed to execute cf_shm_unlink !")
    }
    if(_autoClose)
    {
        if( 0!=cf_close(_fd) )
            ;//_THROW(SyscallExecuteError, "Failed to execute cf_close !")
    }
}

cf_pvoid PosixShM::Mmap(cf_int fd, cf_int prot)
{
    cf_pvoid pShm = cf_mmap(NULL,_size, prot,
                            ipcdefs::MMAPFLAG_DEFAULT, fd, 0);
    if ( MAP_FAILED==pShm )
        _THROW(SyscallExecuteError, "Failed to execute cf_mmap !");
    return pShm;
}

cf_pvoid PosixShM::Get() cf_const
{
    if (_pShm == MAP_FAILED)
        _THROW(ValueError, "_pShm == MAP_FAILED !");
    return _pShm;
}
cf_void PosixShM::Ftruncate(off_t length)
{
    if( 0!=cf_ftruncate(_fd, length) )
        _THROW(SyscallExecuteError, "Failed to execute cf_ftruncate !");
    _size =length;
}

cf_void PosixShM::Fstat(struct stat * buf)
{
    if( 0!=cf_fstat(_fd, buf) )
        _THROW(SyscallExecuteError, "Failed to execute cf_fstat !");
}

cf_const std::string & PosixShM::GetName() cf_const
{
    return _name;
}
cf_void PosixShM::Unlink()
{
    if( 0!=cf_shm_unlink(_name.c_str()) )
        _THROW(SyscallExecuteError, "Failed to execute cf_shm_unlink !");
}
cf_void PosixShM::Close()
{
    if( 0!=cf_close(_fd) )
        _THROW(SyscallExecuteError, "Failed to execute cf_close !");
}


SysVShM::SysVShM(key_t key, size_t size, cf_int shmFlag,bool autoRemove,
                 bool autoDetach)
    :_key(key),
     _shmid(-1),
     _pShmAddr(NULL),
     _autoRemove(autoRemove),
     _autoDetach(autoDetach)
{
    if ( -1==(_shmid = cf_shmget(_key, size, shmFlag)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_shmget !");
    if ( (cf_void *)(-1)==(_pShmAddr=cf_shmat(_shmid, NULL, 0)) )
        _THROW(SyscallExecuteError, "Failed to execute cf_shmat !");
}

SysVShM::~SysVShM()
{
    if(_autoDetach)
    {
        if ((_pShmAddr != NULL) && 0!=(cf_shmdt(_pShmAddr)))
        {
            ;//_THROW_(CSystemInvokeException, "Failed to invoke cf_shmdt !");
            _pShmAddr = NULL;
        }
    }
    if(_autoRemove)
    {
        if(-1==cf_shmctl(_shmid, IPC_RMID, NULL))
            ;//_THROW_(CSystemInvokeException, "Failed to invoke cf_shmctl !");
    }
}

cf_pvoid SysVShM::GetShm() cf_const
{
    if (NULL == _pShmAddr)
        _THROW(ValueError, "NULL == _pShmAddr !");
    if ( (cf_void *)(-1)==_pShmAddr )
        _THROW(ValueError, "(cf_void*)(-1)==_pShmAddr !");
    return _pShmAddr;
}

cf_void  SysVShM::Detach()
{
    if ((_pShmAddr != NULL) && 0!=(cf_shmdt(_pShmAddr)))
        _THROW(SyscallExecuteError, "Failed to execute cf_shmdt !");
    _pShmAddr = NULL;
}

cf_void  SysVShM::Remove()
{
    if(-1==cf_shmctl(_shmid, IPC_RMID, NULL))
        _THROW(SyscallExecuteError, "Failed to execute cf_shmctl !");
}


} // namespace cf
