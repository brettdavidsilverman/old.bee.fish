#include "weather.h"
#include "web-storage.h"

namespace FeebeeCam
{

   Weather weather;
   BeeFishBScript::Object weatherReading;

   bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      client->_contentType = "text/javascript; charset=utf-8";

      if (!client->sendHeaders())
         return false;

      BeeFishBString::BStream output = client->getChunkedOutputStream();

      output << weatherReading << endl;

      output.flush();

      if(!client->sendChunk())
         return false;

      return true;
   }

   bool readWeather() {
      FeebeeCam::weatherReading = FeebeeCam::weather.getWeather();
      return true;
   }

   bool uploadWeatherReport()
   {

      if (FeebeeCam::setup._secretHash.length() == 0)
      {
         cerr << "Missing setup secret hash " << endl;
         return false;
      }

      FeebeeCam::BeeFishStorage storage("/beehive/weather/");

      BeeFishId::Id id;

      bool uploaded = storage.setItem(id, weatherReading);

      if (uploaded)
         cout << "Weather report uploaded with id " << id << endl;
      else
      {
         cerr << "Error uploading weather report" << endl;
         FeebeeCam::resetConnection();
      }

      return uploaded;
   }

}