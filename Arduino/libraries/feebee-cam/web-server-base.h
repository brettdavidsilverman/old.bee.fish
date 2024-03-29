#ifndef WEB_SERVER_BASE2__H
#define WEB_SERVER_BASE2__H

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
#include <bee-fish.h>
#include <WiFi.h>
#include <mutex>
#include "config.h"



class WiFiServer;

namespace FeebeeCam {


    class WebClient;

    class WebServer {
    public:
        typedef std::function<bool(const BeeFishBString::BString& path, WebClient* client)> OnPath;
        typedef std::map<BeeFishBString::BString, OnPath> Paths;
        Paths _paths;
        WebServer::OnPath _defaultHandler = nullptr;
        int _port;
        WiFiServer* _server;
        BString _taskName;
        
        int _priority;
        int _core;
        TaskHandle_t _handle = NULL;
        bool _quit = false;
        bool _isRunning = false;

    public:
        const size_t _pageSize = getPageSize();

        // Declared in web-server2.cpp

        WebServer(int port = 80, int priority = 1, int core = -1);

        virtual ~WebServer();

        virtual bool start();
        
        static void loop(void* param);

        Paths& paths() {
            return _paths;
        }

        WiFiServer* server() {
            return _server;
        }


    };

}


#endif