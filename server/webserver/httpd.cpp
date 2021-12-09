#include "httpd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <thread>

static int listenfd, clients[MAX_CLIENTS];
static void error(char *);
static void startServer(const char *);
static void respond(int);

void serve_forever(const char *PORT)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;    
    
    int slot=0;
    
    printf(
            "Server started %shttp://127.0.0.1:%s%s\n",
            "\033[92m",PORT,"\033[0m"
            );

    // Setting all elements to -1: signifies there is no client connected
    int i;
    for ( i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    startServer(PORT);
    
    // Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);

    // ACCEPT connections
    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients[slot]<0)
        {
            perror("accept() error");
        }
        else
        {
            if ( fork()==0 )
            {
                respond(slot);
                exit(0);
            }
        }

        // Spin wait for client slot to be free
        bool full = false;
        int count = 0;

        while (clients[slot]!=-1) {
            slot = (slot+1)%MAX_CLIENTS;
            if (++count == MAX_CLIENTS)
            {
                std::this_thread::yield();
                count = 0;
            }
        }
    }
}

//start server
void startServer(const char *port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        int option = 1;
        listenfd = socket (p->ai_family, p->ai_socktype, 0);

        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        if (listenfd == -1)
            continue;

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }
    if (p==NULL)
    {
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}

//client connection
void respond(int n)
{
    using namespace std;

    int clientfd = -1;

    int rcvd, fd, bytes_read;
    char *ptr;

    char* buffer = (char*)malloc(getpagesize());
    rcvd=recv(clients[n], buffer, getpagesize(), 0);

    if (rcvd<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    {
        buffer[rcvd] = '\0';

        // bind clientfd to stdout, making it easier to write
        clientfd = clients[n];
        dup2(clientfd, STDOUT_FILENO);
        close(clientfd);

        clog << buffer << endl;

        cout << 
            "HTTP/1.1 200 OK\r\n" \
            "Server: bee.fish/0.0.0 (Linux)\r\n" \
            "Content-Type: text\r\n" \
            "\r\n" \
            "Hello World";

        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
    }

    //Closing SOCKET
    shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clientfd);
    free(buffer);
    clients[n]=-1;
}