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

#ifndef _HEADER_FILE_CFD_CF_EPOLL_HPP_
#define _HEADER_FILE_CFD_CF_EPOLL_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_memory.hpp"
#include "cppfoundation/cf_network.hpp"
#include "cppfoundation/cf_utility.hpp"
//#include "cppfoundation/cf_ipc.hpp"

namespace cf
{

namespace epolldefs
{
enum
{
    SIZE_CREATE = 1,
    SIZE_MAXEVENTS = 1024
};
} // namespace lockdefs

class Epoll : public cf::NonCopyable
{
public:
    typedef std::unordered_set < cf_fd > TYPE_SETLISTENFDS;
    typedef std::unordered_map < cf_fd, cf_ev > TYPE_MAPEVENT;
    typedef TYPE_MAPEVENT::iterator TYPE_MAPEVENT_ITER;
    typedef std::vector < std::pair < cf_fd, networkdefs::EV_TYPE > > TYPE_VECEVENT;
    typedef TYPE_VECEVENT::iterator TYPE_VECEVENT_ITER;

    Epoll(const TYPE_SETLISTENFDS & listenfds ,
          cf_int maxEvents =epolldefs::SIZE_MAXEVENTS);
    ~Epoll();
    cf_void AddConn(cf_fd fd,networkdefs::EV_TYPE ev=networkdefs::CFEV_READ);
    cf_void DelConn(cf_fd fd);
    cf_void AddEvent(cf_fd fd,networkdefs::EV_TYPE ev);
    cf_void DelEvent(cf_fd fd,networkdefs::EV_TYPE ev);
    cf_void WaitEvent(TYPE_VECEVENT & vecEvent, cf_int timeoutMilliseconds);
private:
    cf_void EpollCtl(cf_fd fd,cf_int op, cf_ev event);

    TYPE_SETLISTENFDS _listenfds;
    cf_int _epfd;
    TYPE_MAPEVENT _mapEvent;
    cf_int _maxEvents;
    std::vector < epoll_event > _retEvents;
    // It's not needed in single thread.
    //    Pipe < SocketpairPipe > _pipe;
    //    cf_char _buf4pipe[1];
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_EPOLL_HPP_

