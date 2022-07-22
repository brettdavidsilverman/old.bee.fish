#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include <secure-web-server.h>
#include <file-server.h>
#include <feebee-cam.h>

#define SSID "laptop"
#define PASSWORD "feebeegeeb3"

using namespace FeebeeCam;
FeebeeCam::SecureWebServer* secureServer;
void serveFile(const BString& filename, HTTPResponse * res);

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

    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeBattery();
    
    Serial.println("Creating secure web server");
    
    secureServer = new FeebeeCam::SecureWebServer(1);

    ResourceNode * nodeRoot = 
        new ResourceNode("/", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            res->setHeader("Connection", "keep-alive");
            std::string response = "Secure Hello World Root!!!";
            std::stringstream stream;
            stream << response.size();
            res->setHeader("Content-Length", stream.str().c_str());
            res->write((const uint8_t*)response.data(), response.size());
        }
    );
 
    secureServer->registerNode(nodeRoot);
 
    ResourceNode * nodeWeather = 
        new ResourceNode("/weather", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            res->setHeader("Connection", "keep-alive");
            res->setHeader("Content-type", "text/javascript; charset=utf-8");
            BeeFishBScript::Object reading = weather.getWeather();
            std::string response = reading.str();
            std::stringstream stream;
            stream << response.size();
            res->setHeader("Content-Length", stream.str().c_str());
            res->write((const uint8_t*)response.data(), response.size());
        }
    );
 
    secureServer->registerNode(nodeWeather);

    ResourceNode * defaultNode  = 
        new ResourceNode("", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            res->setHeader("Connection", "keep-alive");
            std::string filename = req->getRequestString();
            serveFile(filename.c_str(), res);
        }
    );
 
    secureServer->setDefaultNode(defaultNode);

    secureServer->start();
   
    if (secureServer->isRunning()) {
        Serial.println("Server ready.");
    }
}

void serveFile(const BString& filename, HTTPResponse * res) {

    BString _filename = filename;

    if (_filename.endsWith("/"))
        _filename += "index.html";
    
    Serial.print("Getting ");
    Serial.print(_filename.c_str());
    Serial.print(" ");

    if (SPIFFS.exists(_filename.c_str())) {

        File file = SPIFFS.open(_filename.c_str(), "r");
        res->setStatusCode(200);
        res->setStatusText("Ok");

        vector<BString> parts = _filename.split('.');
        BString extension = parts[parts.size() - 1];
        BString contentType = FeebeeCam::CONTENT_TYPES[extension];

        res->setHeader("Connection", "keep-alive");
        res->setHeader("Content-Type", contentType.c_str());

        bool cacheRule = FeebeeCam::CACHE_RULES[extension];
        
        cacheRule = true;

        if (cacheRule)
            res->setHeader("Cache-Control", "public, max-age=31536000, immutable");
        else
            res->setHeader("Cache-Control", "no-store, max-age=0");

        std::stringstream stream;
        stream << file.size();
        res->setHeader("Content-Length", stream.str().c_str());

        // Finished headers
        size_t size = file.size();
        size_t chunkSize = getpagesize();
        size_t written = 0;
        uint8_t* buffer = (uint8_t*)malloc(chunkSize);
        while (written < size) {
            if (written + chunkSize > size)
                chunkSize = size - written;
            written += file.read(buffer, chunkSize);
            res->write(buffer, chunkSize);
            //Serial.write(buffer, chunkSize);
        }
        file.close();
        free(buffer);
        Serial.println("Ok");
    }
    else {
        res->setStatusCode(400);
        res->setStatusText("Not Found");
        res->setHeader("Connection", "keep-alive");
        res->setHeader("Content-Type", "text/javascript");
        std::string response = "{\"status\": \"Not found\"}";
        res->write((Byte*)response.c_str(), response.size());
        Serial.println("File Not Found");
    }

};

void loop() {
    FeebeeCam::handleCommandLine();
}