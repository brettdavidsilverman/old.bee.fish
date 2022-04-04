#include "web-server.h"

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
#include "../web-request/web-request.h"

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
    if ( listen (listenfd, MAX_CLIENTS) != 0 )
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
    for (int i = 0; i < MAX_CLIENTS; ++i)
        clients[i] = -1;
    //memset(clients, -1, MAX_CLIENTS);
    
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
            cerr << "Accepted" << endl;

            std::thread responder(respond, slot);
            responder.detach();
/*
            if ( fork()==0 )
            {
                respond(slot);
                exit(0);
            }
*/            
        }

        // Wait for client slot to be free
        while (clients[slot] != -1) {
            slot = (slot + 1) % MAX_CLIENTS;
            if (slot == 0)
            {
                // Yield after every loop up to MAX_CLIENTS
                std::this_thread::yield();
            }
        }

    }
}

#define SEND(clientfd, data, size) { \
    send(clientfd, data, size, 0); \
    cerr << std::string((const char*)data, size); \
}


//client connection
void respond(int slot)
{

    using namespace std;

    int clientfd = clients[slot];

    std::stringstream stream;

    BeeFishWeb::WebRequest request;

    BeeFishJSON::JSONParser parser(request);

    char* inputBuffer = (char*)malloc(getpagesize());

    while (parser.result() == BeeFishMisc::nullopt) {

        int received, fd, bytes_read;
        char *ptr;

        received = recv(clientfd, inputBuffer, getpagesize(), 0);

        if (received < 0)  {
            // receive error
            cerr << "web-server::respond::recv() error." << endl;
           break;
        }  
        else if (received == 0) {
            // receive socket closed
            cerr << "web-server::respond::Client disconnected upexpectedly." << endl;;
            break;
        }
        else {
        
            const BeeFishBString::Data data(inputBuffer, received);

            // message received
            parser.read(data);

      //      std::string str((char*)data.data(), data.size());
      //      cerr << str;

        }

    }    

    free(inputBuffer);

    BeeFishBString::BStream output;

    output._onbuffer = [clientfd](const BeeFishBString::Data& data) {
        SEND(clientfd, data.data(), data.size());
    };

    output << 
        "HTTP/1.1 200 OK\r\n" \
        "Server: " BEE_FISH_WEBSERVER_VERSION "\r\n" \
        "Content-Type: text/plain\r\n" \
        "Connection: close\r\n" \
        "Transfer-Encoding: chunked\r\n" \
        "\r\n";

    output.flush();

    output._onbuffer = [clientfd](const BeeFishBString::Data& data) {
        std::stringstream stream;
        stream << std::hex << data.size();
        std::string size = stream.str();
        SEND(clientfd, size.c_str(), size.size());
        SEND(clientfd, "\r\n", 2);
        SEND(clientfd, data.data(), data.size());
        SEND(clientfd, "\r\n", 2);
    };

    output << "Responder thread in slot " << slot << endl;

    if (parser.result() == true) {
        cerr << "OK" << endl;
        output << "Parsed valid request";
    }
    else {
        cerr << "OK" << endl;
        output << "Invalid request";
    }

 //   output << "\r\n";

    output.flush();

    SEND(clientfd, "0\r\n", 3);
    SEND(clientfd, "\r\n", 2);

    // tidy up
    //fflush(stdout);
    //shutdown(STDOUT_FILENO, SHUT_WR);
    // shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    //close(STDOUT_FILENO);
    close(clientfd);
    clients[slot] = -1;
}