#include <arpa/inet.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "header.h"
void cli_echo(int sock_fd){
     // //We now attempt to read  from the file descriptor
    for(;;){
        struct args args;
        struct result result;

        char sendbuf[BUFSIZE];
        printf("\n[+]Enter message to send to server:");
        if ((fgets(sendbuf,sizeof(sendbuf),stdin)==NULL)) {
            perror("\nFGETS error\n");
            exit(EXIT_FAILURE);
        }
        size_t size=strlen(sendbuf);
        
        if((send(sock_fd,sendbuf,size,0))==-1){
            perror("\nSend error\n");
            exit(EXIT_FAILURE);
        }
        char rbuff[BUFSIZE];
        size_t r =recv(sock_fd,rbuff,sizeof(rbuff)-1, 0);
        if(r==-1){
            perror("\nRecv\n");
            exit(EXIT_FAILURE);
        }else if(r==0){
            printf("\n[+]Server not reacheble\n");
            exit(EXIT_FAILURE);
        }else{
            rbuff[r]='\0';
            printf("\n[+]Echo from server: %s\n",rbuff);
        }
    } 
    return;
}
void cli_socket_handler(server_addr *s,char **argv){
    int sock_fd=0,n=0;
    char recvbf[BUFSIZE];
    
    //This here seems like a potential issue....but since this is just a test a try on the socket ,I will comw back and look at is.You sould definately not set a fixed buffer , what if it gets overflowed!!
    memset(&recvbf,'0',sizeof(recvbf));

    //create a socket with the socket call
    sock_fd=socket(AF_INET,SOCK_STREAM,0);
    printf("\n[+]Creating socket file descriptor\n");
    if(sock_fd==-1){
        fprintf(stderr,"\nError: %s occured!\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    memset(s,0, sizeof(*s));
    
    //htons helps converts the endiness of the port number given by the user to a standard format.
    (*s).sin_port=htons(5000);
    (*s).sin_family=AF_INET;
    //We now convert the ip string to its binary value  
    int val =inet_pton(AF_INET,argv[1],&(*s).sin_addr);
    if(val==0){
        printf("\nSrc does not contain a character string representing a valid network address in the specified address family.\n");
        exit(EXIT_FAILURE);
    }else if (val==-1) {
        fprintf(stderr, "\nError: %s\n",strerror(errno));
          exit(EXIT_FAILURE);
    }
    
    if((connect(sock_fd,(struct sockaddr *)s,sizeof(*s)))==-1){
        perror("\n[-]Connectin error!\n");
        exit(EXIT_FAILURE);
    }
       cli_echo(sock_fd);
    return;

}

int main(int argc, char *argv[]){   
    if(argc!=2){
        printf("\nUsage: %s <ip addr>\n",argv[0]);
        exit(EXIT_FAILURE);
    }    
    server_addr s;
    cli_socket_handler(&s,argv);

    return EXIT_SUCCESS;
}
