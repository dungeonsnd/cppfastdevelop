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
#include "cppfoundation/cf_network.hpp"
#include "cppfoundation/cf_compress.hpp"

std::string g_server_host;
int g_server_port =18600;
int g_threadscnt =1;
int g_times =1;
int g_reqsize =8192;
std::vector < cf_fd > g_vecSock;

#define HEADER_LEN 4
#define SERVER_IP "127.0.0.1"


cf_void InitVecConnection()
{
    cf::ConnectToServer(g_server_host.c_str(),g_server_port,g_threadscnt,g_vecSock);
}

#define ID_SIZE 16
#define HEADER_SIZE 4+4+ID_SIZE
cf_void PackHeader(const char * body,unsigned bodyLen,const char * id,
                   char * header)
{
    int ad =cf::CalAdler32(body,bodyLen);
    bodyLen =htonl(bodyLen+HEADER_SIZE-4);
    memcpy(header,&bodyLen,4);
    ad =htonl(ad);
    memcpy(header+4,&ad,4);
    memcpy(header+8,id,ID_SIZE);
}

bool CheckResult(const char * header,const char * body,unsigned bodyLen)
{
    int ad =*( (int *)(&header[4]) );
    int adlerBody =cf::CalAdler32(body,bodyLen);
    adlerBody =ntohl(adlerBody);
    return ad==adlerBody;
}

cf_pvoid Run(void * p)
{
    try
    {
        int index =*((int *)p);
        cf_fd sockfd=g_vecSock[index];
        //    printf("index=%d,sockfd=%d \n",index,sockfd);
        ssize_t hasSent =0;
        ssize_t hasRecv =0;
        ssize_t shouldRecv =0;
        bool peerClosedWhenRead =false;
        int timeoutMsec =8000;

        std::string bufsend(g_reqsize+HEADER_SIZE,'\0');
        std::string bufrecv(g_reqsize+HEADER_SIZE,'\0');

        cf_uint64 seconds =0;
        cf_uint32 useconds =0;
        cf::Gettimeofday(seconds, useconds);

        /// Going to send.
        for(int k=0; k<g_times; k++) // send times
        {
            cf::GenRandString(&bufsend[HEADER_SIZE],g_reqsize);
            char header[HEADER_SIZE];
            char id[ID_SIZE+1];
            snprintf(id,sizeof(id),"[%6d ,%6d]",index,k);
            PackHeader(&bufsend[HEADER_SIZE],g_reqsize,id,header);
            memcpy(&bufsend[0],header,sizeof(header));
            hasSent =0;
            bool succ =cf::SendSegmentSync(sockfd,
                                           bufsend.c_str(), bufsend.size(),
                                           hasSent,timeoutMsec,bufsend.size());

            if(succ)
            {
#if 0
                fprintf(stderr,"Sent succeeded ! hasSent=%d ,k=%d ,",int(hasSent),k);
                std::string s1 =cf::String2Hex(bufsend.c_str(),bufsend.size());
                fprintf(stderr,"buff=%s \n",s1.c_str());
                printf("data sent=%s \n",&bufsend[8]);
#endif
            }
            else
                fprintf(stderr,"Warning,Send timeout ! \n");
            if(hasSent!=(cf_uint32)bufsend.size())
                fprintf(stderr,"Warning,hasSent len{%u}!=bufsend.size(){%u}! \n",
                        (cf_uint32)hasSent,(cf_uint32)(bufsend.size()));
        }

        /// Going to recv.
        for(int k=0; k<g_times; k++)
        {
            peerClosedWhenRead =false;
            hasRecv =0;
            shouldRecv =bufsend.size();
            bool succ =cf::RecvSegmentSync(sockfd,&bufrecv[0], shouldRecv,hasRecv,
                                           peerClosedWhenRead,timeoutMsec);
            if(succ)
            {
#if 0
                fprintf(stderr,"Recv succeeded ! hasRecv=%d ,k=%d ,",int(hasSent),k);
                std::string s1 =cf::String2Hex(bufrecv.c_str(),bufrecv.size());
                fprintf(stderr,"buff=%s \n",s1.c_str());
                printf("data recv=%s \n",&bufrecv[8]);
#endif
            }
            else
                fprintf(stderr,"Warning,Recv timeout ! \n");
            if(hasRecv!=shouldRecv)
                fprintf(stderr,
                        "Warning,Recved hasRecv{%u}!=shouldRecv{%u}! peerClosedWhenRead=%u \n",
                        (cf_uint32)hasRecv,(cf_uint32)shouldRecv,cf_uint32(peerClosedWhenRead));
            else
            {
                bool eql =CheckResult(&bufrecv[0],&bufrecv[HEADER_SIZE],g_reqsize);
                if( !eql )
                {
                    std::string s1 =cf::String2Hex(bufrecv.c_str(),bufrecv.size());
                    fprintf(stderr,
                            "****** tid=%d,Warning,sockfd=%d, Sent data != Recv data(%s)\n",
                            index,sockfd,s1.c_str());
                    break;
                }
            }
        }

#if 0
        fprintf(stderr,"cf_close ! tid=%u\n",(cf_uint32)pthread_self());
#endif
        cf_close(sockfd);
    }
    catch (cf::BaseException & e)
    {
        printf("Caught exception:\n%s\n",e.what());
    }
    catch (...)
    {
        printf("Caught unknow exception.\n");
    }
    return NULL;
}

cf_int main(cf_int argc,cf_char * argv[])
{
    //    CF_PRINT_FUNC;
    if(argc<6)
    {
        printf("Usage: %s <server host> <server port> <threads count> <sending times per thread> <requset size> \n",
               argv[0]);
        return 1;
    }
    g_server_host =argv[1];
    g_server_port =atoi(argv[2]);
    g_threadscnt =atoi(argv[3]);
    g_times =atoi(argv[4]);
    g_reqsize =atoi(argv[5]);

    cf_uint64 seconds =0;
    cf_uint32 useconds =0;
    cf::Gettimeofday(seconds, useconds);
    fprintf (stderr, "main stated ,g_threadscnt=%u,g_times=%u,"
             "g_reqsize=%u,time=%llu.%06u\n"
             "Going to generate %u random strings for every requeset in each thread \n",
             (cf_uint32)g_threadscnt,(cf_uint32)g_times,
             (cf_uint32)g_reqsize,seconds,useconds,
             (cf_uint32)g_times);

    cf::Gettimeofday(seconds, useconds);
    fprintf (stderr, "After generate random string,time=%llu.%06u \n"
             "Going to init %u connections \n\n",
             seconds,useconds,(cf_uint32)g_threadscnt);
    InitVecConnection();

    pthread_t tid[512];
    void * res;
    int s,tnum;
    std::vector < int > index(g_threadscnt);

    cf_uint64 seconds1 =0;
    cf_uint32 useconds1 =0;
    cf::Gettimeofday(seconds1, useconds1);
    fprintf (stderr, "+++ Threads starting,time=%llu.%06u \n",
             seconds1,useconds1);

    int idex =0;
    if(1==g_threadscnt)
    {
        Run(&idex);
    }
    else
    {
        for (tnum = 0; tnum < g_threadscnt; tnum++)
        {
            index[tnum] =tnum;
            s = pthread_create(&tid[tnum], NULL,&Run, &(index[tnum]));
            if (s != 0)
                perror("pthread_create");
        }
        for (tnum = 0; tnum < g_threadscnt; tnum++)
        {
            s = pthread_join(tid[tnum], &res);
            if (s != 0)
                perror("pthread_join");
            free(res);
        }
    }

    cf_uint64 seconds2 =0;
    cf_uint32 useconds2 =0;
    cf::Gettimeofday(seconds2, useconds2);
    fprintf (stderr, "+++ Thread stopped ,time=%llu.%06u \n",
             seconds2,useconds2);

    cf_uint64 resultUS =(seconds2-seconds1)*1000000+useconds2-useconds1;
    printf("Acctual using time=%llu.%06u seconds \n",
           (cf_uint64)(resultUS/1000000),(cf_uint32)(resultUS%1000000));
    printf("QPS=%d\n",
           int(double(g_threadscnt)*g_times/(resultUS/1000000.0)));

    return 0;
}
