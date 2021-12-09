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

static int listenfd, clients[MAX_CLIENTS];
static void error(char *);
static void startServer(const std::string& port);
static void respond(int);

void serve_forever(const std::string& port)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;    
    
    int slot=0;
    
    printf(
            "Server started %shttp://127.0.0.1:%s%s\n",
            "\033[92m",port.c_str(),"\033[0m"
            );

    // Setting all elements to -1: signifies there is no client connected
    int i;
    for ( i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    startServer(port);
    
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
            if (++count == MAX_CLIENTS)
            {
                std::this_thread::yield();
                count = 0;
            }
        }
    }
}


//start server
void startServer(const string& port)
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
}

//client connection
void respond(int n)
{
    using namespace std;

    int clientfd = clients[n];

    int rcvd, fd, bytes_read;
    char *ptr;

/*    char* buffer = (char*)malloc(getpagesize());
    rcvd=recv(clients[n], buffer, getpagesize(), 0);

    if (rcvd<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    */
    std::vector<std::string> buffer;

    {
      //  buffer[rcvd] = '\0';
        cerr << clientfd << ":Redirecting client file descriptor" << endl;

        // bind clientfd to stdout, making it easier to write
        int result = dup2(clientfd, STDIN_FILENO);
        cerr << "Dup2.1: " << result << endl;
        close(clientfd);
        clientfd = result;
        result = dup2(clientfd, STDOUT_FILENO);
        cerr << "Dup2.2: " << result << endl;
        cerr << "Client FD: " << clientfd << endl;
        close(clientfd);
        //close(STDIN_FILENO);


        BeeFishBString::BString line;
        while (!cin.eof()) {
            getline(cin, line);
            if (line.trim().length() == 0)
                break;
            buffer.push_back(line);
            cerr << "Read: " << line << endl;
        }


        //clog << buffer << endl;
        //close(clientfd);
    
        cout << 
            "HTTP/1.1 200 OK\r\n" \
            "Server: " BEE_FISH_WEBSERVER_VERSION "\r\n" \
            "Content-Type: text\r\n" \
            "\r\n" \
            "Hello World\r\n";

        for (string& line : buffer) {
            cout << line;
            cerr << "Wrote: " << line;
        }

        cout << "\r\n";
        cerr << "Tidy Up " << endl;
        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
 
        // tidy up
        /*
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        */
    }

    // Closing SOCKET
    shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clientfd);
    //free(buffer);
    clients[n] = -1;
}