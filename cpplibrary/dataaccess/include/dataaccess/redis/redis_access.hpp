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

#ifndef _HEADER_FILE_REDIS_ACCESS_HPP_
#define _HEADER_FILE_REDIS_ACCESS_HPP_

#include <hiredis.h>

namespace cl
{

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

} // namespace cl

#endif // _HEADER_FILE_REDIS_ACCESS_HPP_

