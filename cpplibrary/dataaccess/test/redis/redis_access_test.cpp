#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataaccess/redis/redis_access.hpp"

int main(int argc, char ** argv)
{
    cl::RedisConn rc("127.0.0.1", 6379, 3);

    cl::RedisReply rr(rc.execCommand("PING"));
    printf("PING: %s\n", rr.Reply()->str);

    rr.Reset(rc.execCommand("SET foo 'helloworld'"));
    printf("PING: %s\n", rr.Reply()->str);
    return 0;
}

