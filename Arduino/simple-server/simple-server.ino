//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//
#include <Arduino.h>
#include <WiFi.h>

#define ASYNC_TCP_SSL_ENABLED 1
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "Laptop";
const char* password = "feebeegeeb3";

const char* PARAM_MESSAGE = "message";

IPAddress ipAddress(10, 10, 1, 1);
IPAddress gateway(10, 10, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

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
    
    WiFi.onEvent(clientConnected, SYSTEM_EVENT_AP_STACONNECTED);

    //WiFi.mode(WIFI_AP);

    //WiFi.softAPConfig(ipAddress, gateway, subnet);

    //WiFi.softAP(ssid, password);
    WiFi.begin(ssid, password);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);
    
//    server.beginSecure("/server.cer", "/server.key", NULL);

    server.begin();    
}

void loop() {
}
