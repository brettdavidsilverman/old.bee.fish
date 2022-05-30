#ifndef WEB_SERVER
#define WEB_SERVER

#include "web-server-base.h"
#include "web-client.h"

namespace BeeFishWebServer {

    inline bool WebServer::handleClient(int clientSocket) {

        WebClient* client = new WebClient(this, clientSocket);

#ifdef SERVER
        std::thread thread(WebClient::process, client);
        thread.detach();
#else
        WebClient::process(client);
#endif
        return true;

    }

}

#endif