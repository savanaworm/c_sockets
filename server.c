#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
//#include <signal.h>

#define LISTENQ 1024
#define BUFSIZE 1024
#define PORT 5000

//We are trying to pass the socket struct by reference to avoid copying the whole struct to the function
typedef struct sockaddr_in server_addr,client_addr;

//volatile sig_atomic_t keep_running=1;


//function prototype
void socket_handler(server_addr *s,client_addr *c);
/*
void signint_handler(int sig_num){
    printf("\nOooouch!Are you trying to kill me :(\nCtrl-C detected ....Terminating execution!\n");
    keep_running=0;
    exit(EXIT_FAILURE);
}
*/

int main(int argc, char *argv[])
{
    printf("Welcome to my test server\nThis is how to use the server\nYou can use Ctrl-C to terminate the server\n");
    server_addr s;
    client_addr c;
    //Cregister the signal signint_handler
    //signal(SIGINT,signint_handler);
    /*while(keep_running){
        socket_handler(&s, &c);
    }*/ 
    socket_handler(&s, &c);
    return EXIT_SUCCESS;
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
            fgets(buffer,1024-1,stdin);
            size_t size = strlen(buffer);
            send(conn_fd,buffer,size,0);
            sleep(1);
            char rbuff[BUFSIZE];
            if((recv(conn_fd,rbuff,size,0))==-1){
                perror("\nRecv\n");
                exit(EXIT_FAILURE);
            }
            printf("Message from client: %s",rbuff);
            exit(EXIT_SUCCESS);
        }
        printf("\nParent pid: %d",getpid());
        close(conn_fd);
    }

    return;
}

