#include <Arduino.h>
#include <WiFi.h>
#include <map>
#include <bee-fish.h>

class WebServer : public WiFiServer {
protected:
   typedef std::function<bool(BeeFishWeb::WebRequest& webRequest, WiFiClient& client)> OnPath;
   std::map<BString, OnPath> _requests;
   OnPath _defaultOnPath;

public:

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

      BeeFishWeb::WebRequest webRequest;
      BeeFishJSON::JSONParser parser(webRequest);

      while (client.available()) {

         char c = client.read();

         Serial.print(c);

         if (!parser.match(c))
         {
            Serial.println("Failed to match");
            break;
         }

         if ( ( webRequest.firstLine().result() == true &&
                webRequest.method() == "GET" )
                || webRequest.result() == true) 
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

            client.println("HTTP/1.1 404 Not Found");

            client.println("Content-Type: text/plain;charset=utf-8");

            client.println("Access-Control-Allow-Origin: null");

            client.println("Connection: close");   // the connection will be closed after completion of the response

            client.println();

            client.println("Not Found");

            break;

         }

      }

      client.stop();

   };

   WebServer(int port) : WiFiServer(port) {

   }


};
