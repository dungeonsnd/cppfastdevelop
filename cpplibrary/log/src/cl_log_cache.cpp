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

#include "log/cl_log_cache.hpp"

namespace cl
{
namespace log
{

LogCache logcacheobj;

char logcacheLevelName[COUNT][CLLEVELNAME_MAX_LEN] = {"TRACE","DEBUG"," INFO"," WARN","ERROR","FATAL"};

LogCache::LogCache():
    _fd(-1),
    _level(TRACE),
    _currentLoop(0),
    _lastSeconds(0),
    _lastPid(0),
    _lastTid(0)
{
}

cf_void LogCache::AllocBuf(cf_int max_msg_len, cf_int max_loop)
{
    for(cf_int i=0; i<max_loop; i++)
    {
        std::string str(max_msg_len,'\0');
        _destBuf.push_back(str);
        _idx.push_back(0);
    }
}

cf_void LogCache::Init(cf_const std::string & logFileName,
                       cf_int max_msg_len,cf_int max_loop,
                       cf_int flags,
                       mode_t mode)
{
    _fd =cf_open(logFileName.c_str(),flags,mode);
    if(-1==_fd)
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_open !");
    AllocBuf(max_msg_len, max_loop);
}

cf_void LogCache::Init(cf_int fd)
{
    _fd =fd;
    if(-1==_fd)
        _THROW(cf::SyscallExecuteError, "Failed to execute cf_open !");
    AllocBuf();
}

LogCache::~LogCache()
{
    Flush();
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
cf_void LogCache::SetLevel(CLLEVEL level)
{
    _level =level;
}
CLLEVEL LogCache::GetLevel()
{
    return _level;
}

ssize_t LogCache::Write(  CLLEVEL level,
                          size_t len,
                          cf_cpstr fileName, size_t lineNum,cf_cpstr funcName,
                          pid_t pid,pthread_t tid)
{
    if(level<_level)
        return 0;
    int n =0;
    struct timeval tv;
    if(-1==gettimeofday(&tv, NULL))
        _THROW(cf::SyscallExecuteError, "Failed to execute gettimeofday !");

    int & idx =_idx[_currentLoop];

    // Write prefix.
    if(tv.tv_sec!=_lastSeconds)
    {
        struct tm gmt;
        gmtime_r(&tv.tv_sec, &gmt);
        cf_snprintf(_lastTimeCache,sizeof(_lastTimeCache),
                    "%4d%02d%02d %02d:%02d:%02d.",
                    gmt.tm_year+1900,gmt.tm_mon+1,gmt.tm_mday,
                    gmt.tm_hour,gmt.tm_min,gmt.tm_sec);
    }
    idx =sizeof(_lastTimeCache)-1;
    char * p =&(_destBuf[_currentLoop][0]);
    cf_memcpy(p,_lastTimeCache,idx);

    n =cf_snprintf(p+idx,15,"%06d %6s ",tv.tv_usec,
                   logcacheLevelName[level]);
    idx +=n;
    _destBuf[_currentLoop][idx] =' ';
    idx +=1;

    // Write suffix.
    if(idx+len+CL_LOG_SUFFIX_LEN<CL_LOG_MAX_MSG_LEN)
    {
        idx +=len;

        if(pid==_lastPid&&tid==_lastTid)
        {
            memcpy(p+idx,&_pidtidCache[0],_pidtidCache.size());
            n =_pidtidCache.size();
        }
        else
        {
            n =cf_snprintf(p+idx,CL_LOG_SUFFIX_LEN,
                           " [pid:%d tid:%u  ",
                           pid,(cf_uint)tid);
            _pidtidCache.assign(p+idx,n);

            _lastPid =pid;
            _lastTid =tid;
        }
        idx +=n-1;

        memcpy(p+idx,fileName,128); // max file name length.
        n =strlen(fileName);
        idx +=n;

        memcpy(p+idx,":",1);
        idx +=1;

        n =cf_snprintf(p+idx,6,
                       "%d ",lineNum); // max line number length.
        idx +=n;

        memcpy(p+idx,funcName,128); // max function name length.
        n =strlen(funcName);
        idx +=n;

        memcpy(p+idx,"]\n",2);
        idx +=2;
    }
    else
    {
        idx =CL_LOG_MAX_MSG_LEN;
        _destBuf[_currentLoop][idx-1] ='\n';
    }

    ++_currentLoop;
    if(_currentLoop==int(_destBuf.size()))
        Flush();
    return 0;
}

cf_void LogCache::Flush()
{
    if(_currentLoop==0)
        return;
    for(int i=0; i<_currentLoop; i++)
    {
        int & idx =_idx[i];
        write( _fd,&(_destBuf[i][0]),idx );
        //        printf("--write-- \n");
    }
    _currentLoop =0;
}

cf_char * LogCache::GetBuf()
{
    char * p =&(_destBuf[_currentLoop][0]);
    //    memset(p,' ',sizeof(_destBuf)[_currentLoop]);
    return p+CL_LOG_PREFIX_LEN;
}

cf_uint LogCache::GetBufLen()
{
    return CL_LOG_MAX_MSG_LEN-CL_LOG_PREFIX_LEN;
}

} // namespace log
} // namespace cl
