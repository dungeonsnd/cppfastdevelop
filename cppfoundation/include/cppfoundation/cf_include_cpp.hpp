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

#ifndef _HEADER_FILE_CFD_CF_INCLUDE_CPP_HPP_
#define _HEADER_FILE_CFD_CF_INCLUDE_CPP_HPP_

// misc.
#include <memory>
#include <stddef.h>

// stl.
#include <string>
#include <queue>
#include <vector>
#include <list>
#include <map>
#include <set>

// stream.
#include <iostream>
#include <sstream>

// exception.
#include <typeinfo>
#include <exception>
#ifdef __linux__
#include <execinfo.h> // int backtrace(void **buffer, int size);
#include <cxxabi.h>
#endif // __linux__

// shared_ptr support.
#if defined(__GNUC__) && __GNUC__ >= 4
    #ifndef __GXX_EXPERIMENTAL_CXX0X__ // std::tr1
//        #pragma message("$$$$ Using shared_ptr from <tr1/memory> !")
        #include <tr1/memory>
        namespace std 
        {
            using tr1::bad_weak_ptr;
            using tr1::const_pointer_cast;
            using tr1::dynamic_pointer_cast;
            using tr1::enable_shared_from_this;
            using tr1::get_deleter;
            using tr1::shared_ptr;
            using tr1::static_pointer_cast;
            using tr1::swap;
            using tr1::weak_ptr;
        }
    #else
//        #pragma message("$$$$ Using shared_ptr from <memory> !")
        #include <memory>
    #endif // __GXX_EXPERIMENTAL_CXX0X__
#else
//    #pragma message("$$$$ Using shared_ptr from <boost/tr1/memory.hpp> !")
    #include <boost/tr1/memory.hpp> // boost tr1
    namespace std 
    {
        using tr1::bad_weak_ptr;
        using tr1::const_pointer_cast;
        using tr1::dynamic_pointer_cast;
        using tr1::enable_shared_from_this;
        using tr1::get_deleter;
        using tr1::shared_ptr;
        using tr1::static_pointer_cast;
        using tr1::swap;
        using tr1::weak_ptr;
    }
#endif


#endif // _HEADER_FILE_CFD_CF_INCLUDE_CPP_HPP_
