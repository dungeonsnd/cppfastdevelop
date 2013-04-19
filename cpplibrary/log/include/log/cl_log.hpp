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

#ifndef _HEADER_FILE_CFD_CL_LOG_HPP_
#define _HEADER_FILE_CFD_CL_LOG_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"
#include "cppfoundation/cf_lock.hpp"
#include "cppfoundation/cf_lock_guard.hpp"

namespace cl
{
namespace log
{

enum LEVEL
{
    DEBUG=0,
    INFO,
    NOTICE,
    WARN,
    ERROR,
    CRIT,
    FATAL,
    COUNT // End of enum define.
};
cf_char LEVELNAME[COUNT] = {'D','I','N','W','E','C','F'};

#define CLLOG_VERBOSE

#ifdef CLLOG_VERBOSE
#define CLLOG(logObj,level,buf,len) \
    logObj.WriteVerbose(level, buf, len,__FILE__, __LINE__, getpid(),pthread_self())
#else
#define CLLOG(logObj,level,buf,len) \
    logObj.Write(level, buf, len,__FILE__, __LINE__)
#endif

// Thread-safe
template < typename MutexType =cf::PthreadMutex /*or = cf::FakeMutex*/ >
class LogNonCache : public cf::NonCopyable
{
public:
    enum
    {
        MAX_MSG_LEN =8192
    };

    LogNonCache(  cf_const std::string & logFileName,
                  cf_int flags=O_RDWR|O_CREAT|O_APPEND,
                  mode_t mode=S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP):
        _fd(-1),
        _level(NOTICE),
        _lastSeconds(0)
    {
        _fd =cf_open(logFileName.c_str(),flags,mode);
        if(-1==_fd)
            _THROW(cf::SyscallExecuteError, "Failed to execute cf_open !")
        }
    LogNonCache(cf_int fd =stdout):
        _fd(fd),
        _level(NOTICE),
        _lastSeconds(0)
    {
    }
    ~LogNonCache()
    {
        if(_fd>2)
        {
            int rt =0;
            rt =cf_fsync(_fd);
            if(-1==rt)
                ;//_THROW(SyscallExecuteError, "Failed to execute cf_fsync !")
            rt =cf_close(_fd);
            if(-1==rt)
                ;//_THROW(SyscallExecuteError, "Failed to execute cf_close !")
        }
    }
    cf_void SetLevel(LEVEL level)
    {
        _level =level;
    }
    LEVEL GetLevel()
    {
        return _level;
    }

    ssize_t Write(  LEVEL level,
                    cf_cpstr buf, size_t len,
                    cf_cpstr fileName, size_t lineNum)
    {
        if(level<_level)
            return 0;

        struct timeval tv;
        if(-1==gettimeofday(&tv, NULL))
            _THROW(cf::SyscallExecuteError, "Failed to execute gettimeofday !")

            cf::LockGuard<MutexType> lock(_mutex);
        memset(_destBuf,0,sizeof(_destBuf));

        if(tv.tv_sec!=_lastSeconds)
        {
            struct tm gmt;
            gmtime_r(&tv.tv_sec, &gmt);
            cf_snprintf(_lastTimeCache,sizeof(_lastTimeCache),
                        "%4d%02d%02d %02d:%02d:%02d",
                        gmt.tm_year+1900,gmt.tm_mon+1,gmt.tm_mday,
                        gmt.tm_hour,gmt.tm_min,gmt.tm_sec);
        }
        _idx =sizeof(_lastTimeCache)-1;
        cf_memcpy(_destBuf,_lastTimeCache,_idx);

        cf_snprintf(_destBuf+_idx,5,".%03d",tv.tv_usec);
        _idx +=5;

        cf_snprintf(_destBuf+_idx,3," %c",LEVELNAME[level]);
        _idx +=3;

        cf_snprintf(_destBuf+_idx,32," %s:",fileName);
        _idx +=32;

        cf_snprintf(_destBuf+_idx,6,"%04d ",lineNum);
        _idx +=6;

        cf_memcpy(_destBuf+_idx,buf,len);
        _idx +=len;
        cf_memcpy(_destBuf+_idx,"\n",1);
        _idx +=1;

        return write( _fd,_destBuf,_idx );
    }

    ssize_t WriteVerbose(  LEVEL level,
                           cf_cpstr buf, size_t len,
                           cf_cpstr fileName, size_t lineNum,
                           pid_t pid,pthread_t tid)
    {
        if(level<_level)
            return 0;

        struct timeval tv;
        if(-1==gettimeofday(&tv, NULL))
            _THROW(cf::SyscallExecuteError, "Failed to execute gettimeofday !")

            cf::LockGuard<MutexType> lock(_mutex);
        memset(_destBuf,0,sizeof(_destBuf));

        if(tv.tv_sec!=_lastSeconds)
        {
            struct tm gmt;
            gmtime_r(&tv.tv_sec, &gmt);
            cf_snprintf(_lastTimeCache,sizeof(_lastTimeCache),
                        "%4d%02d%02d %02d:%02d:%02d",
                        gmt.tm_year+1900,gmt.tm_mon+1,gmt.tm_mday,
                        gmt.tm_hour,gmt.tm_min,gmt.tm_sec);
        }
        _idx =sizeof(_lastTimeCache)-1;
        cf_memcpy(_destBuf,_lastTimeCache,_idx);

        cf_snprintf(_destBuf+_idx,5,".%03d",tv.tv_usec);
        _idx +=5;

        cf_snprintf(_destBuf+_idx,3," %c",LEVELNAME[level]);
        _idx +=3;

        cf_snprintf(_destBuf+_idx,16," p%d",pid);
        _idx +=16;
        cf_snprintf(_destBuf+_idx,16," t%u",(unsigned int)tid);
        _idx +=16;

        cf_snprintf(_destBuf+_idx,32,"%s:",fileName);
        _idx +=32;

        cf_snprintf(_destBuf+_idx,6,"%04d ",lineNum);
        _idx +=6;

        cf_memcpy(_destBuf+_idx,buf,len);
        _idx +=len;
        cf_memcpy(_destBuf+_idx,"\n",1);
        _idx +=1;

        return write( _fd,_destBuf,_idx );
    }

private:


    cf_int _fd;
    LEVEL _level;
    time_t _lastSeconds;
    cf_char _lastTimeCache[18];
    cf_char _destBuf[MAX_MSG_LEN];
    size_t _idx;
    MutexType _mutex;
};


} //namespace cl
} // namespace log

#endif // _HEADER_FILE_CFD_CL_LOG_HPP_

