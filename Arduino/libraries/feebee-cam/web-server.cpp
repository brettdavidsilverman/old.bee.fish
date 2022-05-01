#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "setup.h"
#include "settings.h"

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
        webServer1->requests()["/light"]          = onLight;
        webServer1->requests()["/setup/settings"] = onSetupSettings;
        webServer1->requests()["/weather"]        = onWeather;
        webServer1->requests()["/settings"]       = onSettings;

        webServer1->requests()["/restart"] = 
            [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
                ESP.restart();
                return true;
            };


        webServer1->setDefaultRequest(onFileServer);

    }

}
