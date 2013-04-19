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

#ifndef _HEADER_FILE_CFD_CL_QUEUE_DEF_HPP_
#define _HEADER_FILE_CFD_CL_QUEUE_DEF_HPP_

#include "cppfoundation/cf_exception.hpp"

namespace cl
{
namespace cq
{
/*
enum eQueueScene{
    QUEUE_SCENE1=1, // head > tail
    QUEUE_SCENE2=2, // head < tail
    QUEUE_SCENE3=3, // head == tail,count=0
    QUEUE_SCENE4=4 // head == tail,count!=0
};

enum eWriteCase{
    QUEUE_CASE1=1, // over-head>sizeof(Element) , write all element.
    QUEUE_CASE2=2, // over-head=sizeof(Element) , write all element.
    QUEUE_CASE3=3, // over-head<sizeof(ElementHeader) , only write some header.
    QUEUE_CASE4=4, // over-head>sizeof(ElementHeader) && over-head<sizeof(Element), only write some body.
    QUEUE_CASE5=5 // over-head=sizeof(ElementHeader) , only write header.
};

Scene 1:
    _pQueue    tail                    head   over
    |(unused)   |         (used)         | (unus|ed)
    +++++++++++++------------------------+++++++

Scene 2:
   _pQueue                            head        tail    over
     |      (used)                      |(unused)  |  (used) |
     -----------------------------------++++++++++++---------
*/

typedef enum _tagQStatus
{
    Q_NOMAL =0,
    Q_STOP2PUT,
    Q_STOP2PUTANDGET
} QStatus;

typedef struct _tagQMeta
{
    cf_byte magic[4];
    cf_byte version[4];

    cf_uint64 qTotalSize; // alse named "over". Not including Meta.
    cf_uint64 qLeftSize;
    cf_uint64 elementCount;

    cf_uint64 headIndex;
    cf_uint64 tailIndex;

    cf_uint64 serialNO;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    time_t operationTime;
    pid_t operationPid;
    cf_byte extention[4];

    QStatus status;

    cf_byte elementBegin[0];
} QMeta,*PQMeta;

typedef struct _tagElementHeader
{
    cf_byte writeCase;
    cf_byte extention[3];
    cf_uint64 bodyLen;
    time_t operationTime;

    cf_byte bodyBegin[0];
} ElementHeader,*PElementHeader;


DEFINECLASS_CHILD_EXCEPTION(QueueFullException, cf::Info);
DEFINECLASS_CHILD_EXCEPTION(SizeTooSmallException, cf::Info);
DEFINECLASS_CHILD_EXCEPTION(StopPutException, cf::Info);
DEFINECLASS_CHILD_EXCEPTION(StopPutAndGetException, cf::Info);

} // namespace cq
} // namespace cl
#endif // _HEADER_FILE_CFD_CL_QUEUE_DEF_HPP_

