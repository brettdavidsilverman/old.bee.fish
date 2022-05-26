#ifndef WEB_SERVER_BASSE
#define WEB_SERVER_BASSE

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
#include "../b-script/b-script.h"

#include "../config.h"

#define MAX_CLIENTS 1


//Server control functions

namespace BeeFishWebServer {


    class WebClient;

    class WebServer {
        typedef std::function<bool(const BeeFishBString::BString& path, WebClient* client)> OnPath;
        typedef std::map<BeeFishBString::BString, OnPath> Paths;
        Paths _paths;

    public:
        const int _port;
        int _serverSocket = -1;
        const size_t _pageSize = getpagesize();

        WebServer(int port = 80) : _port(port) {
            if (!initializeServerSocket())
                perror("Error initializing server socket");
        }
        
        bool start() {
            socklen_t clilen;
            struct sockaddr_in cli_addr;
            int clientSocket = -1;


#ifdef SERVER
            int pid;
            printf(
                "Server started %shttp://127.0.0.1:%i%s\n",
                "\033[92m", _port, "\033[0m"
            );
#else
            printf(
                "Server started on port %i\n",
                _port
            );
#endif


            for (;;)
            {

                // Process will wait here till connection is accepted
                clilen = sizeof(cli_addr);
                clientSocket = accept(_serverSocket, (struct sockaddr *)&cli_addr, &clilen);

                if (clientSocket < 0)
                {
                    perror("ERROR on accept");
                    return false;
                }

                std::cerr << "Client connection accepted  on port " << _port << std::endl;

#ifdef SERVER
                // Create child process
                pid = fork();

                if (pid < 0)
                {
                    perror("ERROR on fork");
                    return false;
                }

                if (pid == 0)
                {
                    // This is the client process
                    close(_serverSocket);   
                    handleClient(clientSocket);
                    close(clientSocket);
                    return false;
                }
                else
                {
                    close(clientSocket);
                }
#else
                handleClient(clientSocket);
                close(clientSocket);
#endif

            }

        }

        bool initializeServerSocket() {
            
            char buffer[256];
            struct sockaddr_in serv_addr;
            int n, pid;

            if (_serverSocket > 0)
                close(_serverSocket);

            // First call to socket() function
            _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (_serverSocket < 0)
            {
                perror("ERROR opening socket");
                return false;
            }

            // Initialize socket structure
            bzero((char *)&serv_addr, sizeof(serv_addr));

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(_port);

            // Now bind the host address using bind() call.
            if (bind(_serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                perror("ERROR on binding");
                return false;
            }

            // Now start listening for the clients,
            int res = listen(_serverSocket, MAX_CLIENTS);

            if (res != 0) {
                perror("Invalid listen result");
                return false;
            }

            return true;

        }

        Paths& paths() {
            return _paths;
        }


        // Defined in web-server.h
        virtual bool handleClient(int clientSocket);
        
    };

}


#endif
