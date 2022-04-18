#include "weather.h"

namespace FeebeeCam {

    Weather weather;
    
    bool onWeather(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        BeeFishJSONOutput::Object& reading = weather.getWeather();

        WiFiWebServer::sendResponse(client, reading);

        return true;
    };

}