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

#ifndef _HEADER_FILE_CFD_CF_UTILITY_HPP_
#define _HEADER_FILE_CFD_CF_UTILITY_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

#ifndef ERR
#define ERR(msg) \
    { \
        fprintf(stderr,"Error! %s,file=%s,line=%d,errno=%d,strerror=%s \n",msg,__FILE__,__LINE__,errno,strerror(errno)); \
        exit(1); \
    }
#endif // ERR


cf_void SetProcessDaemon();

cf_void IgnoreSignals();


// type traits
// If invoker give "T * obj", use these PointerTraits can get the 'T' type.
/**
e.g.

template < typename T >
void func()
{
    typedef typename PointerTraits<T>::ValueType TypeOfT;
    // TypeOfT is 'int'
}

int * x =new int
func(x);
**/
template <typename D>
struct PointerTraits
{
    typedef D TraitsValueType;
    typedef D * TraitsPointerType;
};
template <typename D>
struct PointerTraits<D *>
{
    typedef D  TraitsValueType;
    typedef D * TraitsPointerType;
};


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_UTILITY_HPP_

