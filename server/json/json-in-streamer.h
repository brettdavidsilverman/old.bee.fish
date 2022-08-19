#ifndef BEE_FISH__JSON_IN_STREAMER_H
#define BEE_FISH__JSON_IN_STREAMER_H

#include "json-parser.h"
#include "../database/path.h"

namespace BeeFishJSON {

   using namespace BeeFishBString;
   typedef BeeFishDatabase::Path<BeeFishPowerEncoding::PowerEncoding> Path;
   
   class JSONInStreamer :
      public JSONParser,
      protected Path
   {
   protected:
      JSON _json;
   public:
      JSONInStreamer(
         Path& path
      ) :
         JSONParser(_json),
         Path(path)
      {
      }
      
      virtual void onbeginobject(Match* match) {
      }

      virtual void onendobject(Match* match) {
      }

      virtual void onbeginarray(Match* match) {
      }

      virtual void onarrayvalue(Match* match) {
      }

      virtual void onendarray(Match* match) {
      }

      virtual void onobjectvalue(const BString& key, const JSON* value) {
      }

      virtual void onvalue(JSON* json) {
      }

     
   };
   
   
}

#endif