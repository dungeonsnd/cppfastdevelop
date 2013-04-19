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

#ifndef _HEADER_FILE_CFD_CF_BINARY_SEARCH_HPP_
#define _HEADER_FILE_CFD_CF_BINARY_SEARCH_HPP_

#include "cppfoundation/cf_root.hpp"

namespace cf
{

// Binary search. Ensure the array is ascending .
// return 1,found,and insertIndex is target index;
// return 0,not found,insertIndex is the index it need to insert.
// return <0,error happens,insertIndex is undefined.
// low and high are the array index,not the value size.

// default compare function.
template < typename T >
struct BinarySearchCompare
{
    cf_int64 operator()(cf_const T & x, cf_const T & y) cf_const
    {
        if(x >= y) return 0;
        else return 1;
    }
};

// Use compare class to compare the elements.
template< typename T, typename compare_less>
cf_int BinarySearchAscending(T * a, cf_int64 low, cf_int64 high,
                             cf_const T & target, cf_int64 & insertIndex)
{
#ifdef _ADDCHECK_
    if(low<0)
        return -1;
    if(low>high)
        return -2;
#endif
    compare_less comp;
    while (low <= high)
    {
        cf_int64 middle = low + (high - low)/2;
        if (comp(target, a[middle]))
            high = middle - 1;
        else if (comp(a[middle], target))
            low = middle + 1;
        else
        {
            insertIndex =middle;
            return 1;
        }
    }

    insertIndex =low;

    return 0;
}

// Use '<' to compare the elements.
template< typename T >
cf_int BinarySearchAscending(T * a, cf_int64 low, cf_int64 high,
                             cf_const T & target, cf_int64 & insertIndex)
{
#ifdef _ADDCHECK_
    if(low<0)
        return -1;
    if(low>high)
        return -2;
#endif
    while (low <= high)
    {
        cf_int64 middle = low + (high - low)/2;
        if ( target < a[middle] )
            high = middle - 1;
        else if ( a[middle] < target )
            low = middle + 1;
        else
        {
            insertIndex =middle;
            return 1;
        }
    }

    insertIndex =low;
    return 0;
}

// Use compare class to compare the elements.
template< typename T, typename compare_less>
cf_int BinarySearchDescending(T * a, cf_int64 low, cf_int64 high,
                              cf_const T & target, cf_int64 & insertIndex)
{
#ifdef _ADDCHECK_
    if(low<0)
        return -1;
    if(low>high)
        return -2;
#endif
    compare_less comp;
    while (low <= high)
    {
        cf_int64 middle = low + (high - low)/2;
        if (comp(target, a[middle]))
            low = middle + 1;
        else if (comp(a[middle], target))
            high = middle - 1;
        else
        {
            insertIndex =middle;
            return 1;
        }
    }

    insertIndex =low;

    return 0;
}

// Use '<' to compare the elements.
template< typename T >
cf_int BinarySearchDescending(T * a, cf_int64 low, cf_int64 high,
                              cf_const T & target, cf_int64 & insertIndex)
{
#ifdef _ADDCHECK_
    if(low<0)
        return -1;
    if(low>high)
        return -2;
#endif
    while (low <= high)
    {
        cf_int64 middle = low + (high - low)/2;
        if ( target < a[middle] )
            low = middle + 1;
        else if ( a[middle] < target )
            high = middle - 1;
        else
        {
            insertIndex =middle;
            return 1;
        }
    }

    insertIndex =low;
    return 0;
}


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_BINARY_SEARCH_HPP_

