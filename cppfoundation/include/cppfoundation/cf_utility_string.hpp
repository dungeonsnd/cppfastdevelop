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

#ifndef _HEADER_FILE_CFD_CF_UTILITY_STRING_HPP_
#define _HEADER_FILE_CFD_CF_UTILITY_STRING_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cf
{


inline cf_char * GenRandString(cf_char * str,const cf_uint32 len)
{
    srand(time(NULL));
    cf_uint32 i;
    for(i=0; i<len; ++i)
        str[i]='a'+rand()%26;
    return str;
}

inline std::string String2Hex(cf_cpstr input,cf_uint32 inputLength)
{
    if(inputLength<1)
        return "";
    std::string output;
    std::string everyone(3,'\0');
    for(cf_uint32 i =0; i<inputLength; i++)
    {
        memset(&everyone[0],0,everyone.size());
        snprintf(&everyone[0],everyone.size(),"%02x",input[i]);
        output.append (everyone.c_str(),everyone.size()-1);
    }
    return output;
}

inline std::string String2HexD(cf_cpstr input,cf_uint32 inputLength)
{
    if(inputLength<1)
        return "";
    std::string output;
    std::string everyone(5,'\0');
    for(cf_uint32 i =0; i<inputLength; i++)
    {
        memset(&everyone[0],0,everyone.size());
        snprintf(&everyone[0],everyone.size(),"\\x%02x",input[i]);
        output.append (everyone.c_str(),everyone.size()-1);
    }
    return output;
}



cf_void GenerateRandString(cf_uint32 stringLength, std::string & output);

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_UTILITY_STRING_HPP_

