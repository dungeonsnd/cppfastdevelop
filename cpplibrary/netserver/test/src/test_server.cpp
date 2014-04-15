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

#include "netserver/cl_server.hpp"

int g_server_port =18600;
int g_numprocess =0;
int g_connections_per_process =0;
const int g_timeout_msec =20*1000;


namespace testserverdefs
{
enum
{
    MAX_BODY_SIZE =819200
};
} // namespace serverdefs

class IOCompleteHandler : public cl::EventHandler
{
public:
    IOCompleteHandler():_headLen(4)
    {
    }
    ~IOCompleteHandler()
    {
    }

    cf_void AddClientfds(std::vector < cf_fd > fds)
    {
        for(std::vector < cf_fd >::const_iterator it =fds.begin(); it!=fds.end(); it++)
        {
            fprintf (stdout, "AddClientfds\n");
            AddNewConn(*it,"127.0.0.1",g_server_port,true);
            _clientfds.insert(*it);
        }
    }

    cf_void OnAcceptComplete(cf_fd listenfd,cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
#if 0
        fprintf (stdout, "OnAcceptComplete,listenfd=%d,fd=%d,addr=%s \n",
                 listenfd,session->Fd(),session->Addr().c_str());
#endif
        AsyncRead(session->Fd(), _headLen);
        _recvHeader[session->Fd()] =true;
    }
    cf_void OnReadComplete(cf::T_SESSION session,
                           std::shared_ptr < cl::ReadBuffer > readBuffer)
    {
        CF_PRINT_FUNC;
#if 0
        fprintf (stdout, "OnReadComplete,fd=%d,addr=%s,total()=%d,buf=%s \n",
                 session->Fd(),session->Addr().c_str(),readBuffer->GetTotal(),
                 (cf_char *)(readBuffer->GetBuffer()));
#endif
        //        printf("pid=%d \n",int(getpid()));
        cf_uint32 totalLen =readBuffer->GetTotal();
        if(_recvHeader[session->Fd()])
        {
            if(_headLen!=totalLen)
            {
#if CF_SWITCH_PRINT
                //              fprintf (stdout, "OnReadComplete,fd=%d,_headLen{%u}!=totalLen{%u}  \n",session->Fd(),_headLen,totalLen);
#endif
            }
            else
            {
                cf_uint32 * p =(cf_uint32 *)(readBuffer->GetBuffer());
                cf_uint32 size =ntohl(*p);
                if(size<=testserverdefs::MAX_BODY_SIZE)
                {
                    _recvHeader[session->Fd()] =false;
                    if(size>0)
                        AsyncRead(session->Fd(), size);
                    else
                        _THROW(cf::ValueError, "size==0 !");
                }
                else
                {
#if 1
                    AsyncClose(session->Fd());
                    fprintf (stdout, "Warning, OnReadComplete,fd=%d,size{%u}>MAX_BODY_SIZE{%u}  \n",
                             session->Fd(),size,testserverdefs::MAX_BODY_SIZE);
#endif
                }
            }
        }
        else
        {
            _recvHeader[session->Fd()] =true;
            int t=htonl(totalLen);
            std::string bd(totalLen+4,'\0');
            memcpy(&bd[0],&t,sizeof(int));
            memcpy(&bd[4],readBuffer->GetBuffer(),totalLen);
            AsyncWrite(session->Fd(), bd.c_str(), bd.size());
#if 0
            fprintf (stdout, "AsyncWrite ,fd=%d,bufSize=%d, pid=%u \n",session->Fd(),
                     int(bd.size()),(cf_uint32)getpid());
#endif
            AsyncRead(session->Fd(), _headLen);
        }
    }
    cf_void OnWriteComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }

    virtual cf_void OnCloseComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnTimeoutComplete()
    {
        CF_PRINT_FUNC;
    }
    virtual cf_void OnErrorComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
    }

private:
    std::map < cf_fd , bool > _recvHeader;
    const cf_uint32 _headLen;
    std::unordered_set < cf_fd > _clientfds;
    std::unordered_set < cf_fd > _notifyPipe;
};

typedef cl::TcpServer < IOCompleteHandler > ServerType;

cf_void Work(cf_fd severfd, cf_fd severfd1, int i)
{
    IOCompleteHandler ioHandler;
    std::shared_ptr < ServerType > server;

    cl::T_LISTENFD_MAXCONN listenfd_maxconn;
    listenfd_maxconn[severfd] =g_connections_per_process;
    listenfd_maxconn[severfd1] =g_connections_per_process;
    server.reset(new ServerType(listenfd_maxconn,ioHandler,
                                i,g_timeout_msec));
    std::vector<cf_fd> clientfds;
    int clientSum =0;
    cf::ConnectToServer("127.0.0.1",g_server_port,clientSum,clientfds);
    ioHandler.AddClientfds(clientfds);
    printf("parent pid=%d, child pid=%d,i=%d\n",int(getppid()),int(getpid()),i);
    server->Start();
}

cf_void Run()
{
    cf_fd severfd =ServerType::CreateListenSocket(g_server_port,true);
    cf_fd severfd1 =ServerType::CreateListenSocket(g_server_port+1,true);

    if(0==g_numprocess)
    {
        Work(severfd,severfd1,0);
        return ;
    }

    std::vector < pid_t > pids;
    pid_t pid;
    for(int i=0; i<g_numprocess; i++)
    {
        pid =fork();
        if(pid<0)
        {
            printf("fork error,error=%s \n",strerror(errno));
            break;
        }
        else if(0==pid)
        {
            Work(severfd,severfd1,i);
            return ;
        }
        else
        {
            pids.push_back(pid);
            continue;
        }
    }

    int status =0;
    for(int i=0; i<int(pids.size()); i++)
    {
        waitpid(pids[i],&status,WUNTRACED | WCONTINUED);
    }
}

cf_int main(cf_int argc,cf_char * argv[])
{
    if(argc<4)
    {
        printf("Usage:%s <server port> <process count> <connections per process> \n",
               argv[0]);
        return 0;
    }
    g_server_port =atoi(argv[1]);
    g_numprocess =atoi(argv[2]);
    g_connections_per_process =atoi(argv[3]);
    Run();
    return 0;
}
