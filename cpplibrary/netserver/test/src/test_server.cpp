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
#include "cppfoundation/cf_io_utility.hpp"
#include "netserver/cl_event_loop.hpp"

class Complete : public cl::ns::IOComplete
{
public:
    ~Complete()
    {
    }
    void Initial(cl::ns::Epoll * epoll)
    {
        _epoll =epoll;
    }
    cf_void OnAcceptComplete(cf_int fd)
    {
        printf("OnAcceptComplete \n");
    }
    cf_void OnReadComplete(cf_int fd, cf_void * buff, cf_uint32 bytes)
    {
        printf("OnReadComplete \n");
    }
    cf_void OnWriteComplete(cf_int fd, cf_uint32 bytes)
    {
        printf("OnWriteComplete \n");
    }
    cf_void OnCloseComplete(cf_int fd)
    {
        printf("OnCloseComplete \n");
    }
    cf_void OnTimeoutComplete(cf_int fd)
    {
        printf("OnTimeoutComplete \n");
    }
    cf_void OnErrorComplete(cf_int fd)
    {
        printf("OnErrorComplete \n");
    }
private:
    cl::ns::Epoll * _epoll;
};

cf_void Run()
{

    int listenfd =cf::CreateServerSocket(8601,SOCK_STREAM,32);
    if(listenfd<0)
    {
        printf("CreateServerSocket failed !\n");
        exit(1);
    }

    Complete complete;
    cl::ns::Epoll * _epoll =new cl::ns::Epoll(listenfd,complete);
    complete.Initial(_epoll);
    _epoll->Wait(-1);
}

cf_int main(cf_int argc,cf_char * argv[])
{
    Run();
    return 0;
}
