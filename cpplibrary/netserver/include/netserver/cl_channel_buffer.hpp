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
   ChannelBuffer():
      _asyncClose(false),
      _readTotal(0),
      _hasRead(0),
      _writeTotal(0),
      _hasWritten(0)

   {
   }
   ~ChannelBuffer()
   {
   }
   cf_void ClearRead()
   {
      _readTotal =0;
      _hasRead =0;
   }
   cf_void ClearWrite()
   {
      _writeTotal =0;
      _hasWritten =0;
   }

   cf_void SetAsyncClose()
   {
      _asyncClose =true;
   }
   bool GetAsyncClose()
   {
      return _asyncClose;
   }

   cf_void SetReadTotal(cf_uint32 bytes)
   {
      ClearRead();
      _readTotal =bytes;
      if(_buf2read.size()<_readTotal)
         _buf2read.resize(_readTotal);
   }
   cf_uint32 GetReadTotal() const
   {
      return _readTotal;
   }
   cf_pvoid GetReadCurrentPtr()
   {
      cf_char * p =&_buf2read[0];
      return p+_hasRead;
   }
   cf_uint32 GetHasRead() const
   {
      return _hasRead;
   }
   cf_uint32 GetReadLeft() const
   {
      return  _readTotal-_hasRead;
   }
   cf_pvoid GetReadBuffer()
   {
      return &_buf2read[0];
   }
   cf_void AppendReadCount(cf_uint32 n)
   {
      _hasRead +=n;
   }


   cf_void SetWrittenTotal(cf_void * buff,cf_uint32 bytes)
   {
      ClearWrite();
      _writeTotal =bytes;
      if(_buf2write.size()<_writeTotal)
         _buf2write.resize(_writeTotal);
      memcpy(&_buf2write[0],buff,bytes);
   }
   cf_uint32 GetWrittenTotal() const
   {
      return _writeTotal;
   }
   cf_pvoid GetWrittenCurrentPtr()
   {
      cf_char * p =&_buf2write[0];
      return p+_hasWritten;
   }
   cf_uint32 GetHasWritten() const
   {
      return _hasWritten;
   }
   cf_uint32 GetWrittenLeft() const
   {
      return  _writeTotal-_hasWritten;
   }
   cf_pvoid GetWrittenBuffer()
   {
      return &_buf2write[0];
   }
   cf_void RemoveWrittenCount(cf_uint32 n)
   {
      _hasWritten -=n;
   }

private:
   bool _asyncClose;

   std::string _buf2read;
   cf_uint32 _readTotal;
   cf_uint32 _hasRead;

   std::string _buf2write;
   cf_uint32 _writeTotal;
   cf_uint32 _hasWritten;
};

} // namespace ns
} // namespace cl

#endif // _HEADER_FILE_CFD_CL_CHANNEL_BUFFER_HPP_
