#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <bee-fish.h>

#define HTTP_PORT    443
#define HTTP_METHOD  "GET" // or "POST"
#define HOST_NAME    "laptop" // hostname of web server:
#define PATH_NAME    "/"

void setup() {

   Serial.begin(1500000);

   while (!Serial)
      delay(10);

   Serial.print("Connecting to WiFi...");
   
   WiFi.begin("Laptop", "feebeegeeb3");
   while (!WiFi.isConnected()) {
      Serial.print(".");
      delay(500);
   }

   Serial.println("Connected");
   
   
   WiFiClientSecure client;
   
   client.setInsecure();

   if(client.connect(HOST_NAME, HTTP_PORT)) {
      Serial.println("Connected to server " HOST_NAME);
   }
   else {
      Serial.println("Connection failed");
   }

   client.println(HTTP_METHOD " " PATH_NAME " HTTP/1.1");
   client.println("Host: " HOST_NAME);
   client.println("Connection: close");
   client.println(); // end HTTP request header
   
   BeeFishWeb::WebResponse response;
   BeeFishParser::Parser parser(response);
   char c;
   
   while (client.connected()) {
      
      while(client.available()) {
         // read an incoming char from the server and print them to serial monitor:
         c = client.read();
         Serial.print(c);
         if (!parser.match(c)) {
            Serial.print("{err}");
            break;
         }

      }

      if (parser.result() != BeeFishMisc::nullopt)
         break;

      delay(10);
   }

   if(!client.connected())
   {
      // if the server's disconnected, stop the client:
      Serial.println("Disconnected");
      client.stop();
   }

   if (parser.result() == true)
      Serial.println("Success parsing");
   else if (parser.result() == BeeFishMisc::nullopt)
      Serial.println("Unknown parse result");
   else
      Serial.println("Error parsing");

}

void loop() {
   // put your main code here, to run repeatedly:

}
