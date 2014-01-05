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

#ifndef _HEADER_FILE_CFD_CF_SYSTEM_STUB_H_
#define _HEADER_FILE_CFD_CF_SYSTEM_STUB_H_


#include "cppfoundation/cf_include_system.h"
#include "cppfoundation/cf_types_defination.h"

#define CF_E_NULLPARAMS 8192

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

pid_t cf_fork();
cf_void cf_exit(cf_int status);
cf_int cf_chdir(cf_cpstr path);
pid_t cf_setsid();

// file operation.
cf_int cf_access(cf_cpstr pathname, cf_int mode);
cf_int cf_open(cf_cpstr pathname, cf_int flags, mode_t mode);
cf_int cf_dup(cf_int oldfd);
cf_int cf_close(cf_int fd);
cf_int cf_fcntl(cf_int fd, cf_int cmd, ...);
ssize_t cf_read(cf_int fd, cf_pvoid  buf, size_t count);
ssize_t cf_write(cf_int fd, cf_cpvoid buf, size_t count);
ssize_t cf_readv(cf_int fd, cf_const struct iovec * vector, cf_int count);
ssize_t cf_writev(cf_int fd, cf_const struct iovec * vector, cf_int count);
cf_int cf_fstat(cf_int filedes, struct stat * buf);
cf_int cf_ftruncate(cf_int fd, off_t length);
cf_int cf_flock(cf_int fd, cf_int operation);
cf_int cf_fsync(cf_int fd);
cf_int cf_fdatasync(cf_int fd);
key_t cf_ftok(cf_cpstr pathname, cf_int id);
mode_t cf_umask(mode_t cmask);
cf_int cf_getrlimit(cf_int resource, struct rlimit * rlp);
cf_int cf_setrlimit(cf_int resource, cf_const struct rlimit * rlp);

FILE * cf_fopen(cf_cpstr path, cf_cpstr mode);
cf_int cf_fclose(FILE * fp);

// socket operation.
ssize_t cf_recv(cf_int socket, cf_pvoid  buffer, size_t length, cf_int flags);
ssize_t cf_send(cf_int socket, cf_cpvoid buffer, size_t length, cf_int flags);
ssize_t cf_sendmsg(cf_int socket, cf_const struct msghdr * message,
                   cf_int flags);
ssize_t cf_recvmsg(cf_int socket, struct msghdr * message, cf_int flags);

cf_int cf_listen(cf_int socket, cf_int backlog);
cf_int cf_bind(cf_int socket, cf_const struct sockaddr * address,
               socklen_t address_len);
cf_int cf_connect(cf_int socket, cf_const struct sockaddr * address,
                  socklen_t address_len);
cf_int cf_accept(cf_int socket, struct sockaddr * address,
                 socklen_t * address_len);
cf_int cf_shutdown(cf_int socket, cf_int how);
cf_int cf_setsockopt(cf_int socket, cf_int level, cf_int option_name,
                     cf_cpvoid option_value, socklen_t option_len);

cf_int cf_getpeername(cf_int socket, struct sockaddr * address,
                      socklen_t * address_len);
cf_int cf_getsockname(cf_int s, struct sockaddr * name, socklen_t * namelen);
cf_cpstr ca_inet_ntop(cf_int af, cf_cpvoid src,
                      cf_char * dst, socklen_t cnt);
uint16_t cf_ntohs(uint16_t netshort);
uint16_t cf_htons(uint16_t hostshort);
cf_int cf_socket(cf_int domain, cf_int type, cf_int protocol);
time_t cf_time(time_t * t);
struct tm * cf_localtime_r(const time_t * timep, struct tm * result);
cf_int cf_unlink(cf_cpstr pathname);
struct hostent * cf_gethostbyname(cf_cpstr name);
cf_int cf_socketpair(cf_int domain, cf_int type, cf_int protocol, cf_int sv[2]);

cf_int cf_getaddrinfo(cf_const cf_char * node, cf_const cf_char * service,
                      cf_const struct addrinfo * hints,
                      struct addrinfo ** res);
cf_void cf_freeaddrinfo(struct addrinfo * res);
cf_const cf_char * cf_gai_strerror(cf_int errcode);
cf_int cf_inet_pton(cf_int af, cf_cpstr src, cf_pvoid dst);

// SystemV message queue.
cf_int cf_msgget(key_t key,cf_int oflag);
cf_int cf_msgsnd(cf_int msqid, cf_cpvoid  buf, size_t length, cf_int flag);
ssize_t cf_msgrcv(cf_int msqid, cf_pvoid buf, size_t length, cf_long type,
                  cf_int flag);
cf_int cf_msgctl(cf_int msgid,cf_int cmd,struct msqid_ds * buf);

// Posix message queue.
mqd_t cf_mq_open(cf_cpstr name, cf_int oflag, mode_t mode,
                 struct mq_attr * attr);
cf_int cf_mq_unlink(cf_cpstr name);
cf_int cf_mq_close(mqd_t mqdes);
cf_int cf_mq_getattr(mqd_t mqdes, struct mq_attr * attr);
cf_int cf_mq_setattr(mqd_t mqdes, struct mq_attr * newattr,
                     struct mq_attr * oldattr);
cf_int cf_mq_send(mqd_t mqdes, const char * msg_ptr,
                  size_t msg_len, unsigned msg_prio);
cf_int cf_mq_timedsend(mqd_t mqdes, const char * msg_ptr,
                       size_t msg_len, unsigned msg_prio,
                       const struct timespec * abs_timeout);
ssize_t cf_mq_receive(mqd_t mqdes, char * msg_ptr,
                      size_t msg_len, unsigned * msg_prio);
ssize_t cf_mq_timedreceive(mqd_t mqdes, char * msg_ptr,
                           size_t msg_len, unsigned * msg_prio,
                           const struct timespec * abs_timeout);
cf_int cf_mq_notify(mqd_t mqdes, const struct sigevent * sevp);


//SystemV share memory.
cf_int cf_shmget(key_t key, size_t size,cf_int shmflg);
cf_pvoid cf_shmat(cf_int shmid, cf_cpvoid shmaddr, cf_int shmflag);
cf_int cf_shmctl(cf_int shmid, cf_int cmd, struct shmid_ds * buf);
cf_int cf_shmdt(cf_cpvoid shmaddr);

// Posix share memory.
cf_int cf_mlock(cf_cpvoid addr, size_t len);
cf_int cf_munlock(cf_cpvoid addr, size_t len);
cf_pvoid cf_mmap(cf_pvoid addr, size_t length, cf_int prot, cf_int flags,
                 cf_int fd, off_t offset);
cf_int cf_munmap(cf_pvoid addr, size_t length);
cf_int cf_msync(cf_pvoid start, size_t length, cf_int flags);

cf_int cf_shm_open(cf_cpstr name, cf_int oflag, mode_t mode);
cf_int cf_shm_unlink(cf_cpstr name);

// Pthread operation.
cf_void cf_pthread_testcancel();
cf_int cf_pthread_setcancelstate(cf_int state, cf_int * oldstate);
cf_int cf_pthread_attr_init(pthread_attr_t * attr);
cf_int cf_pthread_attr_destroy(pthread_attr_t * attr);
cf_int cf_pthread_attr_setstacksize(pthread_attr_t * attr, size_t stacksize);
cf_int cf_pthread_attr_getstacksize(cf_const pthread_attr_t * attr,
                                    size_t * stacksize);
cf_int cf_pthread_attr_setdetachstate(pthread_attr_t * attr,
                                      cf_int detachstate);
cf_int cf_pthread_attr_getdetachstate(cf_const pthread_attr_t * attr,
                                      cf_int  * detachstate);
cf_int cf_pthread_create(pthread_t * thread, cf_const pthread_attr_t * attr,
                         cf_pvoid (*start_routine)(cf_pvoid), cf_pvoid arg);
cf_int cf_pthread_cancel(pthread_t thread);
cf_int cf_pthread_join(pthread_t thread, cf_ppvoid value_ptr);
cf_int cf_pthread_detach(pthread_t thread);
pthread_t cf_pthread_self();

// memory & string operation.
cf_pvoid cf_memset(cf_pvoid s, cf_int c, size_t n);
cf_pvoid cf_memcpy(cf_pvoid dest, cf_cpvoid src, size_t n);
cf_pvoid cf_memmove(cf_pvoid dest, cf_cpvoid src, size_t n);
cf_char * cf_strcpy(cf_char * dest, cf_cpstr src);
cf_char * cf_strncpy(cf_char * dest, cf_cpstr src, size_t n);
cf_cpvoid cf_memchr(cf_cpvoid s, cf_int c, size_t n);
cf_cpstr  cf_strchr(cf_cpstr s, cf_int c);
cf_cpstr cf_strstr(cf_cpstr haystack, cf_cpstr needle);
cf_int cf_sprintf(cf_char * str, cf_cpstr format, ...);
cf_int cf_snprintf(cf_char * str, size_t size, cf_cpstr format, ...);
cf_int cf_vsnprintf(cf_char * str, size_t size, cf_cpstr format, va_list ap);
size_t cf_strlen(cf_cpstr s);
cf_int cf_strcmp(cf_cpstr s1, cf_cpstr s2);
cf_int cf_strncmp(cf_cpstr s1, cf_cpstr s2, size_t n);
cf_int cf_memcmp(cf_cpvoid s1, cf_cpvoid s2, size_t n);
cf_void cf_free(cf_pvoid ptr);
cf_pvoid cf_malloc(size_t size);

// Posix semophore.
sem_t * cf_sem_open(cf_cpstr name, cf_int oflag,
                    mode_t mode, cf_uint value);
cf_int cf_sem_getvalue(sem_t * sem, cf_int  * sval);
cf_int cf_sem_wait(sem_t * sem);
cf_int cf_sem_trywait(sem_t * sem);
cf_int cf_sem_post(sem_t * sem);
cf_int cf_sem_unlink(cf_cpstr name);
cf_int cf_sem_close(sem_t * sem);

// Signal.
typedef cf_void (* cf_sighandler_t)(cf_int);
cf_sighandler_t cf_signal(cf_int signum, cf_sighandler_t handler);

// pid.
pid_t cf_getpid();
pid_t cf_getppid();

// demultiplex
cf_int cf_select(cf_int nfds, fd_set * readfds, fd_set * writefds,
                 fd_set * exceptfds, struct timeval * timeout);
cf_int cf_poll(struct pollfd * fds, nfds_t nfds, cf_int timeout);
#ifdef __linux__
cf_int cf_epoll_wait(cf_int epfd, struct epoll_event * events,
                     cf_int maxevents, cf_int timeoutMS);
cf_int cf_epoll_ctl(cf_int epfd, cf_int op, cf_int fd,
                    struct epoll_event * event);
cf_int cf_epoll_create(cf_int size);
cf_int cf_epoll_create1(cf_int flags);

cf_int cf_backtrace(cf_ppvoid buffer, cf_int size);
cf_char  ** cf_backtrace_symbols(cf_pcpvoid buffer, cf_int size);
#endif // __linux__

// dll
cf_pvoid cf_dlopen(cf_cpstr filename, cf_int flag);
cf_cpstr cf_dlerror();
cf_pvoid cf_dlsym(cf_pvoid handle,cf_cpstr symbol);
cf_int cf_dlclose(cf_pvoid handle);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _HEADER_FILE_CFD_CF_SYSTEM_STUB_H_

