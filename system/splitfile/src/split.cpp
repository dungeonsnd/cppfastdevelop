
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

int main(int argc,char * argv[])
{
    if(argc<3)
    {
        printf("Usage: split inputFile BytesPerFile \n");
        printf("This program help you to splite one file to some files.\n"
        "e.g. ./split CentOS-6.2-x86_64-bin-DVD2.iso 10485760 \n");
        return 1;
    }
    
    std::string filename(argv[1]);
    int mb2Read =atoi(argv[2]);
    
    int fd =open(filename.c_str(),O_RDONLY);
    
    ssize_t n =0;
    char * buf =new char [mb2Read];
    
    int k =0;
    do{
        std::string ext(16,'\0');
        snprintf(&ext[0],ext.size(),"%d",k);
        std::string tname(filename);
        tname +=ext;
        
        n=read(fd,buf,mb2Read);        
        int tfd =open(tname.c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
        write(tfd,buf,n);
        close(tfd);
        k++;
    }while( mb2Read==n );
    
    delete [] buf;
    close(fd);    
    return 0;
}
