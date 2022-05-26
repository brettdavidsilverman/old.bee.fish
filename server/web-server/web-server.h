#ifndef WEB_SERVER
#define WEB_SERVER

#include "web-server-base.h"
#include "web-client.h"

namespace BeeFishWebServer {

    inline bool WebServer::handleClient(int clientSocket) {

        WebClient client(clientSocket);
        
        if (!client.readRequest())
            return false;

        if (client._webRequest._firstLine->result() == true) {

            const BeeFishBString::BString& path = client._webRequest.path();

            if (_paths.count(path) > 0) {
                OnPath func = _paths.at(path);
                return func(path, &client);
            }
            else {
                client._statusCode = 404;
                client._statusText = "Not Found";
            }

        }

        return client.handleResponse();

    }

}

#endif