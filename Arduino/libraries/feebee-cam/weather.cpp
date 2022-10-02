#include <feebee-cam.h>

namespace FeebeeCam
{
   Weather weather;

   bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      client->_contentType = "application/json; charset=utf-8";

      if (!client->sendHeaders())
         return false;

      BeeFishBString::BStream& output = client->getChunkedOutputStream();

      bool extended = false;
      auto query = client->_webRequest.queryObject();
      if (query.count("extended")) {
         extended = (query["extended"] == "true");
      }

      BeeFishBScript::Object object = FeebeeCam::weather.getWeather(extended);

      output << object.str() << "\r\n";

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

      BeeFishBScript::Object reading = FeebeeCam::weather.getWeather(true);

      FeebeeCam::BeeFishStorage storage("/beehive/weather/");
      BeeFishId::Id id("application/json; charset=utf-8");
      bool uploaded = storage.setItem(id, reading);
      BString weatherURL = storage.url();

      if (!uploaded) {
         cerr << "Error uploading weather report" << endl;
         FeebeeCam::restartAfterError();
      }

      FeebeeCam::settings["lastWeatherURL"] =  weatherURL;
      
      return true;
   }


}