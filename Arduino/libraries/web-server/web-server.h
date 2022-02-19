#include <Arduino.h>
#include <WiFi.h>
#include <map>
#include <bee-fish.h>

namespace FeebeeCam {

   class WebServer : public WiFiServer {
   protected:
      typedef std::function<bool(BeeFishWeb::WebRequest& webRequest, WiFiClient& client)> OnPath;
      std::map<BString, OnPath> _requests;
      OnPath _defaultOnPath;

   public:

      WebServer(int port, int core = 1) : WiFiServer(port){
         BaseType_t xReturned;
         TaskHandle_t xHandle = NULL;

         xReturned = xTaskCreatePinnedToCore(
               WebServer::webServerTask,         /* Task function. */
               "WebServerTask",      /* String with name of task. */
               10000,               /* Stack size in bytes. */
               this,                /* Parameter passed as input of the task */
               1,                     /* Priority of the task. */
               &xHandle            /* Task handle. */,
               core                      /* Pinned to core */
         );         

      }

      static void webServerTask( void * input ) {
         WebServer* webServer = (WebServer*)input;

         webServer->begin();

         for (;;) {
            // listen for incoming clients
            webServer->loop();
            delay(10);
         }

      }

      static void setDefaultHeaders(WiFiClient& client) {
         client.println("HTTP/1.1 200 OK");
         client.println("Connection: keep-alive");
         client.println("Content-Type: text/javascript; charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");
         client.println("Cache-Control: no-store, max-age=0");
      }

      static void sendResponse(WiFiClient& client, const BeeFishJSONOutput::Object& output) {

         setDefaultHeaders(client);
         client.println("Transfer-Encoding: chunked");
         client.println();

         BeeFishBString::BStream stream;

         stream.setOnBuffer(
            [&client](const Data& buffer) {
                  sendChunk(client, buffer);
            }
         );

         stream << output;

         stream.flush();

         sendChunk(client);

      }

      static void sendChunk(WiFiClient& client, const Data& data) {
         
         client.println(data.size(), HEX);
         
         if (data.size())
            client.write(data.data(), data.size());
         
         client.println();

      }

      static void sendChunk(WiFiClient& client) {
         Data data;
         sendChunk(client, data);
      }

      void setDefaultRequest(OnPath onPath) {
         _defaultOnPath = onPath;
      }

      std::map<BString, OnPath>& requests() {
         return _requests;
      }

      void loop() {

         WiFiClient client = available();

         if (!client)
            return;

         // Setup the parser
         BeeFishWeb::WebRequest webRequest;
         BeeFishJSON::JSONParser parser(webRequest);

         while (client.available()) {

            char c = client.read();

            if (!parser.match(c))
            {
               Serial.println("Failed to match");
               break;
            }

            if ( webRequest.headers().result() == true )
            {

               const BString& path = webRequest.path();

               if (_requests.count(path) > 0)
               {
                  OnPath onPath = _requests[path];
                  if (onPath(webRequest, client))
                     break;
                  else {
                     if (_defaultOnPath && _defaultOnPath(webRequest, client))
                        break;
                  }
               }

               // send a standard http response header
               sendFileNotFound(client);

               break;

            }

         }

         //client.stop();

      };

      void sendFileNotFound(WiFiClient& client) {
         client.println("HTTP/1.1 404 Not Found");

         client.println("Content-Type: text/plain;charset=utf-8");

         client.println("Access-Control-Allow-Origin: null");

         client.println("Connection: close");   // the connection will be closed after completion of the response

         client.println();

         client.println("Not Found");
      }


   };
}