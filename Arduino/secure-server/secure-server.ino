//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 32768

#include <WiFi.h>
#include <HTTPSServer.hpp>
#include "ssl-cert.h"
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

using namespace httpsserver;

const char* ssid = "ESP32";
const char* password = "feebeegeeb3";

const char* PARAM_MESSAGE = "message";

IPAddress ipAddress(10, 10, 1, 1);
IPAddress gateway(10, 10, 1, 1);
IPAddress subnet(255, 255, 255, 0);

HTTPSServer* secureServer = nullptr;


void clientConnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{

    Serial.print("IP Address: ");
    IPAddress ipAddress = WiFi.softAPIP();
    Serial.println(ipAddress);
  
}

void setup() {

    Serial.begin(1500000);
    while (!Serial)
      delay(10);

    Serial.println("Starting");

    initializeSSLCert();
    
    WiFi.onEvent(clientConnected, SYSTEM_EVENT_AP_STACONNECTED);

    WiFi.mode(WIFI_AP);

    WiFi.softAPConfig(ipAddress, gateway, subnet);

    WiFi.softAP(ssid, password);
    //WiFi.begin(ssid, password);
/*
    while (!WiFi.isConnected()) {
        Serial.print(".");
        delay(500);
    }
      

    Serial.println(WiFi.localIP());
*/
    Serial.print("Setting up web server...");
    
    secureServer = new HTTPSServer(sslCert);
    
    ResourceNode * nodeRoot = new ResourceNode("/", "GET", [](HTTPRequest * req, HTTPResponse * res){
        res->println("Secure Hello World!!!");
    });
 
    secureServer->registerNode(nodeRoot);
 
    secureServer->start();
   
    if (secureServer->isRunning()) {
        Serial.println("Ok");
    }
 /*   
    server.onSslFileRequest([](void * arg, const char *filename, uint8_t **buf) -> int {
        Serial.printf("SSL File: %s\n", filename);
        File file = SPIFFS.open(filename, "r");
        if(file){
            size_t size = file.size();
            uint8_t * nbuf = (uint8_t*)malloc(size);
            if(nbuf){
                size = file.read(nbuf, size);
                file.close();
                *buf = nbuf;
                return size;
            }
            file.close();
        }
        *buf = 0;
        return 0;
    }, NULL
    );
  
    server.beginSecure("/server.cer", "/server.key", NULL);

*/
}

void loop() {
    secureServer->loop();
}
