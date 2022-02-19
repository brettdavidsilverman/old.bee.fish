#include <map>
#include "freertos/task.h"
#include <FS.h>
#include <SPIFFS.h>
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <bee-fish.h>
#include <wifi-web-server.h>
#include <weather.h>
#include "camera.h"
#include "file-server.h"
#include "i2c.h"

//#include "setup.h"
//#include "command.h"

namespace FeebeeCam {

    WiFiWebServer* cameraServer;
    WiFiWebServer* webServer;

    void initializeWebServers() {
    
        Serial.println("Setting up web servers");

        webServer    = new WiFiWebServer(80, 0);
        cameraServer = new WiFiWebServer(81, 1);

        webServer->requests()["/status"] =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                BeeFishJSONOutput::Object status = {
                    {"status", "Ok"}
                };
                cout << status << endl;
                WiFiWebServer::sendResponse(client, status);
                return true;
            };

        webServer->requests()["/weather"] =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                Weather weather(&wire);
                BeeFishJSONOutput::Object reading = weather.getWeather();
                WiFiWebServer::sendResponse(client, reading);
                return true;
            };

        webServer->setDefaultRequest(onFileServer);

        cameraServer->requests()["/camera"] = onCameraGet;

    /*
        ResourceNode* nodeSetupPost = new ResourceNode("/setup", "POST", onSetupPost);
        ResourceNode* nodeCommandPost = new ResourceNode("/command", "POST", onCommandPost);
        ResourceNode* nodeWeatherGet = new ResourceNode("/weather", "GET", onWeatherGet);
        ResourceNode* nodeWeatherPost = new ResourceNode("/weather", "POST", onWeatherGet);

        ResourceNode* nodeDefault = new ResourceNode("", "GET", onFileServer);

        webServer.registerNode(nodeStatus);
        webServer.registerNode(nodeSetupPost);
        webServer.registerNode(nodeCommandPost);
        webServer.registerNode(nodeWeatherGet);
        webServer.registerNode(nodeWeatherPost);
        webServer.setDefaultNode(nodeDefault);

        webServer.start();
    
        if (webServer.isRunning()) {
            Serial.println("Web server started");
        }


        for (;;) {
            webServer.loop();
            delay(10);
        }
        */
    }

/*
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
*/
}
