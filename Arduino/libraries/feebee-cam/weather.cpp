#include <feebee-cam.h>

namespace FeebeeCam
{
   Weather weather;

   bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      client->_contentType = "application/json; charset=utf-8";

      if (!client->sendHeaders())
         return false;

      BeeFishBString::BStream& output = client->getChunkedOutputStream();

      output << FeebeeCam::weather.getWeather() << endl;

      if(!client->sendFinalChunk())
         return false;

      return true;
   }


   bool uploadWeatherReport()
   {

      if (!FeebeeCam::_setup->_isSetup)
      {
         cerr << "Missing setup secret hash for uploadWeatherReport" << endl;
         return false;
      }



      BeeFishBScript::Object reading = FeebeeCam::weather.getWeather();

      FeebeeCam::BeeFishStorage storage("/beehive/weather/");
      BeeFishId::Id id("json");
      bool uploaded = storage.setItem(id, reading);

      if (uploaded) {
         cout << "Weather report uploaded with id " << id << endl;
         return true;
      }
      else {
         cerr << "Error uploading weather report" << endl;
         FeebeeCam::restartAfterError();
      }

      return false;
   }


}