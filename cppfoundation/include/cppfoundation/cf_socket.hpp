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

#ifndef _HEADER_FILE_CFD_CF_SOCKET_HPP_
#define _HEADER_FILE_CFD_CF_SOCKET_HPP_

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

class Socket:NonCopyable
{
public:
    Socket(cf_sockethandle sock):
        _sock(sock)
    {}
    ~Socket()
    {
        if (_sock > 0)
            cf_close(_sock);
    }
    cf_void Close()
    {
        if (_sock > 0)
            cf_close(_sock);
        else
            _THROW_FMT(ValueError, "_sock{%d}<=0 !", cf_int(_sock));
    }
    cf_void Shutdown()
    {
        if (_sock > 0)
            cf_shutdown(_sock, SHUT_WR);
        else
            _THROW_FMT(ValueError, "_sock{%d}<=0 !", cf_int(_sock));
    }

    cf_int WriteNowait(cf_cpvoid data, cf_uint32 len);
    cf_int ReadNowait(cf_pvoid data, cf_uint32 len);

    std::string getAddr();
    cf_uint32 getIp();
    cf_uint16 getPort();

    bool setKeepAlive(bool on);
    bool setReuseAddress(bool on);
    bool setSoLinger (bool doLinger, int seconds);
    bool setIntOption(int option, int value);
    bool setTimeOption(int option, int milliseconds);
    bool setSoBlocking(bool on);
private:
    cf_sockethandle _sock;
};

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_SOCKET_HPP_
