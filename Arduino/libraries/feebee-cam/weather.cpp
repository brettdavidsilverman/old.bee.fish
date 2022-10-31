#include <feebee-cam.h>

namespace FeebeeCam
{
   Weather weather;

   bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      
      bool extended = false;
      BeeFishWeb::WebRequest::URL::Query& query = client->_webRequest.queryObject();

      if (query.contains("extended")) {
         if ( query["extended"] == "" ||
              query["extended"] == "true" )
         {
            extended = true;
         }
      }

      BeeFishBScript::Object object = FeebeeCam::weather.getWeather(extended);

      client->_contentType = "application/json; charset=utf-8";
      client->_chunkedEncoding = false;
      client->_contentLength = object.contentLength();

      if (!client->sendHeaders()) {
         return false;
      }

      BeeFishBString::BStream& output = client->getOutputStream();

      output << object;

      client->flush();

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
         RESTART_AFTER_ERROR();
      }

      FeebeeCam::settings["lastWeatherURL"] =  weatherURL;
      
      return true;
   }


}