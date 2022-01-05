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
#include <string>
#include <vector>
#include "version.h"
#include "../b-string/string.h"
#include "../json/json-parser.h"
#include "../https/request.h"

static int listenfd, clients[MAX_CLIENTS];
static void error(char *);
static void serveForever();
static void respond(int);

//start server
void startWebserver(const string& port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port.c_str(), &hints, &res) != 0)
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

    printf(
            "Server started %shttp://127.0.0.1:%s%s\n",
            "\033[92m",port.c_str(),"\033[0m"
            );

    serveForever();

}

void serveForever()
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;    
    
    int slot=0;
    
    // Setting all elements to -1: signifies there is no client connected
    int i;
    memset(clients, -1, MAX_CLIENTS);
    
    for ( i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

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
        int count = 0;
        while (clients[slot] != -1) {
            slot = (slot + 1) % MAX_CLIENTS;
            if (slot == 0)
            {
                std::this_thread::yield();
            }
        }
    }
}



//client connection
void respond(int slot)
{
    using namespace std;

    int clientfd = clients[slot];

    std::stringstream stream;

    char* buff = (char*)malloc(getpagesize());

    BeeFishHTTPS::Request request;

    JSONParser parser(request);

    while (parser.result() == BeeFishMisc::nullopt) {

        int received, fd, bytes_read;
        char *ptr;

        received = recv(clientfd, buff, getpagesize(), 0);

        if (received < 0)  {
            // receive error
            cerr << "httpd::respond::recv() error." << endl;
           break;
        }  
        else if (received == 0) {
            // receive socket closed
            cerr << "httpd::respond::Client disconnected upexpectedly." << endl;;
            break;
        }
        else {
        

            const std::string buffer(buff, received);

            // message received
            if (parser.read(buffer) != BeeFishMisc::nullopt) {
                break;
            }

        }

    }    

    stream << 
        "HTTP/1.1 200 OK\r\n" \
        "Server: " BEE_FISH_WEBSERVER_VERSION "\r\n" \
        "Content-Type: text\r\n" \
        "\r\n";

    if (parser.result() == true) {
        cerr << "OK" << endl;
        stream << "Hello World";
    }
    else {
        cerr << "OK" << endl;
        stream << "Goodbye world";
    }

    stream << "\r\n";

    send(clientfd, stream.str().c_str(), stream.str().length(), 0);

    // tidy up
    //fflush(stdout);
    //shutdown(STDOUT_FILENO, SHUT_WR);
    shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    //close(STDOUT_FILENO);
    close(clientfd);
    free(buff);
    clients[slot] = -1;
}