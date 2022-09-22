#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <map>
#include <bee-fish.h>

namespace FeebeeCam {

   class WiFiWebServer : public WiFiServer {
   protected:
      typedef std::function<bool(BeeFishWeb::WebRequest& webRequest, WiFiClient& client)> OnPath;
      std::map<BString, OnPath> _requests;
      OnPath _defaultOnPath;

   public:

      WiFiWebServer(int port) : WiFiServer(port){
         BaseType_t xReturned;
         TaskHandle_t xHandle = NULL;

         xReturned = xTaskCreatePinnedToCore(
               WiFiWebServer::webServerTask,         /* Task function. */
               "WebServerTask",      /* String with name of task. */
               10000,               /* Stack size in bytes. */
               this,                /* Parameter passed as input of the task */
               1,                     /* Priority of the task. */
               &xHandle            /* Task handle. */,
               1//core                      /* Pinned to core */
         );         

      }

      static void webServerTask( void * input ) {
         WiFiWebServer* webServer = (WiFiWebServer*)input;

         webServer->begin();

         for (;;) {
            // listen for incoming clients
            webServer->loop();
            delay(10);
         }

      }

      static void setDefaultHeaders(WiFiClient& client) {
         client.println("HTTP/1.1 200 OK");
         client.println("Content-Type: application/json; charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");
         client.println("Cache-Control: no-store, max-age=0");
         client.println("Connection: close");
      }

      static bool parseRequest(Parser& parser, WiFiClient& client) {

         while (client && client.available()) {

            char character = client.read();

            Serial.print(character);

            if (!parser.match(character))
            {
               Serial.println("Failed to match parsing request");
               break;
            }

            if (parser.result() != BeeFishMisc::nullopt)
               break;
         }

         return (parser.result() == true);

      }

      static void sendResponse(WiFiClient& client, const BeeFishBScript::Object& output) {

         setDefaultHeaders(client);
         client.println("Transfer-Encoding: chunked");
         client.println();

         BeeFishBString::BStream stream;

         stream.setOnBuffer(
            [&client](const Data& buffer) {
                  sendChunk(client, buffer);
            }
         );

         stream << output.str();

         stream.flush();

         sendChunk(client);

      }

      static void sendChunk(WiFiClient& client, const Data& data) {
         
         client.println(data.size(), HEX);
         
         if (data.size())
            client.write(data._data, data.size());
         
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

         while (client && client.available()) {

            char c = client.read();

            if (!parser.match(c))
            {
               Serial.println("Failed to match");
               break;
            }

            if ( webRequest.headers().result() == true )
            {

               const BString& path = webRequest.path();
               WiFiClient clientCopy(client);

               if (_requests.count(path) > 0)
               {
                  OnPath onPath = _requests[path];
                  if (onPath(webRequest, clientCopy))
                     break;
               }
               else {
                  if (_defaultOnPath && _defaultOnPath(webRequest, clientCopy))
                     break;
               }

               // send a standard http response header
               sendFileNotFound(clientCopy);

               break;

            }

         }

         client.stop();



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