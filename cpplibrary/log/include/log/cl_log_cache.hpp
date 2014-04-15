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

#ifndef _HEADER_FILE_CFD_CL_LOG_CACHE_HPP_
#define _HEADER_FILE_CFD_CL_LOG_CACHE_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_utility.hpp"
#include "log/cl_log_commons.hpp"

namespace cl
{
namespace log
{

// Not thread-safe, but process-safe
class LogCache : public cf::NonCopyable
{
public:

    LogCache();
    cf_void Init(cf_const std::string & logFileName,
                 cf_int max_msg_len =CL_LOG_MAX_MSG_LEN,cf_int max_loop =CL_LOG_MAX_LOOP,
                 cf_int flags=O_RDWR|O_CREAT|O_APPEND,
                 mode_t mode=S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    cf_void Init(cf_int fd);
    ~LogCache();
    cf_void SetLevel(CLLEVEL level);
    CLLEVEL GetLevel();
    cf_char * GetBuf();
    cf_uint GetBufLen();
    ssize_t Write(  CLLEVEL level,size_t len,
                    cf_cpstr fileName, size_t lineNum,cf_cpstr funcName,
                    pid_t pid,pthread_t tid);

    cf_void Flush();
private:
    cf_void AllocBuf(cf_int max_msg_len =CL_LOG_MAX_MSG_LEN,
                     cf_int max_loop =CL_LOG_MAX_LOOP);

    cf_int _fd;
    CLLEVEL _level;
    cf_char _lastTimeCache[19];
    std::vector < std::string > _destBuf;
    std::vector < cf_int > _idx;
    cf_int _currentLoop;

    time_t _lastSeconds;
    pid_t _lastPid;
    pthread_t _lastTid;
    std::string _pidtidCache;
};

extern LogCache logcacheobj;

#define CLLOGC_INIT(logFileName) cl::log::logcacheobj.Init(logFileName)
#define CLLOGC_SET(level) cl::log::logcacheobj.SetLevel(level)
#define CLLOGC_FLUSH() cl::log::logcacheobj.Flush()

#define CLLOGC(level,...) \
    { \
        int n =snprintf(cl::log::logcacheobj.GetBuf(), \
                        cl::log::logcacheobj.GetBufLen(),__VA_ARGS__); \
        char p [256] ={0}; \
        cl::log::logcacheobj.Write(level, n, \
                                   cf::GetNameByPathNameExt(__FILE__,strlen(__FILE__),p), \
                                   __LINE__,__FUNCTION__, getpid(),pthread_self());  \
    }

} // namespace log
} //namespace cl

#endif // _HEADER_FILE_CFD_CL_LOG_CACHE_HPP_

