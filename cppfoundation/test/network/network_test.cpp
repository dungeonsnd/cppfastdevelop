
#include "cppfoundation/cf_network.hpp"

int main(int argc,char * argv[])
{
    cf_uint32 u =0;
    cf::IPString2Int("192.168.111.70",u);
    printf("%u \n",u);
    std::string s =cf::IPInt2String(u);
    printf("%s \n",s.c_str());
    return 0;
}
