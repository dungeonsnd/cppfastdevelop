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

#include "cppfoundation/cf_utility_program.hpp"

namespace cf
{


cf_void SetProcessDaemon()
{
    cf_int i,fd0,fd1,fd2;
    pid_t pid;
    struct rlimit rl;

    cf_umask(0);
    if(cf_getrlimit(RLIMIT_NOFILE,&rl)<0)
    {
        printf("Can't get file limit! errno=%d.file:%s,function:%s,line:%d.\n",
               cf_int(errno),__FILE__,__PRETTY_FUNCTION__,__LINE__);
        cf_exit(1);
    }
    if ((pid=cf_fork())<0)
    {
        printf("Can't fork! errno=%d.file:%s,function:%s,line:%d.\n",cf_int(errno),
               __FILE__,__PRETTY_FUNCTION__,__LINE__);
        cf_exit(1);
    }
    else if(pid != 0) // parent will exit normally.
    {
        cf_exit(0);
    }

    cf_setsid();

    if ((pid=cf_fork())<0)
    {
        printf("Can't fork!! errno=%d.file:%s,function:%s,line:%d.\n",cf_int(errno),
               __FILE__,__PRETTY_FUNCTION__,__LINE__);
        exit(1);
    }
    else if (pid!=0)
    {
        cf_exit(0);
    }

    if (cf_chdir("/tmp")<0)
    {
        printf("Can't change directory to /tmp! errno=%d.file:%s,function:%s,line:%d.\n",
               cf_int(errno),__FILE__,__PRETTY_FUNCTION__,__LINE__);
        cf_exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max =1024;
    for (i=0; i < cf_int(rl.rlim_max); i++)
        cf_close(i);

    fd0 =cf_open("/dev/null",O_RDWR,0);
    fd1 =cf_dup(0);
    fd2 =cf_dup(0);
}

cf_void IgnoreSignals()
{
    for(cf_int i = 1; i < 64; i++ )
    {
        switch(i)
        {
        case(SIGBUS):
        case(SIGINT):
        case(SIGHUP):
        case(SIGSEGV):
        case(SIGABRT):
        case(SIGFPE):
        case(SIGUSR1):
        case(SIGUSR2):
        case(SIGKILL):
        case(SIGCHLD):
            break;
        default:
            {
                cf_signal(i,SIG_IGN);
                break;
            }
        }
    }
}

} // namespace cf
