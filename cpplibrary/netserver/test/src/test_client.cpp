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

int g_threadscnt =1;
int g_times =1;
int g_reqsize =8192;
std::string g_bufrecv;
std::vector < std::string > g_vecStr;
std::vector < cf_fd > g_vecSock;

#define HEADER_LEN 4

cf_void InitVecStr()
{
    //    CF_PRINT_FUNC;
    srand(time(NULL));
    std::string tmp;
    char tmp1[2];
    int r =0;
    for(int i=0; i<g_times; i++)
    {
        tmp ="";
        for(int j=0; j<g_reqsize; j++)
        {
            r =rand()%26+'A';
            memset(tmp1,0,sizeof tmp1);
            snprintf(tmp1,sizeof tmp1,"%c",r);
            tmp.append(tmp1);
        }

        cf_uint32 bodylen =htonl(tmp.size());
        std::string buf(HEADER_LEN+tmp.size(),'\0');
        memcpy(&buf[0],&bodylen,HEADER_LEN);
        memcpy((cf_char *)(&buf[0])+HEADER_LEN,tmp.c_str(),tmp.size());

        g_vecStr.push_back(buf);
    }
}

cf_void InitVecConnection()
{
    for(int i=0; i<g_threadscnt; i++)
    {
        cf_int sockfd;
        struct sockaddr_in servaddr;
        sockfd=cf_socket(AF_INET, SOCK_STREAM, 0);
        if(-1==sockfd)
            CF_ERR("cf_socket");
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_port=htons(8601);
        inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
        cf_int rt =cf_connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if(-1==rt)
        {
            CF_ERR("cf_connect")
        }
        g_vecSock.push_back(sockfd);
    }
}


cf_pvoid Run(void * p)
{
    CF_PRINT_FUNC;
    int index =*((int *)p);
    cf_fd sockfd=g_vecSock[index];
    //    printf("index=%d,sockfd=%d \n",index,sockfd);
    ssize_t hasSent =0;
    ssize_t hasRecv =0;
    ssize_t shouldRecv =0;
    bool peerClosedWhenRead =false;

    cf_uint64 seconds =0;
    cf_uint32 useconds =0;
    cf::Gettimeofday(seconds, useconds);

    for(int k=0; k<g_times; k++) // send times
    {
        std::string & body =g_vecStr[k];
        hasSent =0;
        bool succ =cf::SendSegmentSync(sockfd,body.c_str(), body.size(),hasSent,8000,
                                       body.size());
        if(succ)
        {
#if 0
            fprintf(stderr,"Sent succeeded ! hasSent=%d ,k=%d ,",int(hasSent),k);
            fprintf(stderr,"buff=%s \n",body.c_str()+HEADER_LEN);
#endif
        }
        else
            fprintf(stderr,"Warning,Send timeout ! \n");
        if(hasSent!=(cf_uint32)body.size())
            fprintf(stderr,"Warning,Recved len{%u}!=body.size(){%u}! \n",
                    (cf_uint32)hasSent,(cf_uint32)(body.size()));

        peerClosedWhenRead =false;
        hasRecv =0;
        shouldRecv =hasSent-HEADER_LEN;
        succ =cf::RecvSegmentSync(sockfd,&g_bufrecv[0], shouldRecv,hasRecv,
                                  peerClosedWhenRead,8000);
        if(succ)
        {
#if 0
            fprintf(stderr,"Recv succeeded ! hasRecv=%d ,k=%d ,",int(hasRecv),k);
            fprintf(stderr,"buff=%s \n",g_bufrecv.c_str());
#endif
        }
        else
            fprintf(stderr,"Warning,Recv timeout ! \n");
        if(hasRecv!=shouldRecv)
            fprintf(stderr,
                    "Warning,Recved hasRecv{%u}!=shouldRecv{%u}! peerClosedWhenRead=%u \n",
                    (cf_uint32)hasRecv,(cf_uint32)shouldRecv,cf_uint32(peerClosedWhenRead));
    }

#if 0
    fprintf(stderr,"cf_close ! tid=%u\n",(cf_uint32)pthread_self());
#endif
    cf_close(sockfd);
    return NULL;
}

cf_int main(cf_int argc,cf_char * argv[])
{
    //    CF_PRINT_FUNC;
    if(argc<4)
    {
        printf("Usage: program <threads count> <times per thread> <requset size> \n"
               "e.g. ./test_client 1 10000 8192 \n");
        return 1;
    }
    g_threadscnt =atoi(argv[1]);
    g_times =atoi(argv[2]);
    g_reqsize =atoi(argv[3]);
    g_bufrecv.resize(g_reqsize);

    cf_uint64 seconds =0;
    cf_uint32 useconds =0;
    cf::Gettimeofday(seconds, useconds);
    fprintf (stderr, "main stated ,g_threadscnt=%u,g_times=%u,"
             "g_reqsize=%u,time=%llu.%06u\n"
             "Going to generate %u random strings for every requeset in each thread \n",
             (cf_uint32)g_threadscnt,(cf_uint32)g_times,
             (cf_uint32)g_reqsize,seconds,useconds,
             (cf_uint32)g_times);

    InitVecStr();
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
