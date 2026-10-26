#define BUFSIZE 1024
#define PORT 5000
#define LISTENQ 1024

typedef void Sigfunc(int);

typedef struct sockaddr_in server_addr;
typedef struct sockaddr_in client_addr;

struct args{
    long arg1;
    long arg2;
};

struct result{
    long result;
};
//FUnction prototypes
void socket_handler(server_addr *s,client_addr *c);
void sig_child(int signo);
Sigfunc *Signal(int signo,Sigfunc *func);
void str_echo(int conn_fd);
void cli_echo(int sock_fd);
void cli_socket_handler(server_addr *s,char **argv);
