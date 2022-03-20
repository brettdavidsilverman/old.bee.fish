#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


// Replace with your network credentials
const char* ssid     = "ESP32";
const char* password = "feebeegeeb3";

IPAddress ipAddress(10, 10, 1, 1);
IPAddress gateway(10, 10, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer webserver(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void clientConnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    Serial.println("WiFi AP Client Connected");

    IPAddress IP = WiFi.softAPIP();
    
    Serial.print("AP IP address: ");
    Serial.println(IP);
  
    webserver.begin();
}

void initializeWebSite() {
    WiFi.disconnect(true, true);

    WiFi.onEvent(clientConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);

    WiFi.mode(WIFI_AP);

    WiFi.softAPConfig(ipAddress, gateway, subnet);

    WiFi.softAP(ssid, password);

    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    webserver.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    webserver.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    webserver.onNotFound(notFound);


}

void wifiLoop() {
    
}
