#include "settings.h"

namespace FeebeeCam {

    Settings settings;

    bool onSettings(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        WiFiWebServer::sendResponse(client, settings);

        return true;
    };

}