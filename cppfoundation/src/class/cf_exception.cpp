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

#include "cppfoundation/cf_exception.hpp"

namespace cf
{

BaseException::BaseException(cf_cpstr file,
                             cf_int line,
                             cf_cpstr func,
                             cf_int errnum,
                             cf_const std::string & msg
                            ) throw() :
    _file(file),
    _line(line),
    _func(func),
    _errnum(errnum),
    _errStr(""),
    _msg(msg),
    _time(""),
    _stackTraceSize(0)
{
#if defined(__linux__)
    _stackTraceSize = cf_backtrace(_stackTrace, MAX_STACKTRACESIZE);
#endif

    cf_char            stamp[64];
    struct  tm      sttm;
    time_t          now;
    cf_time(&now);
    cf_localtime_r(&now, &sttm);
    cf_snprintf(stamp,sizeof(stamp), "%04d%02d%02d-%02d:%02d:%02d",
                sttm.tm_year+1900,sttm.tm_mon+1,sttm.tm_mday,sttm.tm_hour,sttm.tm_min,
                sttm.tm_sec);
    _time = stamp;

    cf_char buf[4096];
    cf_memset(buf,0,sizeof(buf));
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
    strerror_r(_errnum, buf, sizeof(buf)); /* XSI-compliant */
    _errStr.assign(buf);
#else
    _errStr.assign(strerror_r(_errnum, buf, sizeof(buf)));
#endif
    if(CF_E_NULLPARAMS==_errnum)
    {
        _errStr.assign("Function parameter is NULL !");
    }
}
BaseException::~BaseException() throw()
{
}

cf_cpstr BaseException::what() cf_const throw()
{
    if (_what.empty())
    {
        std::stringstream sstr("");
        sstr<<"\n--------------------------------------------------\n"; // 50CHARs
        if (_line > 0)
        {
            sstr << "FILE       = "<<_file << std::endl;
            sstr << "LINE       = "<< _line << std::endl;
            sstr << "FUNC       = "<<_func << std::endl;
            sstr << "Errno      = "<<_errnum << std::endl;
            sstr << "ErrStr     = "<<_errStr << std::endl;
        }
        sstr     << "Class      = " << GetClassName() << std::endl;
        if (!_msg.empty())
        {
            sstr << "DESP       = " << _msg << std::endl;
        }
        sstr     << "Time       = " << _time << std::endl;
        sstr     << "Stack Trace= \n";
        std::string result =  GetStackTrace();
        sstr << result;
        sstr<<"- - - - - - - - - - - - - - - - - - - - - - - - - ";
        _what = sstr.str();
    }

    return _what.c_str();
}

#ifdef __GLIBC__
std::string BaseException::GetStackTrace() cf_const
{
    if (_stackTraceSize == 0)
        return "++++ No stack trace! ++++\n";
    cf_char ** strings = cf_backtrace_symbols(_stackTrace, _stackTraceSize);
    if (strings == NULL)
        return "++++ Unknown error: backtrace_symbols returned strings==NULL! ++++\n";
    std::string result;
    for (size_t i = 0; i < _stackTraceSize; ++i)
    {
        std::string mangledName = strings[i];
        std::string::size_type begin = mangledName.find('(');
        std::string::size_type end = mangledName.find('+', begin);
        if (begin == std::string::npos || end == std::string::npos)
        {
            result += mangledName;
            result += '\n';
            continue;
        }
        ++begin;
        cf_int status;
        cf_char * s = abi::__cxa_demangle(mangledName.substr(begin, end-begin).c_str(),
                                          NULL, 0, &status);
        if (status != 0)
        {
            result += mangledName;
            result += '\n';
            continue;
        }
        std::string demangledName(s);
        cf_free(s);
        result += mangledName.substr(0, begin);
        result += demangledName;
        result += mangledName.substr(end);
        result += '\n';
    }
    cf_free(strings);
    return result;
}

#else
std::string BaseException::GetStackTrace() cf_const
{
    return std::string("++++ Stacktrace is not supported on this platform! ++++");
}
#endif


} // namespace cf
