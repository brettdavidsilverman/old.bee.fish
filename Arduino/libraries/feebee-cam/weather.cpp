#include "weather.h"
#include "web-storage.h"

namespace FeebeeCam {

    Weather weather;
    
    bool onWeather(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        BeeFishBScript::Object& reading = weather.getWeather();

        WiFiWebServer::sendResponse(client, reading);

        return true;
    };

    bool uploadWeatherReport() {

      BeeFishId::Id id;

      cout << "Weather Id: " << id << endl;
      
      BeeFishStorage storage;

      return storage.setItem("/beehive/weather/", id, weather.getWeather());

    }

}