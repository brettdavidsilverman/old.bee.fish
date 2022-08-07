#include "weather.h"
#include "web-storage.h"

namespace FeebeeCam
{
   Weather weather;

   bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      client->_contentType = "text/javascript; charset=utf-8";

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

      if (FeebeeCam::setup._secretHash.length() == 0)
      {
         cerr << "Missing setup secret hash for uploadWeatherReport" << endl;
         return false;
      }

      
      FeebeeCam::BeeFishStorage storage("/beehive/weather/");

      BeeFishId::Id id("image");
      
      BeeFishBScript::Object reading = FeebeeCam::weather.getWeather();

      // Capture a high-res image
      BeeFishBString::Data* image = FeebeeCam::getImage();
      
      if (image) {
         
         reading["image"] = BeeFishBScript::Object {
            {"type", "base64;jpeg"},
            {"value", image->toBase64("data:image/jpeg;base64,")}
         };
         
         delete image;
      }

      bool uploaded = storage.setItem(id, reading);

      if (uploaded)
         cout << "Weather report uploaded with id " << id << endl;
      else {
         cerr << "Error uploading weather report" << endl;
         FeebeeCam::resetConnection();
      }

      return uploaded;
   }

}