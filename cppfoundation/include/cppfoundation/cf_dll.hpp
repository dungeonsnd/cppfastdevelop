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
#ifndef _HEADER_FILE_DLL_HPP_
#define _HEADER_FILE_DLL_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

class Dll
{
public:
    static std::string GetPrename()
    {
        return "lib";
    }
    static std::string GetExtname()
    {
        return ".so";
    }

    Dll(cf_cpstr filename)
    {
        if (0!=cf_access(filename,R_OK))
        {
#ifdef _DEBUG
            fprintf(stdout,"cf_access,%s.errno=%d,%s \n",filename,errno,strerror(errno));
#endif
            _THROW_FMT(SyscallExecuteError, "Failed to execute cf_access!");
        }

        _handle =cf_dlopen(filename, RTLD_LAZY);
        if (!_handle)
        {
#ifdef _DEBUG
            fprintf(stdout,"Failed to execute cf_dlopen , %s!",cf_dlerror());
#endif
            _THROW_FMT(SyscallExecuteError, "Failed to execute cf_dlopen , %s!",
                       cf_dlerror());
        }
        cf_dlerror();
    }
    ~Dll()
    {
        int rt =cf_dlclose(_handle);
        if (0!=rt)
        {
#ifdef _DEBUG
            fprintf(stderr, "Failed dlclose, %s\n", cf_dlerror());
#endif
        }
    }
    void * GetFunction(const char * funcName)
    {
        void * p =cf_dlsym(_handle, funcName);
        if (NULL==p)
        {
#ifdef _DEBUG
            fprintf(stderr, "Failed dlsym, %s\n", cf_dlerror());
#endif
            _THROW_FMT(SyscallExecuteError, "Failed to execute cf_dlsym , %s!",
                       cf_dlerror());
        }
        return p;
    }
private:
    void * _handle;
};


} // namespace cf

#endif // _HEADER_FILE_DLL_HPP_


