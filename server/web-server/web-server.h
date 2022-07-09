#ifndef WEB_SERVER
#define WEB_SERVER

#include "web-server-base.h"
#include "web-client.h"

namespace BeeFishWebServer {

   inline bool WebServer::handleClient(int clientSocket) {

      WebClient* client = new WebClient(this, clientSocket);

      WebClient::process(client);

      return true;
      
#ifdef SERVER
      std::thread thread(WebClient::process, client);
      thread.detach();
#else

      TaskHandle_t xHandle = NULL;

      std::stringstream stream;
      stream << "WebClient:" << clientSocket;

      std::string taskName = stream.str();

      xTaskCreatePinnedToCore(
         WebClient::process,      // Task function. 
         taskName.c_str(),      // String with name of task. 
         5120,      // Stack size in bytes. 
         client,       // Parameter passed as input of the task 
         WebClient::PRIORITY,     // Priority of the task. 
         &xHandle,        // Task handle
         1        // Pinned to core 
      );

      if (xHandle == NULL)
         cerr << "Error creating client process" << endl;
         
      return xHandle != NULL;
      
#endif
      return true;

   }

}

#endif