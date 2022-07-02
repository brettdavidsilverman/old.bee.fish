#include "weather.h"
#include "web-storage.h"

namespace FeebeeCam
{

   Weather weather;

   bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {

      client->_contentType = "text/javascript; charset=utf-8";

      if (!client->sendHeaders())
         return false;

      BeeFishBString::BStream output = client->getChunkedOutputStream();

      output << FeebeeCam::weather.getWeather() << endl;

      output.flush();

      if(!client->sendChunk())
         return false;

      return true;
   }

   bool uploadWeatherReport()
   {

      if (FeebeeCam::_setup._secretHash.length() == 0)
      {
         cerr << "Missing setup secret hash " << endl;
         return false;
      }
      /*
            if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup._secretHash)) {
                  cerr << "Couldnt upload weather report... Couldn't log on" << endl;
                  return false;
            }
      */
      static FeebeeCam::BeeFishStorage *storage = nullptr;

      if (!storage)
         storage = new FeebeeCam::BeeFishStorage("/beehive/weather/");

      BeeFishId::Id id;

      bool uploaded = storage->setItem(id, FeebeeCam::weather.getWeather());

      if (uploaded)
         cout << "Weather report uploaded with id " << id << endl;
      else
      {
         cerr << "Error uploading weather report" << endl;
         delete storage;
         storage = nullptr;
      }

      return uploaded;
   }

}