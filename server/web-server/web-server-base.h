#ifndef WEB_SERVER_BASE
#define WEB_SERVER_BASE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "../bee-fish.h"
/*
#include "../b-string/string.h"
#include "../json/json-parser.h"
#include "../web-request/web-request.h"
#include "../b-script/b-script.h"
*/
#define MAX_CLIENTS 10

#ifdef SERVER
    #define delay(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
#endif                                        


//Server control functions

namespace BeeFishWebServer {


    class WebClient;

    class WebServer {
    public:
        typedef std::function<bool(const BeeFishBString::BString& path, WebClient* client)> OnPath;
        typedef std::map<BeeFishBString::BString, OnPath> Paths;
        Paths _paths;
        inline static const int PRIORITY = 1;
        WebServer::OnPath _defaultHandler = nullptr;

    public:
        const int _port;
        int _serverSocket = -1;
        const size_t _pageSize = getPageSize();
        std::string _taskName;

        WebServer(int port = 80) : _port(port) {
            std::stringstream stream;
            stream << "WebServer:" << _port;
            _taskName = stream.str();
        }
        
        virtual bool start(int core = 0) {

#ifdef SERVER
            WebServer::loop(this); 
            return true;
#else
            TaskHandle_t xHandle = NULL;
            
            xTaskCreatePinnedToCore(
                WebServer::loop,      // Task function. 
                _taskName.c_str(),      // String with name of task. 
                5120, //2048,                // Stack size in bytes. 
                this,                 // Parameter passed as input of the task 
                WebServer::PRIORITY,     // Priority of the task. 
                &xHandle,             // Task handle
                core                  // Pinned to core 
            );

            if (xHandle == NULL)
                cerr << "Error starting " << _taskName << endl;

            return xHandle != NULL;
#endif

        }

        static void loop(void* param) {

            WebServer* webServer = (WebServer*)param;

            for (;;) {

                socklen_t clilen;
                struct sockaddr_in cli_addr;
                int clientSocket = -1;

                if (!webServer->initializeServerSocket()) {
                    cerr << "Error initializing server socket" << endl;
                    delay(2000);
                    continue;
                }

                cerr << "Server started on port " << webServer->_port << endl;

                for (;;)
                {

                    // Process will wait here till connection is accepted
                    clilen = sizeof(cli_addr);
                    clientSocket = accept(webServer->_serverSocket, (struct sockaddr *)&cli_addr, &clilen);
                    
                    if (clientSocket < 0)
                    {
                        cerr << "WebServer::start error on accept" << endl;
                        delay(2000);
                        break;
                    }

                    std::cerr << "Web server request socket accepted: " << clientSocket << std::endl;

                    webServer->handleClient(clientSocket);

                    delay(10);
                }
            }

        }

        bool initializeServerSocket() {
            
            char buffer[256];
            struct sockaddr_in serv_addr;

            if (_serverSocket > 0) {
                cerr << "Closing server socket " << _serverSocket << endl;
#ifdef SERVER
                close(_serverSocket);
#else                                
                closesocket(_serverSocket);
#endif                
            }

            // First call to socket() function
            _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (_serverSocket < 0)
            {
                cerr << "Error creating server socket" << endl;
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
                cerr << "Error binding server socket" << endl;
                return false;
            }

            // Now start listening for the clients,
            int res = listen(_serverSocket, MAX_CLIENTS);

            if (res != 0) {
                cerr << "Invalid listen result" << endl;
                return false;
            }

            cerr << "Server listening on socket " << _serverSocket << endl;

            return true;

        }

        Paths& paths() {
            return _paths;
        }


        static bool& processingClient() {
            static bool value = false;
            return value;
        }

        // Defined in web-server.h
        virtual bool handleClient(int clientSocket);
        
    };

}


#endif
