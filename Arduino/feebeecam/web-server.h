#include <map>
#include "freertos/task.h"
#include <FS.h>
#include <SPIFFS.h>
#include <bee-fish.h>
#include <wifi-web-server.h>
#include <camera.h>
#include "file-server.h"
#include "weather.h"
#include "battery.h"

//#include "setup.h"

namespace FeebeeCam {

    WiFiWebServer* webServerCore0;
    WiFiWebServer* webServerCore1;

    double getFramerate();

    void initializeWebServers() {
    
        Serial.println("Setting up web servers");

        webServerCore0 = new WiFiWebServer(80, 0);
        webServerCore1 = new WiFiWebServer(81, 1);

        // Status (alive or not)
        webServerCore0->requests()["/status"] =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                BeeFishJSONOutput::Object status = {
                    {"status", "Ok"}
                };
                cout << status << endl;
                WiFiWebServer::sendResponse(client, status);
                return true;
            };

        // Weather
        webServerCore0->requests()["/weather"] =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {

                Weather weather;

                BeeFishJSONOutput::Object reading = weather.getWeather();

                reading["frame rate"] = BeeFishJSONOutput::Object{
                    {"value", getFramerate()},
                    {"unit", "fps"},
                    {"precision", 2}
                };

                reading["battery"] = BeeFishJSONOutput::Object {
                    {"value", bat_get_voltage()},
                    {"unit", "mV"},
                    {"precision", 0}
                };
                  
                WiFiWebServer::sendResponse(client, reading);

                return true;
            };

        webServerCore0->requests()["/command"] = onCommandPost;
        webServerCore0->requests()["/settings"] = onSettingsePost;
        webServerCore0->requests()["/capture"] = onCaptureGet;

        webServerCore0->setDefaultRequest(onFileServer);

        webServerCore1->requests()["/camera"] = onCameraGet;
    }

    double getFramerate() {
        
        double framerate = FeebeeCam::framesPerSecond;
        
        FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
        FeebeeCam::frameCount = 0;

        return framerate;
    }
}
