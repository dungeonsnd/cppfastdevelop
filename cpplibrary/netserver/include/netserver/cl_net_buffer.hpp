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

#ifndef _HEADER_FILE_CFD_CL_CHANNEL_BUFFER_HPP_
#define _HEADER_FILE_CFD_CL_CHANNEL_BUFFER_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

namespace cl
{
namespace ns
{

class ChannelBuffer : public cf::Object
{
public:
    ChannelBuffer()
    {
    }
    ~ChannelBuffer()
    {
    }
    void SetReadTotal(cf_uint32 bytes)
    {
        _bytes2read =bytes;
        if(_buf2read.size()<_bytes2read)
            _buf2read.resize(_bytes2read);
    }
    void SetWriteTotal(const void * buff,cf_uint32 bytes)
    {
        _bytes2write =bytes;
        if(_bytes2write.size()<_bytes2write)
            _buf2write.resize(_bytes2write);
        memcpy(&_buf2write[0],buff,bytes);
    }
    void Append(cf_uint32 n)
    {
    }
    void Remove()
    {
    }
    
    void * GetReadPtr()
    {
        char * p =&_buf2read[0];
        return p+_bytesRead;
    }
    cf_uint32 GetReadLeft()
    {
        char * p =&_buf2read[0];
        return p+_bytesRead;
    }
private:
    std::string _buf2read;
    std::string _buf2write;
    cf_uint32 _bytes2read;
    cf_uint32 _bytes2write;
    cf_uint32 _bytesRead;
    cf_uint32 _bytesWritten;
};

} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_CHANNEL_BUFFER_HPP_
