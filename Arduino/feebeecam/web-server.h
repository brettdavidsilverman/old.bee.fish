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

namespace FeebeeCam {

    void cameraServer( void * pvParameters );

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
        ResourceNode* nodeCameraGet = new ResourceNode("/camera", "GET", onCameraGet);

        ResourceNode* nodeDefault = new ResourceNode("", "GET", onFileServer);

        secureServer.registerNode(nodeStatus);
        secureServer.registerNode(nodeSetupPost);
        secureServer.registerNode(nodeCameraGet);
        secureServer.setDefaultNode(nodeDefault);

        secureServer.start();
    
        if (secureServer.isRunning()) {
            Serial.println("Web server started");
        }

        BaseType_t xReturned;
        TaskHandle_t xHandle = NULL;

        xReturned = xTaskCreatePinnedToCore(
            cameraServer,      /* Task function. */
            "CameraServer",    /* String with name of task. */
            10000,          /* Stack size in bytes. */
            NULL,           /* Parameter passed as input of the task */
            1,              /* Priority of the task. */
            &xHandle        /* Task handle. */,
            0               /* Pinned to core */
        );      

        if (xReturned == pdPASS) {
            Serial.println("Task for Camera Server started");
        }

        for (;;) {
            secureServer.loop();
            delay(10);
        }
    }

    void cameraServer( void * pvParameters ) {
    
        using namespace httpsserver;

        Serial.println("Setting up camera server");

        HTTPSServer secureServer(sslCert, 444);

        ResourceNode* nodeStatus = new ResourceNode("/status", "GET", [](HTTPRequest * req, HTTPResponse * res){
            res->setStatusCode(200);
            setDefaultHeaders(res);
            res->println("{\"status\": \"Ok\"}");
        });

        ResourceNode* nodeCameraGet = new ResourceNode("/camera", "GET", onCameraGet);

        secureServer.registerNode(nodeStatus);
        secureServer.registerNode(nodeCameraGet);

        secureServer.start();
    
        if (secureServer.isRunning()) {
            Serial.println("Camera server started");
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
            2,              /* Priority of the task. */
            &xHandle        /* Task handle. */,
            1               /* Pinned to core */
        );      

        if (xReturned == pdPASS) {
            Serial.println("Task for Web Server started");
        }

    }
}
