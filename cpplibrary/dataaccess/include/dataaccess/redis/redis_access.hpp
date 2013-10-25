#ifndef _HEADER_FILE_REDIS_ACCESS_HPP_
#define _HEADER_FILE_REDIS_ACCESS_HPP_

#include <hiredis.h>

class RedisReply
{
public:
    RedisReply(redisReply * reply):_reply(reply)
    {}
    void Reset(redisReply * reply)
    {
        freeReplyObject(_reply);
        _reply =reply;
    }
    ~RedisReply()
    {
        freeReplyObject(_reply);
    }
    redisReply * Reply()
    {
        return _reply;
    }
private:
    redisReply * _reply;
};

class RedisConn
{
public:
    RedisConn(const char * hostname,unsigned port,unsigned timeoutSec)
    {
        struct timeval timeout = { timeoutSec, 0 };
        _c = redisConnectWithTimeout(hostname, port, timeout);
        if (_c == NULL || _c->err)
        {
            if (_c)
            {
                printf("Connection error: %s\n", _c->errstr);
                redisFree(_c);
            }
            else
            {
                printf("Connection error: can't allocate redis context\n");
            }
            throw "";
        }
    }
    redisReply * execCommand(const char * cmd)
    {
        redisReply * reply = (redisReply *)redisCommand(_c,cmd);
        printf("SET: %s\n", reply->str);
        return reply;
    }
    ~RedisConn()
    {
        redisFree(_c);
    }
private:
    redisContext * _c;
};

#endif // _HEADER_FILE_REDIS_ACCESS_HPP_

