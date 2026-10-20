#include <arpa/inet.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include "header.h"



int main(int argc, char *argv[])
{
    printf("Welcome to my test server\nThis is how to use the server\nYou can use Ctrl-C to terminate the server\n");
    server_addr s;
    client_addr c;
    socket_handler(&s, &c);
    return EXIT_SUCCESS;
}

Sigfunc *Signal(int signo,Sigfunc *func){
 
    struct sigaction act,oact;
    act.sa_handler=func;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    if(signo==SIGALRM){
#ifdef SA_RESTART
        act.sa_flags &=~SA_RESTART;
#endif /* ifdef MACRO */
    }else{
#ifdef SA_RESTART
        act.sa_flags |=SA_RESTART;
#endif
    }
    if((sigaction(signo,&act,&oact))<0){
        return (SIG_ERR);
    }
    return(oact.sa_handler);
}

void sig_child(int signo){
    pid_t pid;
    int stat;

    pid= wait(&stat);
    printf("\n[+]Child has been terminated PID: %d\n",pid);
    return;
}


void socket_handler(server_addr *s,client_addr *c){
    
    char buffer[BUFSIZE];
    int listen_fd,conn_fd;
    
    //Get the file descriptor that willbe used for listening
    if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("\nListen!\n");
        exit(EXIT_FAILURE);
    }

    //Fill the struct with 0 and the buffer too
    memset(s,0 , sizeof(*s));
    memset(buffer, 0, sizeof(buffer));

    //Setup struct  values by reference
    s->sin_family=AF_INET;
    s->sin_port=htons(PORT);
    s->sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    //Bind socket file descriptor to address 
    if(bind(listen_fd,(struct sockaddr *)s,sizeof(*s))<0){
        perror("\nError binding socket!\n");
        exit(EXIT_FAILURE);
    }
    
    printf("\nSuccessfully bound!\n");
    printf("\n[+]Listening....\n");
    if(listen(listen_fd,LISTENQ)<0){
        perror("\nListen!\n");
        exit(EXIT_FAILURE);
    }
    Signal(SIGCHLD,sig_child);
    socklen_t client_len;
    while(1){
        pid_t pid;
        client_len=sizeof(*c);
        if((conn_fd=accept(listen_fd, (struct sockaddr *)c, &client_len))<0){
                perror("\nAccept!\n");
                exit(EXIT_FAILURE);
                }
        if((pid=fork())==0){
            close(listen_fd);
            printf("\nThis is the child running with the PID: %d\n",getpid());
            
            char rbuff[BUFSIZE];
            size_t r=recv(conn_fd, rbuff, sizeof(rbuff)-1,0);
            if(r==-1){
                perror("\nRecv\n");
                exit(EXIT_FAILURE);
            }
            if(r==0){
                printf("\n[+]Clinet closed connection\n");
                exit(EXIT_FAILURE);
            }else{
                rbuff[r]='\0';
                printf("\nMessage from client: %s\n",rbuff);
                exit(EXIT_SUCCESS);
            }

        }
        printf("\nParent pid: %d\n",getpid());
        close(conn_fd);
    }

    return;
}

