#define BUFSIZE 1024
#define PORT 5000
#define LISTENQ 1024

typedef void Sigfunc(int);

typedef struct sockaddr_in server_addr;
typedef struct sockaddr_in client_addr;

//Function prototypes
void socket_handler(server_addr *s,client_addr *c);
void sig_child(int signo);
Sigfunc *Signal(int signo,Sigfunc *func);

