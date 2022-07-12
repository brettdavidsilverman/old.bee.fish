#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../config.h"
#include "../test/test.h"
#include "version.h"
#include "web-server.h"

int main(int argc, const char* argv[])
{
    using namespace std;
    using namespace BeeFishTest;

    std::cout << "Webserver http Server" << std::endl;
    std::cout 
    << "C++ run time: "
        << __cplusplus
        << std::endl
    << "Version: "
        << BEE_FISH_WEBSERVER_VERSION
        << std::endl;
        /*
    << "Database file: "
        << databaseFile.toUTF8()
        << std::endl
    << "Transaction file: "
        << transactionFile.toUTF8()
        << std::endl
    << "Host: "
        << HOST_NAME
        << std::endl;
    */

    if (hasArg(argc, argv, "-test") >= 0)
    {
        cout << "Testing HTTP..." << endl;
        
        return 0;
    }
    
    std::string port;
    int portArg;

    if ((portArg = hasArg(argc, argv, "-port")) >= 0)
    {
        port = argv[portArg + 1];
    }
    else
        port = "80";


    int _port = atoi(port.c_str());

    BeeFishWebServer::WebServer server(_port);
    
    server.paths()["/camera"] = 
        [](const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {
            cerr << "Camera" << endl;
            return client->defaultResponse();
        };

    server.start();

    return -1;
}

