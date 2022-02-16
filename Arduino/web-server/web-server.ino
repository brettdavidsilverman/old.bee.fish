#include <SPI.h>
#include <WiFi.h>
#include <web-server.h>

#define INTERNET_SSID "Laptop"         // your network SSID (name)
#define PASSWORD "feebeegeeb3"    // your network password

int keyIndex = 0;                         // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;


WebServer server(80);

void webServerTask( void * pvParameters ) {
   server.begin();
   for (;;) {
      // listen for incoming clients
      server.loop();
      delay(10);
   }
}


void setup() {

   Serial.begin(1500000);

   while (!Serial) {
      delay(10);
   }

   server.requests()["/"] = [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
      client.println("HTTP/1.1 200 OK");

      client.println("Content-Type: text/plain;charset=utf-8");

      client.println();

      client.println("Hello World");

      return true;
   };

   // attempt to connect to Wifi network:
   WiFi.begin(INTERNET_SSID, PASSWORD);

   while (!WiFi.isConnected()) {
      Serial.print("."); 
      delay(500);
   }

   BaseType_t xReturned;
   TaskHandle_t xHandle = NULL;

   xReturned = xTaskCreatePinnedToCore(
         webServerTask,         /* Task function. */
         "WebServerTask",      /* String with name of task. */
         10000,               /* Stack size in bytes. */
         NULL,                /* Parameter passed as input of the task */
         1,                     /* Priority of the task. */
         &xHandle            /* Task handle. */,
         1                      /* Pinned to core */
   );         

   // you're connected now, so print out the status:

   printWifiStatus();
}

void loop() {
   delay(10);
}

void printWifiStatus() {

   // print the SSID of the network you're attached to:

   Serial.print("SSID: ");

   Serial.println(WiFi.SSID());

   // print your WiFi shield's IP address:

   IPAddress ip = WiFi.localIP();

   Serial.print("IP Address: ");

   Serial.println(ip);

   // print the received signal strength:

   long rssi = WiFi.RSSI();

   Serial.print("signal strength (RSSI):");

   Serial.print(rssi);

   Serial.println(" dBm");
}