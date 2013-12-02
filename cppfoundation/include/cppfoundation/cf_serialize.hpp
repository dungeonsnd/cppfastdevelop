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

#ifndef _HEADER_FILE_CFD_CF_SERIALIZE_HPP_
#define _HEADER_FILE_CFD_CF_SERIALIZE_HPP_

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_exception.hpp"

#define __PROTO__
//#define __THRIFT__

#ifdef __THRIFT__
#include <thrift/config.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransportUtils.h>
#endif // __THRIFT__

namespace cf
{

#ifdef __PROTO__
template<typename ProtoStruct>
bool ProtoToString(cf_const ProtoStruct & fmsg, std::string & output)
{
    return fmsg.SerializeToString(&output);
}

template<typename ProtoStruct>
bool StringToProto(ProtoStruct & fmsg, cf_cpstr buf, cf_int bufLen)
{
    return fmsg.ParseFromArray(buf,bufLen);
}
#endif // __PROTO__

#ifdef __THRIFT__
template<typename ThriftStruct>
bool ThriftToString(cf_const ThriftStruct & ts, std::string & output)
{
    using namespace apache::thrift::transport;
    using namespace apache::thrift::protocol;
    shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer());
    shared_ptr<TProtocol> protocol(new TBinaryProtocol(membuffer));
    ts.write(protocol.get());
    uint8_t * buf;
    uint32_t size;
    membuffer->getBuffer(&buf, &size);
    output.assign((cf_char *)(buf), size_t(size));
    return true;
}

template<typename ThriftStruct>
bool StringToThrift(ThriftStruct & ts, cf_cpstr buf, cf_int bufLen)
{
    using namespace apache::thrift::transport;
    using namespace apache::thrift::protocol;
    TMemoryBuffer * pmembuffer = new TMemoryBuffer;
    pmembuffer->write((cf_const uint8_t *)buf, size_t(bufLen));
    shared_ptr<TMemoryBuffer> membuffer(pmembuffer);
    shared_ptr<TProtocol> protocol(new TBinaryProtocol(membuffer));
    ts.read(protocol.get());
    return true;
}
#endif // __THRIFT__

} // namespace cf

#endif // _HEADER_FILE_CFD_CF_SERIALIZE_HPP_

