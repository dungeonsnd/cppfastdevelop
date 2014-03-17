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
const int g_timeout_msec =2*3600*1000; // 2 hours.


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
            AddNewConn(*it,"127.0.0.1",g_server_port);
            _clientfds.insert(*it);
        }
    }

    cf_void OnAcceptComplete(cf::T_SESSION session)
    {
        CF_PRINT_FUNC;
#if 0
        fprintf (stdout, "OnAcceptComplete,fd=%d,addr=%s \n",
                 session->Fd(),session->Addr().c_str());
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
            AsyncWrite(session->Fd(), readBuffer->GetBuffer(), totalLen);
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


cf_void Run()
{
    typedef cl::TcpServer < IOCompleteHandler > ServerType;
    cf_fd severfd =ServerType::CreateListenSocket(g_server_port,true);

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
            IOCompleteHandler ioHandler;
            std::shared_ptr < ServerType > server;
            if(g_connections_per_process>0)
                server.reset(new ServerType(severfd,ioHandler,i,
                                            g_connections_per_process,g_timeout_msec));
            else
                server.reset(new ServerType(severfd,ioHandler,i,0,g_timeout_msec));
            std::vector<cf_fd> clientfds;
            int clientSum =0;
            cf::ConnectToServer("127.0.0.1",g_server_port,clientSum,clientfds);
            ioHandler.AddClientfds(clientfds);
            printf("parent pid=%d, child pid=%d,i=%d\n",int(getppid()),int(getpid()),i);
            server->Start();
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
