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

#ifndef _HEADER_FILE_CFD_CF_fdET_HPP_
#define _HEADER_FILE_CFD_CF_fdET_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

namespace socketdefs
{
enum
{
    DEFAULT_STACKSIZE =1024*1024*2
};
} // namespace socketdefs


class Socket : public NonCopyable
{
public:
    Socket(cf_fd fd, in_addr ip ,cf_uint16 port):
        _fd(fd),_ip(ip),_port(port)
    {}
    ~Socket()
    {
        if (_fd > 0)
            cf_shutdown(_fd, SHUT_WR);
    }
    cf_void Close()
    {
        if (_fd > 0)
            cf_close(_fd);
        else
            _THROW_FMT(ValueError, "_fd{%d}<=0 !", cf_int(_fd));
        _fd =-1;
    }
    cf_void Shutdown()
    {
        if (_fd > 0)
            cf_shutdown(_fd, SHUT_WR);
        else
            _THROW_FMT(ValueError, "_fd{%d}<=0 !", cf_int(_fd));
    }

    cf_int SendAsync(cf_cpvoid data, ssize_t len);
    cf_int RecvAsync(cf_pvoid data, ssize_t len, bool & peerClosedWhenRead);

    cf_fd Fd() cf_const
    {
        return _fd;
    }
    cf_uint32 Ip() cf_const
    {
        return (cf_uint32)(_ip.s_addr);
    }
    std::string Addr() cf_const
    {
        return (inet_ntoa(_ip));
    }

    cf_uint16 Port() cf_const
    {
        return _port;
    }
    /*
    bool setKeepAlive(bool on);
    bool setReuseAddress(bool on);
    bool setSoLinger (bool doLinger, int seconds);
    bool setIntOption(int option, int value);
    bool setTimeOption(int option, int milliseconds);
    bool setSoBlocking(bool on);
    */
private:
    cf_fd _fd;
    in_addr _ip;
    cf_uint16 _port;
};

typedef std::shared_ptr < Socket > T_SESSION;

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_fdET_HPP_
