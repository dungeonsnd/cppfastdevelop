
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <string>

int main(int argc,char * argv[])
{
    if(argc<5)
    {
        printf("Usage: merge outputFile fromNumber toNumer maxBytesPerfile\n\n");
        printf("This program help you to merge some files to one file.\n"
        "e.g. ./merge CentOS-6.2-x86_64-bin-DVD2.iso 0 125 10485760 \n");
        return 1;
    }
    
    std::string outputFile(argv[1]);
    int fromNumber =atoi(argv[2]);
    int toNumer =atoi(argv[3]);
    int maxBytes =atoi(argv[4]);
    
    int outputFd =open(outputFile.c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
    
    ssize_t n =0;
    char * buf =new char [maxBytes];
    std::string ext(16,'\0');
    for(int i=fromNumber; i<=toNumer; i++)
    {
        memset(&ext[0],0,ext.size());
        snprintf(&ext[0],ext.size(),"%d",i);
        std::string inputFile(outputFile);
        inputFile +=ext;
        
        int inputFd =open(inputFile.c_str(),O_RDONLY);
        n=read(inputFd,buf,maxBytes);  
        write(outputFd,buf,n);
        close(inputFd);    
    }
    
    delete [] buf;
    close(outputFd);
    return 0;
}
