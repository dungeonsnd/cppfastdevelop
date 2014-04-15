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

#ifndef _HEADER_FILE_CFD_CF_UTILITY_PROGRAM_HPP_
#define _HEADER_FILE_CFD_CF_UTILITY_PROGRAM_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

#ifndef MAX_SIZE
#define MAX_SIZE 4096
#endif

// 从路径加文件名称中获取'/'的位置索引,从传入字符串的末尾开始搜索.
inline void GetIndexByPathNameExt(cf_cpstr pp, cf_int32 & last1,
                                  cf_int32 & last2)
{
    last1 =-1;
    last2 =-1;
    int len =int(strlen(pp));
    int cnt =0;
    for(int i=len-1; i>=0; i--)
    {
        if('/'==pp[i])
        {
            if(0==cnt)
            {
                last1 =i;
                cnt++;
            }
            else if(1==cnt)
            {
                last2 =i;
                cnt++;
            }
            else
                break;
        }
    }
}

// 从路径加文件名称中获取文件名称. 如果最末是'/'解释为没有文件名称,返回空.
inline cf_cpstr GetNameByPathNameExt(cf_cpstr pp, cf_uint32 ppLen ,
                                     cf_char p [MAX_SIZE])
{
    cf_int32 last1, last2;
    GetIndexByPathNameExt(pp, last1, last2);
    if(-1==last1)
    {
        memcpy(p,pp,ppLen);
        return p;
    }
    if(int(ppLen-1)==last1)
    {
        p[0] ='\0';
        return p;
    }
    memset(p,0,sizeof(p));
    memcpy(p,pp+last1+1,int(ppLen-last1));
    p[ppLen-last1] ='\0';
    return p;
}

// 从路径加文件名称中获取文件路径. 如果最末是'/'解释为没有文件名称,返回输入的路径,返回路径中不带'/'.
inline cf_cpstr GetPathByPathNameExt(cf_cpstr pp, cf_int32 ppLen ,
                                     char p [MAX_SIZE])
{
    cf_int32 last1, last2;
    GetIndexByPathNameExt(pp, last1, last2);
    if(last1<=0)
    {
        p[0] ='\0';
        return p;
    }
    memset(p,0,sizeof(p));
    memcpy(p,pp,last1);
    p[last1] ='\0';
    return p;
}

// 获取执行程序所在的绝对路径加程序名称
inline cf_cpstr GetProgramFullName(char fullName[MAX_SIZE], int & len)
{
    len = readlink("/proc/self/exe", fullName, MAX_SIZE);
    if (len < 0 || len >= MAX_SIZE)
        return NULL;
    return fullName;
}

// 获取执行程序所在的绝对路径
inline cf_cpstr GetExeAbsPath(char absPath[MAX_SIZE])
{
    int len;
    GetProgramFullName(absPath,len);
    if('\0'==absPath[0])
        return absPath;
    char absPathRes[MAX_SIZE];
    memcpy(absPathRes,absPath,sizeof(absPathRes));
    return GetPathByPathNameExt(absPathRes, len , absPath);
}

// 获取执行程序名称
inline cf_cpstr GetExeName(char exeName[MAX_SIZE])
{
    int len;
    GetProgramFullName(exeName,len);
    if('\0'==exeName[0])
        return exeName;
    char exeNameRes[MAX_SIZE];
    memcpy(exeNameRes,exeName,sizeof(exeNameRes));
    return GetNameByPathNameExt(exeNameRes, len , exeName);
}

// 获取执行程序所在的绝对路径的上一级目录
inline cf_cpstr GetExeParentAbsPath(char parentAbsPath[MAX_SIZE])
{
    int len;
    GetProgramFullName(parentAbsPath,len);
    if('\0'==parentAbsPath[0])
        return parentAbsPath;
    cf_int32 last1, last2;
    GetIndexByPathNameExt(parentAbsPath, last1, last2);

    if(last1<=0||last2<=0)
    {
        parentAbsPath[0] ='\0';
        return parentAbsPath;
    }
    parentAbsPath[last2] ='\0';
    return parentAbsPath;
}

// 目录或文件是否存在,return 1 存在, return 0 不存在.
inline int IsFileExist(cf_cpstr file)
{
    return 0==access(file, R_OK);
}

cf_void SetProcessDaemon();

cf_void IgnoreSignals();

class PidFileWriter : public NonCopyable
{
public:
    PidFileWriter(cf_cpstr filePathName,
                  cf_int32 filePathNameLen):_filePathName(filePathName)
    {
    }
    cf_void Set()
    {
        WritePid();
    }
    cf_uint32 Get()
    {
        return ReadPid();
    }
    ~PidFileWriter()
    {
        cf_fsync(_fd);
        cf_close(_fd);
        cf_unlink(_filePathName.c_str());
    }
private:
    cf_void WritePid()
    {
        char p [MAX_SIZE];
        GetPathByPathNameExt(_filePathName.c_str(), _filePathName.size() , p);
        if(0==IsFileExist(p))
        {
            if(-1==mkdir(p, S_IRWXU | S_IRWXG | S_IROTH))
            {
                _THROW(SyscallExecuteError, "Failed to execute mkdir !");
            }
        }

        _fd =cf_open(_filePathName.c_str(),O_RDWR|O_CREAT|O_TRUNC,
                     S_IRWXU | S_IRWXG | S_IROTH);
        if(-1==_fd)
        {
            _THROW(SyscallExecuteError, "Failed to execute cf_open !");
        }
        char buf[8] = {0};
        int n =snprintf(buf,sizeof(buf),"%u",unsigned(getpid()));
        ssize_t nw =cf_write(_fd,buf,n);
        if(nw!=n)
        {
            _THROW_FMT(ValueError, "nw{%d}!=n{%d} !",int(nw),int(n));
        }
    }
    cf_uint32 ReadPid()
    {
        char p [MAX_SIZE];
        GetPathByPathNameExt(_filePathName.c_str(), _filePathName.size() , p);
        if(0==IsFileExist(p))
        {
            _THROW_FMT(RuntimeWarning, "Failed access file %s !",p);
        }

        _fd =cf_open(_filePathName.c_str(),O_RDWR,S_IRWXU | S_IRWXG | S_IROTH);
        if(-1==_fd)
        {
            _THROW(SyscallExecuteError, "Failed to execute cf_open !");
        }
        char buf[8] = {0};
        ssize_t nr =cf_read(_fd,buf,sizeof(buf));
        if(nr<=0)
        {
            _THROW_FMT(ValueError, "Failed cf_read ! nr=%d ",int(nr));
        }
        return cf_uint32(atoi(buf));
    }

    int _fd;
    std::string _filePathName;
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_UTILITY_PROGRAM_HPP_

