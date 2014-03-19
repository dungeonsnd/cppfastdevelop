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

#ifndef _HEADER_FILE_CFD_CF_COMPRESS_HPP_
#define _HEADER_FILE_CFD_CF_COMPRESS_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

#define __ZLIB__

#ifdef __ZLIB__
#include "zlib.h"
#endif // __ZLIB__

namespace cf
{

#ifdef __ZLIB__
/*
src [in]
srcLen [in]
dest [out]
destLen [out]
compressLevel [in]

compressLevel Copy from zlib.h
#define Z_NO_COMPRESSION 0
#define Z_BEST_SPEED 1
#define Z_BEST_COMPRESSION 9
#define Z_DEFAULT_COMPRESSION (-1)
*/
inline cf_int Compress(cf_cpstr src, cf_ulong srcLen,
                       std::string & dest, cf_ulong & destLen,
                       cf_int compressLevel =Z_DEFAULT_COMPRESSION)
{
    destLen =compressBound(srcLen);
    dest.resize(destLen);
    return compress2((Bytef *)(&dest[0]),(uLongf *)(&destLen),
                     (Bytef *)src,(uLong)srcLen, compressLevel);
}


/*
src [in]
srcLen [in]
dest [out]
destLen [in,out]
compressLevel [in]
*/
inline cf_int Uncompress(cf_cpvoid src, cf_ulong srcLen, cf_pvoid dest,
                         cf_ulong & destLen)
{
    return uncompress((Bytef *)dest,(uLongf *)(&destLen),
                      (Bytef *)src,(uLong)srcLen);
}

inline cf_int CalAdler32(cf_cpstr src,cf_uint32 srcLen)
{
    uLong adler = adler32(0L, Z_NULL, 0);
    return adler32(adler, (const Bytef *)src, srcLen);
}

#endif // __ZLIB__


} // namespace cf

#endif // _HEADER_FILE_CFD_CF_COMPRESS_HPP_

