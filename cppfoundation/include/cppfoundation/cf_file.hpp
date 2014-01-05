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

#ifndef _HEADER_FILE_CFD_CF_FILE_HPP_
#define _HEADER_FILE_CFD_CF_FILE_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{
typedef enum _tagOpenMode
{
    MODE_READ =01,
    MODE_WRITE =02,
    MODE_WRITE_APPEND =04
} OpenMode;
} // namespace filedefs


class CFile : public NonCopyable
{
public:
    CFile(cf_cpstr fileName,OpenMode mode):
        _file(NULL)
    {
        if(mode&MODE_READ)
        {
            _file = cf_fopen(fileName,"r+");
        }
        else if(mode&MODE_WRITE)
        {
            _file = cf_fopen(fileName,"w+");
        }
        else
        {
            _file = cf_fopen(fileName,"a+");
        }
        if(NULL==_file)
        {
#ifdef _DEBUG
            fprintf(stdout,"Failed to execute cf_fopen!");
#endif
            _THROW_FMT(SyscallExecuteError, "Failed to execute cf_fopen!");
        }
    }
    ~CFile()
    {
        if(_file)
        {
            cf_fclose(_file);
            _file = NULL;
        }
    }

    FILE * file()
    {
        return _file;
    }
private:
    FILE * _file;
};

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_FILE_HPP_
