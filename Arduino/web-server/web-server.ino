#include <SPI.h>
#include <WiFi.h>
#include "web-server.h"

#define INTERNET_SSID "Laptop"      // your network SSID (name)
#define PASSWORD "feebeegeeb3"   // your network password

int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;


WebServer server(80);


void setup() {

  //Initialize serial and wait for port to open:

  Serial.begin(1500000);

  while (!Serial) {
    delay(10);

  }


  // attempt to connect to Wifi network:
  WiFi.begin(INTERNET_SSID, PASSWORD);

  while (!WiFi.isConnected()) {
    Serial.print("."); 
    delay(500);

  }

  server.begin();

  // you're connected now, so print out the status:

  printWifiStatus();
}

void loop() {

  // listen for incoming clients
  server.loop();
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