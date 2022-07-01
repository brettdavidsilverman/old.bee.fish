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

      if (FeebeeCam::isRunning) {

        FeebeeCam::pause = true;

        while (!FeebeeCam::isPaused)
          delay(10);

      }

      if (!BeeFishWebRequest::logon(_setup._secretHash)) {
        FeebeeCam::pause = false;
        return false;
      }

      BeeFishId::Id id;

      BeeFishStorage storage("/beehive/weather/");

      bool uploaded = storage.setItem(id, weather.getWeather());

      if (uploaded)
        cout << "Weather report uploaded with id " << id << endl;
      else
        Serial.println("Error uploading weather report");
      
      FeebeeCam::pause = false;

      return uploaded;

    }

}