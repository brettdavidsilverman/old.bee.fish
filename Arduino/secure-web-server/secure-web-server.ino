#include <Arduino.h>
#include <secure-web-server.h>

#define SSID "Laptop"
#define PASSWORD "feebeegeeb3"

using namespace FeebeeCam;
FeebeeCam::SecureWebServer* secureServer;

void setup() {

    Serial.begin(1500000);

    while (!Serial)
        delay(10);

    Serial.println("SecureWebServer starting");

    Serial.print("Connecting to WiFi");

    WiFi.begin(SSID, PASSWORD);

    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(500);
        
    }

    Serial.println("Connected");

    Serial.println(WiFi.localIP());
    
    Serial.println("Creating secure web server");
    
    secureServer = new FeebeeCam::SecureWebServer();

    ResourceNode * nodeRoot = 
        new ResourceNode("/", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            res->println("Secure Hello World!!!");
        }
    );
 
    secureServer->registerNode(nodeRoot);
 
    secureServer->start();
   
    if (secureServer->isRunning()) {
        Serial.println("Server ready.");
    }
}

void loop() {
    secureServer->loop();
}