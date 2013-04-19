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

#ifndef _HEADER_FILE_CFD_CF_MEMORY_HPP_
#define _HEADER_FILE_CFD_CF_MEMORY_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{

/// alloc momery.

template <typename Type>
inline Type * cf_New()
{
    Type * p = NULL;
    try
    {
        p = new Type;
    }
    catch (std::bad_alloc & ba)
    {
        p = NULL;
    }
    return p;
}

template <typename Type>
inline Type * cf_PlacementNew(void * p)
{
    Type * pt = NULL;
    try
    {
        pt = new (p) Type;
    }
    catch (std::bad_alloc & ba)
    {
        pt = NULL;
    }
    return pt;
}

template <typename Type>
inline Type * cf_NewA(cf_uint arraySize)
{
    Type * p = NULL;
    try
    {
        p = new Type[arraySize];
    }
    catch (std::bad_alloc & ba)
    {
        p = NULL;
    }
    return p;
}

#define CF_NEWOBJ(Pointer, Class, ...) \
    Class * Pointer=NULL; \
    try \
    { \
        Pointer = new Class(__VA_ARGS__); \
    } \
    catch (std::bad_alloc& ba) \
    { \
        Pointer = NULL; \
    } \
     
#define CF_NEWOBJ_A(Pointer, Class, arraySize) \
    Class * Pointer=NULL; \
    try \
    { \
        Pointer = new Class [arraySize]; \
    } \
    catch (std::bad_alloc& ba) \
    { \
        Pointer = NULL; \
    } \
     
/// free momery.

template <typename Type>
inline cf_int cf_Delete(Type *& rp)
{
    cf_int rt =0;
    if (rp != NULL)
    {
        try
        {
            delete rp;
            rp = NULL;
        }
        catch (std::bad_alloc & ba)
        {
            rt =-1;
        }
        rp = NULL;
    }
    return rt;
}

template <typename Type>
inline cf_int cf_DeleteA(Type *& rp)
{
    cf_int rt =0;
    if (rp != NULL)
    {
        delete [] rp;
        rp = NULL;
    }
    return rt;
}


template <typename Type> inline void cf_Free(Type *& rp)
{
    if (rp != NULL)
    {
        free(rp);
        rp = NULL;
    }
}

} // namespace cf
#endif // _HEADER_FILE_CFD_CF_MEMORY_HPP_

