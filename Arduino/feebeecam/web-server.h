#include <map>
#include <FS.h>
#include <SPIFFS.h>
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <ssl-cert.h>
#include <bee-fish.h>
#include "setup.h"

namespace FeebeeCam {

    std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES = {
        {"html", "text/html; charset=utf-8"},
        {"js", "text/javascript; charset=utf-8"},
        {"jpg", "image/jpeg"},
        {"gif", "image/gif"}
    };

    void webServer( void * pvParameters ) {
    
        using namespace httpsserver;

        Serial.println("Setting up web server");

        initializeSSLCert();

        HTTPSServer secureServer(sslCert);

        ResourceNode* nodeStatus = new ResourceNode("/status", "GET", [](HTTPRequest * req, HTTPResponse * res){
            res->setStatusCode(200);
            setDefaultHeaders(res);
            res->println("{\"status\": \"Ok\"}");
        });

        ResourceNode* nodeSetupPost = new ResourceNode("/setup", "POST", onSetupPost);

        ResourceNode* nodeDefault = new ResourceNode("", "GET", [](HTTPRequest * req, HTTPResponse * res){
            using namespace BeeFishBString;

            BString filename = req->getRequestString();
            Serial.print("Getting ");
            Serial.print(filename.c_str());
            File file = SPIFFS.open(filename.c_str(), "r");
            if (file) {

                res->setStatusCode(200);

                vector<BString> parts = filename.split('.');
                const BString& extension = parts[parts.size() - 1];
                const BString& contentType = CONTENT_TYPES[extension];
                res->setHeader("Connection", "keep-alive");
                res->setHeader("Content-Type", contentType.c_str());

                size_t size = file.size();
                size_t chunkSize = getpagesize();
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
                Serial.println("Ok");
            }
            else {
                res->setStatusCode(404);
                res->println("{\"status\": \"Not found\"}");
                Serial.println("File Not Found");
            }
        });

        secureServer.registerNode(nodeStatus);
        secureServer.registerNode(nodeSetupPost);
        secureServer.setDefaultNode(nodeDefault);

        secureServer.start();
    
        if (secureServer.isRunning()) {
            Serial.println("Web server started");
        }

        for (;;) {
            secureServer.loop();
            delay(10);
        }
    }


    void initializeWebServer() {

        BaseType_t xReturned;
        TaskHandle_t xHandle = NULL;

        xReturned = xTaskCreatePinnedToCore(
            webServer,      /* Task function. */
            "WebServer",    /* String with name of task. */
            10000,          /* Stack size in bytes. */
            NULL,           /* Parameter passed as input of the task */
            1,              /* Priority of the task. */
            &xHandle        /* Task handle. */,
            1               /* Pinned to core */
        );      

        if (xReturned == pdPASS) {
            Serial.println("Task for Web Server started");
        }

    }
}
