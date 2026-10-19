#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#define BUFFSIZE 1024
int main(int argc, char *argv[]){   
    if(argc!=2){
        printf("\nUsage: %s <ip addr>\n",argv[0]);
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in  serv_addr;

    int sock_fd=0,n=0;
    char recvbf[1024];
    
    //This here seems like a potential issue....but since this is just a test a try on the socket ,I will comw back and look at is.You sould definately not set a fixed buffer , what if it gets overflowed!!
    memset(&recvbf,'0',sizeof(recvbf));

    //create a socket with the socket call
    sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd==-1){
        fprintf(stderr,"\nError: %s occured!\n",strerror(errno));
        return EXIT_FAILURE;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    
    //htons helps converts the endiness of the port number given by the user to a standard format.
    serv_addr.sin_port=htons(5000);
    serv_addr.sin_family=AF_INET;
    
    //We now convert the ip string to its binary value  
    int val =inet_pton(AF_INET,argv[1],&serv_addr.sin_addr);
    if(val==0){
        printf("\nSrc does not contain a character string representing a valid network address in the specified address family.\n");
        return EXIT_FAILURE;
    }else if (val==-1) {
         fprintf(stderr, "\nError: %s\n",strerror(errno));
         return  EXIT_FAILURE;
    }
    
    int ret=connect(sock_fd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
    if(ret==-1){
        fprintf(stderr, "\nError: %s\n",strerror(errno));
        return  EXIT_FAILURE;
    }
    
    //We now attempt to read  from the file descriptor
    while((n=read(sock_fd,recvbf,sizeof(recvbf)-1))>0){
        recvbf[n]=0;
        if(fputs(recvbf,stdout)==EOF){
            printf("\nEnd of file!\n");
        }


    }
    char  sendbuf[BUFFSIZE];
    size_t size=strlen(sendbuf);
    fgets(sendbuf,1024-1,stdin);
    send(sock_fd,sendbuf,size,0);
    if(n<0){
        fprintf(stderr,"\nError: %s",strerror(errno));
        return  EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
