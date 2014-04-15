
#include "cppfoundation/cf_utility_program.hpp"

int main(int argc,char * argv[])
{
    char p [MAX_SIZE];
    cf_int32 last1;
    cf_int32 last2;
    std::string f ="/1/3/56.8";
    cf::GetIndexByPathNameExt( f.c_str(),last1,last2);
    printf("GetIndexByPathNameExt , last1=%d,last2=%d \n\n",last1,last2);

    f ="1234.6";
    cf::GetIndexByPathNameExt( f.c_str(),last1,last2);
    printf("GetIndexByPathNameExt , last1=%d,last2=%d \n\n",last1,last2);

    f ="/root/test/abc.txt/";
    printf("GetNameByPathNameExt = %s \n\n", cf::GetNameByPathNameExt( f.c_str(),
            f.size(),p) );
    f ="/root/test/abc.txt";
    printf("GetNameByPathNameExt = %s \n\n", cf::GetNameByPathNameExt( f.c_str(),
            f.size(),p) );

    f ="/root/test/abc.txt/";
    printf("GetPathByPathNameExt = %s \n\n", cf::GetPathByPathNameExt( f.c_str(),
            f.size(),p) );
    f ="/root/test/abc.txt";
    printf("GetPathByPathNameExt = %s \n\n", cf::GetPathByPathNameExt( f.c_str(),
            f.size(),p) );


    printf("GetExeAbsPath = %s \n\n", cf::GetExeAbsPath(p) );

    printf("GetExeName = %s \n\n", cf::GetExeName(p) );

    printf("GetExeParentAbsPath = %s \n\n", cf::GetExeParentAbsPath(p) );

    std::string pidFile =cf::GetExeParentAbsPath(p);
    pidFile +="/conf/utility_program_test.pid";
    printf("PID=%u \n",unsigned(getpid()));
    cf::PidFileWriter pw(pidFile.c_str(),pidFile.size());
    pw.Set();

    {
        std::string pidFile =cf::GetExeParentAbsPath(p);
        pidFile +="/conf/utility_program_test.pid";
        cf::PidFileWriter pw(pidFile.c_str(),pidFile.size());
        cf_uint32 pid =pw.Get();
        printf("PID Get=%u \n",pid);
    }

    return 0;
}
