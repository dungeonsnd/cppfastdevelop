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

#ifndef _HEADER_FILE_CFD_CF_EXCEPTION_HPP_
#define _HEADER_FILE_CFD_CF_EXCEPTION_HPP_

#include "cppfoundation/cf_root.hpp"

namespace cf
{

/**
 *  ExceptClass is derived from cf::BaseException exception-class.
 *
 *  Using e.g. :
 *
    DEFINECLASS_CHILD_EXCEPTION(CTestExcept, cf::BaseException);

    try{
        THROW_EX_(CTestExcept, "hijklmn");
    }catch(cf_const CTestExcept & e){
        printf("%s\n",e.what());
    }
 *
 */
#define DEFINECLASS_CHILD_EXCEPTION(ExceptClass, Base)             \
    \
    class ExceptClass : public Base                           \
    {                                                         \
    public:                                                   \
        ExceptClass(cf_cpstr file,                                \
                    cf_int line,                                  \
                    cf_cpstr func,                                \
                    cf_int errnumber,                             \
                    cf_const std::string & msg                    \
                   ) throw()                                  \
            : Base(file, line, func, errnumber, msg)           \
        {}                                                    \
        \
        std::string GetClassName() cf_const                      \
        {                                                     \
            return #ExceptClass;                               \
        }                                                     \
    };

#define _THROW(ExceptionClass, msg)                           \
    {   \
        throw ExceptionClass(__FILE__, __LINE__, __PRETTY_FUNCTION__, errno, msg); \
    }

#define _THROW_FMT(ExceptionClass, format, ...)                           \
    { \
        char errstr[4096] ={0}; \
        cf_snprintf(errstr,sizeof(errstr),format,##__VA_ARGS__ ); \
        throw ExceptionClass(__FILE__, __LINE__, __PRETTY_FUNCTION__, errno, errstr); \
    }


/**
 * The root exception-class.
 *
 */
class BaseException : public std::exception , public Object
{
public:
    BaseException(  cf_cpstr file,
                    cf_int line,
                    cf_cpstr func,
                    cf_int errnum,
                    cf_const std::string & msg ) throw();

    virtual ~BaseException() throw();
    cf_cpstr what() cf_const throw();

protected:
    virtual std::string GetClassName() cf_const { return "BaseException"; }
    std::string GetStackTrace() cf_const;

    cf_cpstr _file;
    cf_int _line;
    cf_cpstr _func;
    cf_int _errnum;
    std::string _errStr;
    std::string _msg;
    std::string _time;

private:
    enum { MAX_STACKTRACESIZE = 64 };
    cf_void * _stackTrace[MAX_STACKTRACESIZE];
    size_t _stackTraceSize;
    mutable std::string _what;
};

// Don't use this way of thowing !
#define _THROW_EXCEPTION(msg) \
    ( throw cf::BaseException(__FILE__, __LINE__, __PRETTY_FUNCTION__, errno, msg) )


// 1. cf::BaseException
// 1.1 cf::Fatal
DEFINECLASS_CHILD_EXCEPTION(Fatal, BaseException);

// 1.2 cf::Error
DEFINECLASS_CHILD_EXCEPTION(Error, BaseException);
DEFINECLASS_CHILD_EXCEPTION(UnsupportedSyscallError, Error);
DEFINECLASS_CHILD_EXCEPTION(SyscallExecuteError, Error);
DEFINECLASS_CHILD_EXCEPTION(NullPointerError, Error);
DEFINECLASS_CHILD_EXCEPTION(ValueError, Error);
DEFINECLASS_CHILD_EXCEPTION(UnimplementedError, Error);
DEFINECLASS_CHILD_EXCEPTION(AllocateMemoryError, Error);

// 1.3 cf::Warning
DEFINECLASS_CHILD_EXCEPTION(Warning, BaseException);
DEFINECLASS_CHILD_EXCEPTION(RuntimeWarning, Warning);

// 1.4 cf::Info
DEFINECLASS_CHILD_EXCEPTION(Info, BaseException);
DEFINECLASS_CHILD_EXCEPTION(TimeoutInfo, Info);
DEFINECLASS_CHILD_EXCEPTION(WouldBlockInfo, Info);
DEFINECLASS_CHILD_EXCEPTION(AlreadyLockedInfo, Info);
DEFINECLASS_CHILD_EXCEPTION(SignalIntrInfo, Info);
DEFINECLASS_CHILD_EXCEPTION(ElementNotFoundInfo, Info);

/**
e.g.
    DEFINECLASS_CHILD_EXCEPTION(CSizeTooSmallException, cf::Warning);

    _THROW(UnsupportedSyscallError, "Not supported of flock !");
    _THROW(NullPointerError, "NULL == _pShm !")
    _THROW(ValueError, "_pShm == MAP_FAILED !")
    _THROW(SyscallExecuteError, "Failed to execute cf_mmap !")
    _THROW(AllocateMemoryError, "Allocate memory failed !")
    _THROW(UnimplementedError, "Unsupported temporarily !")

    _THROW(RuntimeWarning, "_type{%d} unexpected !",_type)

    _THROW(ElementNotFoundInfo, "Element not found!")


    cf_int rt =pthread_mutex_trylock(&_mutex);
    if (0!=rt)
    {
        if(EBUSY==rt)
            _THROW_FMT(AlreadyLockedInfo, "Failed to execute pthread_mutex_trylock , rt=%d , already locked!",rt)
        else
            _THROW_FMT(SyscallExecuteError, "Failed to execute pthread_mutex_trylock , rt=%d !",rt)
    }
*/

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_EXCEPTION_HPP_

