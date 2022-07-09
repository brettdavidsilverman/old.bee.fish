#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "setup.h"

namespace FeebeeCam {

    BeeFishWebServer::WebServer* webServer80;
    BeeFishWebServer::WebServer* webServer8080;

    bool initializeWebServer() {

        webServer80 = new BeeFishWebServer::WebServer(80);
        webServer8080 = new BeeFishWebServer::WebServer(8080);

        webServer80->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer80->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer80->paths()["/settings"]         = FeebeeCam::onSettings;
        webServer80->paths()["/command"]          = FeebeeCam::onCommand;
        webServer8080->paths()["/camera"]           = FeebeeCam::onCamera;
        
        webServer80->_defaultHandler              = FeebeeCam::onFileServer;

        webServer80->start(1);
        webServer8080->start(0);

        //webServer->paths()["/light"]          = FeebeeCam::onLight;
        //webServer->paths()["/setup/settings"] = FeebeeCam::onSetupSettings;
        //webServer->paths()["/weather"]        = FeebeeCam::onWeather;

        return true;

    }


}
