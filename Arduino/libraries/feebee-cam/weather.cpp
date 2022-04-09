#include "weather.h"

namespace FeebeeCam {

    bool onWeather(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        Weather weather;
        BeeFishJSONOutput::Object& reading = weather.getWeather();

        WiFiWebServer::sendResponse(client, reading);

        return true;
    };

}