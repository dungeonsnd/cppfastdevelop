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

#include "cppfoundation/cf_system_stub.h"

pid_t cf_fork()
{
    return fork();
}
cf_void cf_exit(cf_int status)
{
    return exit(status);
}
cf_int cf_chdir(cf_cpstr path)
{
    if(ISNULL(path))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return chdir(path);
}
pid_t cf_setsid()
{
    return setsid();
}

cf_int cf_open(cf_cpstr pathname, cf_int flags, mode_t mode)
{
    if(ISNULL(pathname))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return open(pathname,flags,mode);
}

cf_int cf_close(cf_int fd)
{
    return close(fd);
}
cf_int cf_dup(cf_int oldfd)
{
    return dup(oldfd);
}
cf_int cf_fcntl(cf_int fd, cf_int cmd, ...)
{
    va_list valp;
    cf_int rt;
    va_start(valp,cmd);
    rt = fcntl(fd, cmd, valp);
    va_end(valp);
    return rt;
}

ssize_t cf_read(cf_int fd, cf_pvoid  buf, size_t count)
{
    if(ISNULL(buf))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }
    return read(fd,buf,count);
}

ssize_t cf_write(cf_int fd, cf_cpvoid buf, size_t count)
{
    if(ISNULL(buf))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }

    return write(fd,buf,count);
}

ssize_t cf_readv(cf_int fd, cf_const struct iovec * vector, cf_int count)
{
    if(ISNULL(vector))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }

    return readv(fd,vector,count);
}

ssize_t cf_writev(cf_int fd, cf_const struct iovec * vector, cf_int count)
{
    if(ISNULL(vector))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }

    return writev(fd,vector,count);
}


cf_int cf_fstat(cf_int filedes, struct stat * buf)
{
    if(ISNULL(buf))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else if(filedes == -1)
    {
        errno = EBADF;
        return -1;
    }

    return fstat(filedes,buf);
}

cf_int cf_ftruncate(cf_int fd, off_t length)
{
    return ftruncate(fd,length);
}

cf_int cf_flock(cf_int fd, cf_int operation)
{
    return flock(fd, operation);
}

cf_int cf_fsync(cf_int fd)
{
    return fsync(fd);
}
cf_int cf_fdatasync(cf_int fd)
{
    return fdatasync(fd);
}

ssize_t cf_recv(cf_int socket, cf_pvoid  buffer, size_t length, cf_int flags)
{
    return   recv( socket,  buffer,  length,  flags);
}

ssize_t cf_send(cf_int socket, cf_cpvoid buffer, size_t length, cf_int flags)
{
    return send(socket, buffer, length, flags);
}

ssize_t cf_sendmsg(cf_int socket, cf_const struct msghdr * message,
                   cf_int flags)
{
    return   sendmsg( socket,  message,  flags);
}
ssize_t cf_recvmsg(cf_int socket, struct msghdr * message, cf_int flags)
{
    return recvmsg( socket, message,  flags);
}

cf_int cf_listen(cf_int socket, cf_int backlog)
{
    return listen(socket, backlog);
}

cf_int cf_bind(cf_int socket, cf_const struct sockaddr * address,
               socklen_t address_len)
{
    return bind(socket, address,address_len);

}

cf_int cf_connect(cf_int socket, cf_const struct sockaddr * address,
                  socklen_t address_len)
{
    return   connect( socket, address, address_len);
}

cf_int cf_accept(cf_int socket, struct sockaddr * address,
                 socklen_t * address_len)
{
    return accept( socket,  address, address_len);
}


cf_int cf_shutdown(cf_int socket, cf_int how)
{
    return  shutdown( socket, how);
}

cf_int cf_setsockopt(cf_int socket, cf_int level, cf_int option_name,
                     cf_cpvoid option_value, socklen_t option_len)
{
    return   setsockopt( socket,  level,  option_name, option_value,  option_len);

}

cf_int cf_select(cf_int nfds, fd_set * readfds, fd_set * writefds,
                 fd_set * exceptfds, struct timeval * timeout)
{
    return select(nfds,readfds,writefds,exceptfds,timeout);
}

cf_int cf_poll(struct pollfd * fds, nfds_t nfds, cf_int timeout)
{
    if (ISNULL(fds))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return poll(fds, nfds, timeout);
}

key_t cf_ftok(cf_cpstr pathname, cf_int id)
{
    if(ISNULL(pathname))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return ftok(pathname,id);
}


cf_int cf_msgget(key_t key,cf_int oflag)
{
    return msgget(key,oflag);
}

cf_int cf_msgsnd(cf_int msqid, cf_cpvoid buf, size_t length, cf_int flag)
{
    return msgsnd(msqid,buf,length,flag);
}
ssize_t cf_msgrcv(cf_int msqid, cf_pvoid buf, size_t length, cf_long type,
                  cf_int flag)
{
    return msgrcv(msqid,buf,length,type,flag);
}
cf_int cf_msgctl(cf_int msqid,cf_int cmd,struct msqid_ds * buf)
{
    return msgctl(msqid,cmd,buf);
}


cf_int cf_shmget(key_t key, size_t size,cf_int shmflg)
{
    return shmget(key,size,shmflg);
}
cf_pvoid cf_shmat(cf_int shmid, cf_cpvoid shmaddr, cf_int shmflag)
{
    return shmat(shmid,shmaddr,shmflag);
}
cf_int cf_shmctl(cf_int shmid, cf_int cmd, struct shmid_ds * buf)
{
    return shmctl(shmid, cmd, buf);
}
cf_int cf_shmdt(cf_cpvoid shmaddr)
{
    if(ISNULL(shmaddr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return shmdt(shmaddr);
}



cf_int cf_mlock(cf_cpvoid addr, size_t len)
{
    if (ISNULL(addr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return mlock(addr, len);
}
cf_int cf_munlock(cf_cpvoid addr, size_t len)
{
    if (ISNULL(addr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return munlock(addr, len);
}

cf_pvoid cf_mmap(cf_pvoid addr, size_t length, cf_int prot, cf_int flags,
                 cf_int fd, off_t offset)
{
    return mmap(addr, length, prot, flags, fd, offset);
}

cf_int cf_munmap(cf_pvoid addr, size_t length)
{
    if(ISNULL(addr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return munmap(addr, length);
}

cf_int cf_msync(cf_pvoid start, size_t length, cf_int flags)
{
    if (ISNULL(start))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return msync(start, length, flags);
}

cf_int cf_shm_open(cf_cpstr name, cf_int oflag, mode_t mode)
{
    if (ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return shm_open(name, oflag, mode);
}


cf_int cf_shm_unlink(cf_cpstr name)
{
    if (ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return shm_unlink(name);
}



cf_void cf_pthread_testcancel()
{
    pthread_testcancel();
}

cf_int cf_pthread_setcancelstate(cf_int state, cf_int  * oldstate)
{
    return pthread_setcancelstate(state,oldstate);
}

cf_int cf_pthread_attr_init(pthread_attr_t * attr)
{
    if(ISNULL(attr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return pthread_attr_init(attr);
}

cf_int cf_pthread_attr_destroy(pthread_attr_t * attr)
{
    if(ISNULL(attr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return pthread_attr_destroy(attr);
}

cf_int cf_pthread_attr_setstacksize(pthread_attr_t * attr, size_t stacksize)
{
    if(ISNULL(attr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return pthread_attr_setstacksize(attr,stacksize);
}

cf_int cf_pthread_attr_getstacksize(cf_const pthread_attr_t * attr,
                                    size_t * stacksize)
{
    return pthread_attr_getstacksize(attr,stacksize);
}

cf_int cf_pthread_attr_setdetachstate(pthread_attr_t * attr, cf_int detachstate)
{
    if(ISNULL(attr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return pthread_attr_setdetachstate(attr,detachstate);
}

cf_int cf_pthread_attr_getdetachstate(cf_const pthread_attr_t * attr,
                                      cf_int  * detachstate)
{
    return pthread_attr_getdetachstate(attr,detachstate);
}

cf_int cf_pthread_create(pthread_t * thread, cf_const pthread_attr_t * attr,
                         cf_pvoid (*start_routine)(cf_pvoid), cf_pvoid arg)
{
    return pthread_create(thread,attr,start_routine,arg);
}

cf_int cf_pthread_cancel(pthread_t thread)
{
    return pthread_cancel(thread);
}

cf_int cf_pthread_join(pthread_t thread, cf_ppvoid value_ptr)
{
    return pthread_join(thread,value_ptr);
}

cf_int cf_pthread_detach(pthread_t thread)
{
    return pthread_detach(thread);
}

pthread_t cf_pthread_self()
{
    return pthread_self();
}




cf_pvoid cf_memset(cf_pvoid s, cf_int c, size_t n)
{
    if(ISNULL(s))
    {
        errno = CF_E_NULLPARAMS;
        return (NULL);
    }

    if(n > 0)
    {
        return memset(s,c,n);
    }
    else
    {
        return (s);
    }
}

cf_pvoid cf_memcpy(cf_pvoid dest, cf_cpvoid src, size_t n)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = CF_E_NULLPARAMS;
        return NULL;
    }

    if(n > 0)
    {
        return memcpy(dest,src,n);
    }
    else
    {
        return dest;
    }
}

cf_pvoid cf_memmove(cf_pvoid dest, cf_cpvoid src, size_t n)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = CF_E_NULLPARAMS;
        return NULL;
    }

    if(n > 0)
    {
        return memmove(dest,src,n);
    }
    else
    {
        return dest;
    }
}

cf_char  * cf_strcpy(cf_char  * dest, cf_cpstr src)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = CF_E_NULLPARAMS;
        return (NULL);
    }

    return strcpy(dest,src);
}

cf_char  * cf_strncpy(cf_char  * dest, cf_cpstr src, size_t n)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = CF_E_NULLPARAMS;
        return (NULL);
    }

    if(n > 0)
    {
        return strncpy(dest,src,n);
    }
    else
    {
        return (dest);
    }
}

cf_cpvoid cf_memchr(cf_cpvoid s, cf_int c, size_t n)
{
    if(ISNULL(s))
    {
        errno = CF_E_NULLPARAMS;
        return (NULL);
    }

    return memchr(s,c,n);
}

cf_cpstr  cf_strchr(cf_cpstr s, cf_int c)
{
    if(ISNULL(s))
    {
        errno = CF_E_NULLPARAMS;
        return (NULL);
    }

    return strchr(s,c);
}

cf_cpstr  cf_strstr(cf_cpstr haystack, cf_cpstr needle)
{
    if(ISNULL(haystack)||ISNULL(needle))
    {
        errno = CF_E_NULLPARAMS;
        return (NULL);
    }

    return strstr(haystack,needle);
}

cf_int cf_sprintf(cf_char  * str, cf_cpstr format, ...)
{
    cf_int                 rc;
    va_list             valp;
    if(ISNULL(str)||ISNULL(format))
    {
        errno = CF_E_NULLPARAMS;
        return (0);
    }

    va_start( valp, format );
    rc = vsprintf( str, format, valp );
    va_end( valp );

    return (rc);
}

cf_int cf_snprintf(cf_char  * str, size_t size, cf_cpstr format, ...)
{
    cf_int                 rc;
    va_list             valp;
    if(ISNULL(str)||ISNULL(format))
    {
        errno = CF_E_NULLPARAMS;
        return (0);
    }
    else if(size == 0)
    {
        return (0);
    }

    va_start( valp, format );
    rc = vsnprintf( str, size ,format, valp );
    va_end( valp );

    return (rc);
}

cf_int cf_vsnprintf(cf_char  * str, size_t size, cf_cpstr format, va_list ap)
{
    cf_int                 rc;

    if(ISNULL(str)||ISNULL(format))
    {
        errno = CF_E_NULLPARAMS;
        return (0);
    }
    else if(size == 0)
    {
        return (0);
    }
    rc = vsnprintf( str, size ,format, ap);

    return rc;
}

size_t cf_strlen(cf_cpstr s)
{
    if(ISNULL(s))
    {
        errno = CF_E_NULLPARAMS;
        return (0);
    }

    return strlen(s);
}

cf_int cf_strcmp(cf_cpstr s1, cf_cpstr s2)
{
    if(ISNULL(s1) && ISNULL(s2))
    {
        return (0);
    }
    else if(ISNULL(s1) || ISNULL(s2))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }

    return strcmp(s1,s2);
}

cf_int cf_strncmp(cf_cpstr s1, cf_cpstr s2, size_t n)
{
    if(ISNULL(s1) && ISNULL(s2))
    {
        return (0);
    }
    else if(ISNULL(s1) || ISNULL(s2))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }

    return strncmp(s1,s2,n);
}

cf_int cf_memcmp(cf_cpvoid s1, cf_cpvoid s2, size_t n)
{
    if(ISNULL(s1) && ISNULL(s2))
    {
        return 0;
    }
    else if(ISNULL(s1) || ISNULL(s2))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }

    return memcmp(s1,s2,n);
}


cf_int cf_getpeername(cf_int socket, struct sockaddr * address,
                      socklen_t * address_len)
{
    return getpeername( socket,  address,address_len);
}

cf_int cf_getsockname(cf_int s, struct sockaddr * name, socklen_t * namelen)
{
    if (ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return getsockname(s, name, namelen);
}

cf_cpstr cf_inet_ntop(cf_int af, cf_cpvoid src,
                      cf_char  * dst, socklen_t cnt)
{
    if (ISNULL(src) || ISNULL(dst))
    {
        errno = CF_E_NULLPARAMS;
        return NULL;
    }
    return inet_ntop(af, src, dst, cnt);
}

uint16_t cf_ntohs(uint16_t netshort)
{
    return ntohs(netshort);
}
uint16_t cf_htons(uint16_t hostshort)
{
    return htons(hostshort);
}

cf_int cf_socket(cf_int domain, cf_int type, cf_int protocol)
{
    return    socket( domain,  type,  protocol);
}

time_t cf_time(time_t * t)
{
    return time(t);
}

struct tm * cf_localtime_r(cf_const time_t * timep, struct tm * result)
{
    if(ISNULL(timep))
    {
        errno = CF_E_NULLPARAMS;
        return NULL;
    }

    return localtime_r(timep, result);
}

cf_int cf_unlink(cf_cpstr pathname)
{
    if (ISNULL(pathname))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return unlink(pathname);
}

struct hostent * cf_gethostbyname(cf_cpstr name)
{
    if (ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return NULL;
    }
    return gethostbyname(name);
}

sem_t * cf_sem_open(cf_cpstr name, cf_int oflag,
                    mode_t mode, cf_uint value)
{
    if (ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return SEM_FAILED;
    }
    return sem_open(name,oflag,mode,value);
}

cf_int cf_sem_getvalue(sem_t * sem, cf_int  * sval)
{
    return sem_getvalue(sem, sval);
}

cf_int cf_sem_wait(sem_t * sem)
{
    return sem_wait(sem);
}

cf_int cf_sem_trywait(sem_t * sem)
{
    return sem_trywait(sem);
}

cf_int cf_sem_post(sem_t * sem)
{
    return sem_post(sem);
}

cf_int cf_sem_unlink(cf_cpstr name)
{
    if (ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return sem_unlink(name);
}

cf_int cf_sem_close(sem_t * sem)
{
    return sem_close(sem);
}



#if defined(__linux__)
cf_int cf_backtrace(cf_ppvoid buffer, cf_int size)
{
    return backtrace(buffer, size);
}

cf_char  ** cf_backtrace_symbols(cf_pcpvoid buffer, cf_int size)
{
    return backtrace_symbols(buffer, size);
}
#endif


cf_void cf_free(cf_pvoid ptr)
{
    if(ISNULL(ptr))
    {
        errno = CF_E_NULLPARAMS;
    }
    else
    {
        free(ptr);
    }

}

cf_pvoid cf_malloc(size_t size)
{
    return malloc(size);
}

cf_int cf_mkfifo(cf_cpstr pathname, mode_t mode)
{
    if(ISNULL(pathname))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mkfifo(pathname, mode);
}

cf_int cf_socketpair(cf_int domain, cf_int type, cf_int protocol, cf_int sv[2])
{
    return socketpair(domain, type, protocol, sv);
}

cf_int cf_getaddrinfo(cf_const cf_char * node, cf_const cf_char * service,
                      cf_const struct addrinfo * hints,
                      struct addrinfo ** res)
{
    return getaddrinfo(node,service,hints,res);
}

cf_void cf_freeaddrinfo(struct addrinfo * res)
{
    freeaddrinfo(res);
}

cf_const cf_char * cf_gai_strerror(cf_int errcode)
{
    return gai_strerror(errcode);
}

mqd_t cf_mq_open(cf_cpstr name, cf_int oflag, mode_t mode,struct mq_attr * attr)
{
    if(ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return (mqd_t) -1;
    }
    else
        return mq_open(name, oflag, mode,attr);
}

cf_int cf_mq_unlink(cf_cpstr name)
{
    if(ISNULL(name))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_unlink(name);
}

cf_int cf_mq_close(mqd_t mqdes)
{
    return mq_close(mqdes);
}

cf_int cf_mq_getattr(mqd_t mqdes, struct mq_attr * attr)
{
    if(ISNULL(attr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_getattr(mqdes,attr);
}
cf_int cf_mq_setattr(mqd_t mqdes, struct mq_attr * newattr,
                     struct mq_attr * oldattr)
{
    if(ISNULL(newattr)||ISNULL(oldattr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_setattr(mqdes,newattr,oldattr);
}

cf_int cf_mq_send(mqd_t mqdes, const char * msg_ptr,
                  size_t msg_len, unsigned msg_prio)
{
    if(ISNULL(msg_ptr))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_send(mqdes,msg_ptr,msg_len,msg_prio);
}
cf_int cf_mq_timedsend(mqd_t mqdes, const char * msg_ptr,
                       size_t msg_len, unsigned msg_prio,
                       const struct timespec * abs_timeout)
{
    if(ISNULL(msg_ptr)||ISNULL(abs_timeout))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_timedsend(mqdes,msg_ptr,msg_len,msg_prio,abs_timeout);
}
ssize_t cf_mq_receive(mqd_t mqdes, char * msg_ptr,
                      size_t msg_len, unsigned * msg_prio)
{
    if(ISNULL(msg_ptr)||ISNULL(msg_prio))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_receive(mqdes,msg_ptr,msg_len,msg_prio);
}
ssize_t cf_mq_timedreceive(mqd_t mqdes, char * msg_ptr,
                           size_t msg_len, unsigned * msg_prio,
                           const struct timespec * abs_timeout)
{
    if(ISNULL(msg_ptr)||ISNULL(msg_prio)||ISNULL(abs_timeout))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_timedreceive(mqdes,msg_ptr,msg_len,msg_prio,abs_timeout);
}
cf_int cf_mq_notify(mqd_t mqdes, const struct sigevent * sevp)
{
    if(ISNULL(sevp))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return mq_notify(mqdes,sevp);
}

mode_t cf_umask(mode_t cmask)
{
    return umask(cmask);
}

cf_int cf_getrlimit(cf_int resource, struct rlimit * rlp)
{
    if(ISNULL(rlp))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return getrlimit(resource, rlp);
}
cf_int cf_setrlimit(cf_int resource, cf_const struct rlimit * rlp)
{
    if(ISNULL(rlp))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    else
        return setrlimit(resource, rlp);
}

cf_sighandler_t cf_signal(cf_int signum, cf_sighandler_t handler)
{
    if(ISNULL(handler))
    {
        errno = CF_E_NULLPARAMS;
        return SIG_ERR;
    }
    else
        return signal(signum, handler);
}

cf_int cf_access(cf_cpstr pathname, cf_int mode)
{
    if(ISNULL(pathname))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return access(pathname,mode);
}

pid_t cf_getpid()
{
    return getpid();
}
pid_t cf_getppid()
{
    return getppid();
}


#ifdef __linux__
cf_int cf_epoll_wait(cf_int epfd, struct epoll_event * events,
                     cf_int maxevents, cf_int timeout)
{
    if (ISNULL(events))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return epoll_wait(epfd, events, maxevents, timeout);
}
cf_int cf_epoll_ctl(cf_int epfd, cf_int op, cf_int fd,
                    struct epoll_event * event)
{
    if (ISNULL(event))
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return epoll_ctl(epfd, op, fd, event);
}

cf_int cf_epoll_create(cf_int size)
{
    if (size < 0)
    {
        errno = CF_E_NULLPARAMS;
        return -1;
    }
    return epoll_create(size);
}

cf_int cf_epoll_create1(cf_int flags)
{
    return epoll_create1(flags);
}

#endif

