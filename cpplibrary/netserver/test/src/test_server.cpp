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

class IOCompleteHandler : public cl::EventHandler
{
public:
    IOCompleteHandler():_headLen(4)
    {
    }
    ~IOCompleteHandler()
    {
    }
    cf_void OnAcceptComplete(cf::T_SESSION session)
    {
#if CFD_SWITCH_PRINT
        fprintf (stderr, "OnAcceptComplete,fd=%d,addr=%s \n",
                 session->Fd(),session->Addr().c_str());
#endif
#if CFD_SWITCH_PRINT
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ $$ before OnAcceptComplete ,time=%llu.%u \n",seconds,
                 useconds);
#endif
        AsyncRead(session->Fd(), _headLen);
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ before 1 ,time=%llu.%u \n",seconds,useconds);
#endif
        _recvHeader[session->Fd()] =true;
#if CFD_SWITCH_PRINT
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr, "+++ after OnAcceptComplete ,time=%llu.%u \n",seconds,
                 useconds);
#endif
    }
    cf_void OnReadComplete(cf::T_SESSION session,
                           std::shared_ptr < cl::ReadBuffer > readBuffer)
    {
#if CFD_SWITCH_PRINT
        //        fprintf (stderr, "OnReadComplete,fd=%d,addr=%s,total()=%d,buf=%s \n",
        //                 session->Fd(),session->Addr().c_str(),readBuffer->GetTotal(),
        //                 (cf_char *)(readBuffer->GetBuffer()));
#endif

#if CFD_SWITCH_PRINT
        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);
        fprintf (stderr,
                 "+++ before OnReadComplete ,time=%llu.%u \n",seconds,useconds);
#endif
        cf_uint32 totalLen =readBuffer->GetTotal();
        if(_recvHeader[session->Fd()])
        {
            if(_headLen!=totalLen)
            {
#if CFD_SWITCH_PRINT
                //                fprintf (stderr, "OnReadComplete,fd=%d,_headLen{%u}!=totalLen{%u} \n",
                //                         session->Fd(),_headLen,totalLen);
#endif
            }
            else
            {
#if CFD_SWITCH_PRINT
                fprintf (stderr, "OnReadComplete,fd=%d,_headLen{%u}==totalLen{%u} \n",
                         session->Fd(),_headLen,totalLen);
#endif
                cf_uint32 * p =(cf_uint32 *)(readBuffer->GetBuffer());
                cf_uint32 size =ntohl(*p);
                _recvHeader[session->Fd()] =false;
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr,
                         "+++ before AsyncRead ,time=%llu.%u \n",seconds,useconds);
#endif
                if(size>0)
                    AsyncRead(session->Fd(), size);
                else
                    _THROW(cf::ValueError, "size==0 !");
#if CFD_SWITCH_PRINT
                cf::Gettimeofday(seconds, useconds);
                fprintf (stderr,
                         "+++ after AsyncRead ,time=%llu.%u \n",seconds,useconds);
#endif
            }
        }
        else
        {
#if CFD_SWITCH_PRINT
            fprintf (stderr, "OnReadComplete,fd=%d,_flagRecvHeader==false \n" ,
                     session->Fd());
#endif
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr,
                     "+++ before AsyncWrite ,time=%llu.%u \n",seconds,useconds);
#endif
            _recvHeader[session->Fd()] =true;
            AsyncWrite(session->Fd(), readBuffer->GetBuffer(), totalLen);
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr,
                     "+++ before AsyncRead ,time=%llu.%u \n",seconds,useconds);
#endif
            AsyncRead(session->Fd(), _headLen);
#if CFD_SWITCH_PRINT
            cf::Gettimeofday(seconds, useconds);
            fprintf (stderr,
                     "+++ after AsyncRead ,time=%llu.%u \n",seconds,useconds);
#endif
        }
    }
    cf_void OnWriteComplete(cf::T_SESSION session)
    {
#if CFD_SWITCH_PRINT
        fprintf (stderr, "OnWriteComplete,fd=%d,addr=%s\n",
                 session->Fd(),session->Addr().c_str());
#endif
    }

    virtual cf_void OnCloseComplete(cf::T_SESSION session)
    {
#if CFD_SWITCH_PRINT
        fprintf (stderr, "OnCloseComplete,fd=%d,addr=%s\n",
                 session->Fd(),session->Addr().c_str());
#endif
    }
    virtual cf_void OnTimeoutComplete()
    {
#if CFD_SWITCH_PRINT
        fprintf (stderr, "OnTimeoutComplete\n");
#endif
    }
    virtual cf_void OnErrorComplete(cf::T_SESSION session)
    {
#if CFD_SWITCH_PRINT
        fprintf (stderr, "OnErrorComplete,fd=%d,addr=%s\n",
                 session->Fd(),session->Addr().c_str());
#endif
    }

private:
    std::map < cf_fd , bool > _recvHeader;
    const cf_uint32 _headLen;
};

cf_void Run()
{
    IOCompleteHandler ioHandler;
    typedef cl::TcpServer < IOCompleteHandler > ServerType;
    std::shared_ptr < ServerType > server(new ServerType(ioHandler,8601));
    server->Start();
}

cf_int main(cf_int argc,cf_char * argv[])
{
    Run();
    return 0;
}
