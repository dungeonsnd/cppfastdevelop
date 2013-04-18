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

#include "cppfoundation/cf_root.hpp"
#include "cppfoundation/cf_utility.hpp"
#include "netserver/cl_event_loop.hpp"


cf_void Run()
{
   cf_int sockfd;
   struct sockaddr_in servaddr;
   sockfd=cf_socket(AF_INET, SOCK_STREAM, 0);
   if(-1==sockfd)
      ERR("cf_socket")
      bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family=AF_INET;
   servaddr.sin_port=htons(8601);
   inet_pton(AF_INET, "192.168.1.8", &servaddr.sin_addr);
   cf_int rt =cf_connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
   if(-1==rt)
      ERR("cf_connect")

      std::string body("cppfastdevelop");
   cf_uint32 bodylen =htonl(body.size());
   std::string buf(4+body.size(),'\0');
   memcpy(&buf[0],&bodylen,4);
   memcpy((cf_char *)(&buf[0])+4,body.c_str(),body.size());

   ssize_t hasDone =0;
   bool succ =cf::SendSegmentSync(sockfd,buf.c_str(), buf.size(),hasDone,5000,
                                  buf.size());
   if(succ)
      printf("sent succeeded ! hasDone=%d \n",int(hasDone));
   else
      printf("send timeout ! \n");

   usleep(4*1000000);
   cf_close(sockfd);
}

cf_int main(cf_int argc,cf_char * argv[])
{
   Run();
   return 0;
}
