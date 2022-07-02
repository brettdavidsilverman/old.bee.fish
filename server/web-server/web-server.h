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

      TaskHandle_t xHandle = NULL;

      xTaskCreatePinnedToCore(
         WebClient::process,      // Task function. 
         "WebClient",      // String with name of task. 
         5000,      // Stack size in bytes. 
         client,       // Parameter passed as input of the task 
         WebServer::PRIORITY,     // Priority of the task. 
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