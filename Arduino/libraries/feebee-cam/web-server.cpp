#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "setup.h"

namespace FeebeeCam {

    BeeFishWebServer::WebServer* webServer;

    bool initializeWebServer() {

        webServer = new BeeFishWebServer::WebServer(80);

        webServer->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer->paths()["/camera"]           = FeebeeCam::onCamera;
        webServer->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer->paths()["/settings"]         = FeebeeCam::onSettings;
        webServer->paths()["/command"]          = FeebeeCam::onCommand;
        
        webServer->_defaultHandler              = FeebeeCam::onFileServer;

        webServer->start(1);

        //webServer->paths()["/light"]          = FeebeeCam::onLight;
        //webServer->paths()["/setup/settings"] = FeebeeCam::onSetupSettings;
        //webServer->paths()["/weather"]        = FeebeeCam::onWeather;

        return true;

    }


}
