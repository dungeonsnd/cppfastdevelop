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

#include "netserver/cl_buffer.hpp"

namespace cl
{


cf_void ReadBuffer::Clear()
{
    CF_PRINT_FUNC;
    _buf.clear();
    _total =0;
    _already =0;
}

cf_int ReadBuffer::Read(cf::T_SESSION session, bool & peerClosedWhenRead)
{
    CF_PRINT_FUNC;
    cf_char * p =&_buf[0];
    cf_int rdn =session->RecvAsync(p+_already, GetLeft(),peerClosedWhenRead);

#if CF_SWITCH_PRINT
    std::string s1 =cf::String2Hex(p+_already,rdn);
    fprintf (stdout, "<<<< Read ,fd=%d,RecvAsync=%s \n", session->Fd(), s1.c_str());
#endif

    _already +=rdn;
    return rdn;
}
cf_void ReadBuffer::SetTotal(cf_uint32 total)
{
    if(_total)
        _THROW_FMT(cf::ValueError, "_total{%u}!=0 !", _total);
    if(total>_total)
        _buf.resize(total);
    _total =total;
}
cf_uint32 ReadBuffer::GetLeft() cf_const
{
    if(_total>_already)
        return _total-_already;
    else
        _THROW_FMT(cf::ValueError, "_total{%u}<=_already{%u} !", _total,_already);
}
bool ReadBuffer::IsComplete() cf_const
{
    if(0==_total)
        _THROW(cf::ValueError, "0==_total !");
    return _total==_already;
}


cf_void WriteBuffer::Clear()
{
    CF_PRINT_FUNC;
    _buf.clear();
    _total =0;
    _already =0;
}

cf_int WriteBuffer::Write(cf::T_SESSION session)
{
    CF_PRINT_FUNC;
    cf_char * p =&_buf[0];
    cf_int rdn =session->SendAsync(p+_already, GetLeft());

#if CF_SWITCH_PRINT
    std::string s1 =cf::String2Hex(p+_already,rdn);
    fprintf (stdout, ">>>> Write ,fd=%d,SendAsync=%s \n", session->Fd(),
             s1.c_str());
#endif

    _already +=rdn;
    return rdn;
}
cf_void WriteBuffer::SetBuffer(cf_cpvoid buffer, cf_uint32 total)
{
    CF_PRINT_FUNC;
    if(_total)
        _THROW_FMT(cf::ValueError, "_total{%u}!=0 !", _total);
    if(total>_total)
        _buf.resize(total);
    memcpy(&_buf[0],buffer,total);
    _total =total;
}
cf_uint32 WriteBuffer::GetLeft() cf_const
{
    if(_total>_already)
        return _total-_already;
    else
        _THROW_FMT(cf::ValueError, "_total{%u}<=_already{%u} !", _total,_already);
}
bool WriteBuffer::IsComplete() cf_const
{
    if(0==_buf.size())
        _THROW(cf::ValueError, "0==_total !");
    return _total==_already;
}

} // namespace cl

