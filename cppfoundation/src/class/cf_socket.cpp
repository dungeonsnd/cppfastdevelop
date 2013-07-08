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

#include "cppfoundation/cf_socket.hpp"
#include "cppfoundation/cf_network.hpp"

namespace cf
{


cf_int Socket::SendAsync(cf_cpvoid data, ssize_t len)
{
    return SendSegmentAsync(_fd,cf_cpstr(data), len);
}
cf_int Socket::RecvAsync(cf_pvoid data, ssize_t len)
{
    return RecvSegmentAsync(_fd,(cf_char * )(data), len);
}

} // namespace cf
