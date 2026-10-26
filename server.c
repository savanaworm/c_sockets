#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <strings.h>
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
    act.sa_flags=SA_NOCLDSTOP;
    if((sigaction(signo,&act,&oact))<0){
        return (SIG_ERR);
    }
    return(oact.sa_handler);
}

void sig_child(int signo){
    pid_t pid;
    int status;

    while ((pid=waitpid(-1,&status,WNOHANG))!=-1) {
            if (WIFEXITED(status)) {
                printf("\nChild exited successfully with status %d\n",WEXITSTATUS(status));
                break;
            }else{
                perror("\nChild exit error\n");
            }
    }
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
    signal(SIGCHLD,sig_child);
    socklen_t client_len;
    for(;;){
        pid_t pid;
        client_len=sizeof(*c);
        if((conn_fd=accept(listen_fd, (struct sockaddr *)c, &client_len))<0){
            if (errno==EINTR) {
                continue;
            }else {
                perror("\n[+]Accept error\n");
                exit(EXIT_FAILURE);
            }
        }
        if((pid=fork())<0){
            perror("\nFork error\n");
            close(conn_fd);
            continue;
        }

        if(pid==0){
            close(listen_fd);
            printf("\nThis is the child running with the PID: %d\n",getpid());
            for(;;){
                char rbuff [BUFSIZE];
                memset(rbuff,0,sizeof(rbuff));
                size_t r=recv(conn_fd, rbuff, sizeof(rbuff)-1,0);
                if(r==-1){
                    perror("\n[+]Recv error\n");
                    _exit(EXIT_FAILURE);
                }
                if(r==0){
                    printf("\n[+]Client closed connection\n");
                    _exit(EXIT_SUCCESS);
                }
                rbuff[r]='\0';
                printf("\n[+]Message from client: %s\n",rbuff);                    //we echo back to client 
                if((send(conn_fd,rbuff,r,0))==-1){
                    perror("\n[+]Send error!\n");
                    _exit(EXIT_FAILURE);
                }
                memset(rbuff,0,sizeof(rbuff));
            }
        }

    }
    printf("\nPID: %d\n",getpid());
    printf("\n[+]Closing connection file descriptor!\n");
    close(conn_fd);
        
}

