#ifndef FEEBEE_CAM__WEB_REQUEST_PARSER_H
#define FEEBEE_CAM__WEB_REQUEST_PARSER_H

#include <bee-fish.h>

namespace FeebeeCam
{
   
   using namespace BeeFishBString;
   
   typedef BeeFishJSON::JSONParser WebRequestParserBase;

   class WebRequestParser : public WebRequestParserBase
   {
   public:

//      typedef std::function<void(const BString& key, JSON& json)> OnValue;

   public:
      WebRequestParser(BeeFishParser::Match& match) :
         WebRequestParserBase(match)
      {
      }
      
      virtual ~WebRequestParser()
      {
      }
/*      
      void captureValue(const BString& key, BeeFishMisc::optional<BString>& value) {
         _onvalues[key] = 
            [&value] (const BString& key, JSON& json) {
               if (json.isNull())
                  value = BeeFishMisc::nullopt;
               else
                  value = json.value();
            };
      }
*/      

   };


}

#endif