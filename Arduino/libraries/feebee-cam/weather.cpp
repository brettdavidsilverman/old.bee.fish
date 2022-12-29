#include <feebee-cam.h>

namespace FeebeeCam
{

   FeebeeCam::Weather weather1(1);
   FeebeeCam::Weather weather2(2);

   void initializeWeather() {

      if (!weather1.initialize()) {
         cerr << "Error initializing weather sensor 1" << endl;
      }

      if (!weather2.initialize()) {
         cerr << "Error initializing weather sensor 2" << endl;
      }

   }

   bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      cerr << "onWeather" << endl;

      bool extended = false;
      BeeFishWeb::WebRequest::URL::Query& query = client->_webRequest.queryObject();

      if (query.contains("extended")) {
         if ( query["extended"] == "" ||
              query["extended"] == "true" )
         {
            extended = true;
         }
      }

      BeeFishBScript::Object object = FeebeeCam::Weather::getWeather(extended);

      client->_contentType = "application/json; charset=utf-8";
      client->_chunkedEncoding = false;
      client->_contentLength = object.contentLength();

      if (!client->sendHeaders()) {
         return false;
      }

      BeeFishBString::BStream& output = client->getOutputStream();

      output << object;

      client->flush();

      cerr << "~onWeather" << endl;

      return true;
   }


   bool uploadWeatherReport() {

      if (!FeebeeCam::_setup->_isSetup)
      {
         cerr << "Missing setup secret hash for uploadWeatherReport" << endl;
         return false;
      }

      BeeFishBScript::Object reading = FeebeeCam::Weather::getWeather(false);

      FeebeeCam::BeeFishStorage storage("/beehive/weather/");
      BeeFishId::Id id("application/json; charset=utf-8");
      bool uploaded = storage.setItem(id, reading);
      BString weatherURL = storage.url();

      if (!uploaded) {
         cerr << "Error uploading weather report" << endl;
         RESTART_AFTER_ERROR();
      }

      status._lastWeatherURL =  weatherURL;

      return true;
   }

   BeeFishBScript::Object Weather::getWeather(bool extended) {

      BeeFishBScript::Object reading;

      reading["Label"] =
            BeeFishBScript::Object {
               {"value", _setup->_label}
            };

      reading["Date time"] =
            BeeFishBScript::Object {
               {"value", FeebeeCam::getDateTime()}
            };

      reading["Battery"] = BeeFishBScript::Object {
            {"value", bat_get_voltage()},
            {"unit", "V"},
            {"precision", 2}
      };

      reading["Weather 1"] = weather1.getWeather();
      reading["Weather 2"] = weather2.getWeather();

      double frameRate = getFrameRate();
      if (frameRate > 0.0) {
            reading["Frame rate"] = BeeFishBScript::Object{
               {"value", frameRate},
               {"unit", "frames/sec"},
               {"precision", 2}
            };
      }

      if (extended) {

         BeeFishBScript::Object extended;

         extended["Memory"] =
            BeeFishBScript::Object {
               {"value", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
               {"unit", "% free"},
               {"precision", 2}
            };

         if (ESP.getPsramSize() > 0) {

            extended["External memory"] =
               BeeFishBScript::Object {
                     {"value", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0},
                     {"unit", "% free"},
                     {"precision", 2}
               };
         }

         extended["Free sketch size"] = BeeFishBScript::Object {
            {"value", ESP.getFreeSketchSpace()},
            {"unit", "bytes"},
            {"precision", 0}
         };


         extended["URL"] =
            BeeFishBScript::Object {
               {"value", FeebeeCam::getURL()},
               {"unit", "url"},
               {"label", "Beehive local"}
            };

         extended["Last image URL"] =
            BeeFishBScript::Object {
               {"value", FeebeeCam::status._lastImageURL},
               {"unit", "url"},
               {"label", "Last Image"}
            };

         extended["Previous Weather URL"] =
            BeeFishBScript::Object {
               {"value", FeebeeCam::status._lastWeatherURL},
               {"unit", "url"},
               {"label", "Previous weather URL"}
            };
            
         reading["extended"] = extended;
      }

      return reading;
   }


}