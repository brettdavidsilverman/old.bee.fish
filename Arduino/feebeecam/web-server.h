#include <map>
#include "freertos/task.h"
#include <FS.h>
#include <SPIFFS.h>
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <bee-fish.h>
#include <wifi-web-server.h>
#include "camera.h"
#include "file-server.h"
#include "i2c.h"
#include "command.h"
#include <weather.h>

//#include "setup.h"

namespace FeebeeCam {

    WiFiWebServer* cameraServer;
    WiFiWebServer* webServer;

    double getFramerate();

    void initializeWebServers() {
    
        Serial.println("Setting up web servers");

        webServer    = new WiFiWebServer(80, 0);
        cameraServer = new WiFiWebServer(81, 1);

        // Status (alive or not)
        webServer->requests()["/status"] =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                BeeFishJSONOutput::Object status = {
                    {"status", "Ok"}
                };
                cout << status << endl;
                WiFiWebServer::sendResponse(client, status);
                return true;
            };

        // Weather
        webServer->requests()["/weather"] =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {

                Weather weather(&wire);

                BeeFishJSONOutput::Object reading = weather.getWeather();

                reading.insert(
                    {"frame rate", BeeFishJSONOutput::Object {
                        {"value", getFramerate()},
                        {"unit", "fps"},
                        {"precision", 2}
                    }
                });

                WiFiWebServer::sendResponse(client, reading);
                return true;
            };

        webServer->requests()["/command"] = onCommandPost;
        webServer->requests()["/capture"] = onCaptureGet;

        webServer->setDefaultRequest(onFileServer);

        cameraServer->requests()["/camera"] = onCameraGet;
    }

    double getFramerate() {
        
        double framerate = FeebeeCam::framesPerSecond;
        
        FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
        FeebeeCam::frameCount = 0;

        return framerate;
    }
}
