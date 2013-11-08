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

#ifndef _HEADER_FILE_CFD_CF_INCLUDE_SYSTEM_H_
#define _HEADER_FILE_CFD_CF_INCLUDE_SYSTEM_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>

#include <stdint.h> // for uint16_t,for ntohs

#include <sys/uio.h> // for readv
#include <sys/file.h> // for flock

#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netdb.h> // getaddrinfo

#include <netdb.h>
extern int h_errno;

// SystemV Message Queue.
#include <sys/ipc.h>
#include <sys/msg.h>
// SystemV share memory
#include <sys/shm.h>
// Posix share memory
#include <sys/mman.h>
// Posix semaphore
#include <semaphore.h>

#include <mqueue.h>
#include <time.h>
#include <sys/time.h>

#ifdef __linux__
#include <sys/epoll.h>
#endif

#include <dlfcn.h>

#endif // _HEADER_FILE_CFD_CF_INCLUDE_SYSTEM_H_
