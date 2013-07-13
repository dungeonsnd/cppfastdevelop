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


int g_times =1;
std::vector < std::string > g_vecStr;

#define REQ_SIZE 1024
cf_void InitVecStr()
{
    srand(time(NULL));
    std::string tmp;
    char tmp1[2];
    int r =0;
    for(int i=0; i<g_times; i++)
    {
        tmp ="";
        for(int j=0; j<REQ_SIZE; j++)
        {
            r =rand()%26+'A';
            memset(tmp1,0,sizeof tmp1);
            snprintf(tmp1,sizeof tmp1,"%c",r);
            tmp.append(tmp1);
        }
        g_vecStr.push_back(tmp);
    }
}

cf_pvoid Run(void *)
{
    cf_int sockfd;
    struct sockaddr_in servaddr;
    sockfd=cf_socket(AF_INET, SOCK_STREAM, 0);
    if(-1==sockfd)
        CF_ERR("cf_socket");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(8601);
    inet_pton(AF_INET, "192.168.1.70", &servaddr.sin_addr);
    cf_int rt =cf_connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(-1==rt)
    {
        CF_ERR("cf_connect")
    }
    else
        printf("connect successfully! \n");

    for(int k=0; k<g_times; k++) // send times
    {
        std::string body(g_vecStr[k]);
        cf_uint32 bodylen =htonl(body.size());
        std::string buf(4+body.size(),'\0');
        memcpy(&buf[0],&bodylen,4);
        memcpy((cf_char *)(&buf[0])+4,body.c_str(),body.size());

        ssize_t hasSent =0;
        bool succ =cf::SendSegmentSync(sockfd,buf.c_str(), buf.size(),hasSent,5000,
                                       buf.size());
        if(succ)
        {
            printf("Sent succeeded ! hasSent=%d ,k=%d ,",int(hasSent),k);
            //            printf("buff=%s \n",buf.c_str()+4);
            printf("\n");
        }
        else
            printf("Send timeout ! \n");


        bool peerClosedWhenRead =false;
        ssize_t hasRecv =0;
        std::string bufrecv(1024,'\0');
        succ =cf::RecvSegmentSync(sockfd,&bufrecv[0], hasSent-4,hasRecv,
                                  peerClosedWhenRead,2000);
        if(succ)
        {
            printf("Recv succeeded ! hasRecv=%d , k=%d ,",int(hasRecv),k);
            //            printf("buff=%s \n",bufrecv.c_str());
            printf("\n");
        }
        else
            printf("Recv timeout ! \n");
    }

    printf("cf_close ! tid=%u \n",(cf_uint32)pthread_self());
    cf_close(sockfd);
    return NULL;
}

cf_int main(cf_int argc,cf_char * argv[])
{
    if(argc<2)
    {
        printf("Usage: program <times/thread> \n");
        return 1;
    }
    g_times =atoi(argv[1]);
    InitVecStr();

    pthread_t tid[512];
    int num_threads =1;
    void * res;
    int s,tnum;
    for (tnum = 0; tnum < num_threads; tnum++)
    {
        s = pthread_create(&tid[tnum], NULL,&Run, NULL);
        if (s != 0)
            perror("pthread_create");
    }
    for (tnum = 0; tnum < num_threads; tnum++)
    {
        s = pthread_join(tid[tnum], &res);
        if (s != 0)
            perror("pthread_join");
        free(res);
    }

    return 0;
}
