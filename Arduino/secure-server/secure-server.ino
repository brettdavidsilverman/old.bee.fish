//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 32768

#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include <HTTPSServer.hpp>
#include "ssl-cert.h"
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <bee-fish.h>

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

    if(!SPIFFS.begin(true)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }

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
    
    ResourceNode * nodeStatus = new ResourceNode("/status", "GET", [](HTTPRequest * req, HTTPResponse * res){
        res->println("{\"status\": \"Ok\"}");
    });

    ResourceNode * nodeDefault = new ResourceNode("", "GET", [](HTTPRequest * req, HTTPResponse * res){
        BeeFishBString::BString filename = req->getRequestString();
        Serial.print("Getting ");
        Serial.println(filename.c_str());
        File file = SPIFFS.open(filename.c_str(), "r");
        size_t size = file.size();
        size_t chunkSize = getPageSize();
        size_t read = 0;
        uint8_t * nbuf = (uint8_t*)malloc(chunkSize);
        while (read < size) {
            if (read + chunkSize > size)
                chunkSize = size - read;
            read += file.read(nbuf, chunkSize);
            res->write(nbuf, chunkSize);
        }
        file.close();
        free(nbuf);
    });

    secureServer->registerNode(nodeStatus);
    secureServer->setDefaultNode(nodeDefault);
    
    secureServer->start();
   
    if (secureServer->isRunning()) {
        Serial.println("Ok");
    }
/*
    server.beginSecure("/server.cer", "/server.key", NULL);

*/
}

void loop() {
    secureServer->loop();
}
