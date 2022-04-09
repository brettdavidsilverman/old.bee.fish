#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "battery.h"
#include "setup.h"

namespace FeebeeCam {

    // Two Web Servers, one on each core
    WiFiWebServer* webServer0;
    WiFiWebServer* webServer1;

    void initializeWebServers() {

        webServer0 = new WiFiWebServer(81, 0);
        webServer1 = new WiFiWebServer(80, 1);
        
        webServer0->requests()["/camera"]         = onCameraGet;

        webServer1->requests()["/capture"]        = onCaptureGet;
        webServer1->requests()["/command"]        = onCommandPost;
        webServer1->requests()["/settings"]       = onSettings;
        webServer1->requests()["/light"]          = onLight;
    //    webServer1->requests()["/setup"]          = onSetup;
        webServer1->requests()["/setup/settings"] = onSetupSettings;

        webServer1->requests()["/weather"]  =
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                Weather weather;
                BeeFishJSONOutput::Object& reading = weather.getWeather();

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

        webServer1->requests()["/restart"] = 
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                ESP.restart();
                return true;
            };


        webServer1->setDefaultRequest(onFileServer);

    }

}
