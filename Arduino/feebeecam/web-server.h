#include <map>
#include "freertos/task.h"
#include <FS.h>
#include <SPIFFS.h>
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <ssl-cert.h>
#include <bee-fish.h>
#include "setup.h"
#include "file-server.h"
#include "camera.h"
#include "command.h"

namespace FeebeeCam {

    void cameraServer( void * pvParameters );

    void webServer( void * pvParameters ) {
    
        using namespace httpsserver;

        Serial.println("Setting up web server");

        //initializeSSLCert();

        //HTTPSServer secureServer(sslCert);
        HTTPServer webServer;

        ResourceNode* nodeStatus = new ResourceNode("/status", "GET", [](HTTPRequest * req, HTTPResponse * res){
            res->setStatusCode(200);
            setDefaultHeaders(res);
            res->println("{\"status\": \"Ok\"}");
        });

        ResourceNode* nodeSetupPost = new ResourceNode("/setup", "POST", onSetupPost);
        ResourceNode* nodeCommandPost = new ResourceNode("/command", "POST", onCommandPost);

        ResourceNode* nodeDefault = new ResourceNode("", "GET", onFileServer);

        webServer.registerNode(nodeStatus);
        webServer.registerNode(nodeSetupPost);
        webServer.registerNode(nodeCommandPost);
        webServer.setDefaultNode(nodeDefault);

        webServer.start();
    
        if (webServer.isRunning()) {
            Serial.println("Web server started");
        }


        for (;;) {
            webServer.loop();
            delay(10);
        }
    }

    void cameraServer( void * pvParameters ) {
    
        using namespace httpsserver;

        Serial.println("Setting up camera server");

        HTTPServer cameraServer(81);

        ResourceNode* nodeStatus = new ResourceNode("/status", "GET", [](HTTPRequest * req, HTTPResponse * res){
            res->setStatusCode(200);
            setDefaultHeaders(res);
            res->println("{\"status\": \"Ok\"}");
        });

        ResourceNode* nodeCameraGet = new ResourceNode("/camera", "GET", onCameraGet);

        cameraServer.registerNode(nodeStatus);
        cameraServer.registerNode(nodeCameraGet);

        cameraServer.start();
    
        if (cameraServer.isRunning()) {
            Serial.println("Camera server started");
        }

        for (;;) {
            cameraServer.loop();
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
            0               /* Pinned to core */
        );      

        if (xReturned == pdPASS) {
            Serial.println("Task for Web Server started");
        }

        xReturned = xTaskCreatePinnedToCore(
            cameraServer,      /* Task function. */
            "CameraServer",    /* String with name of task. */
            10000,          /* Stack size in bytes. */
            NULL,           /* Parameter passed as input of the task */
            1,              /* Priority of the task. */
            &xHandle        /* Task handle. */,
            1               /* Pinned to core */
        );      

        if (xReturned == pdPASS) {
            Serial.println("Task for Camera Server started");
        }
    }
}
