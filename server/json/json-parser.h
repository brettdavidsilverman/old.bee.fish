#ifndef BEE_FISH__JSON_PARSER_H
#define BEE_FISH__JSON_PARSER_H

#include "../parser/parser.h"
#include "json.h"

namespace BeeFishJSON
{
   
   using namespace BeeFishBString;

   class JSONParser : public Parser
   {
   public:

      typedef std::function<void(const BString& key, JSON& json)> OnKey;
      typedef std::map<const BString, OnKey> OnKeys;
      typedef std::function<void(const BString& key, JSON& json)> OnValue;
      typedef std::map<const BString, OnValue> OnValues;

      inline static OnKeys _onkeys = {};
      inline static OnValues _onvalues {};

   public:
      JSONParser(Match& match) :
         Parser(match)
      {
      }
      
      virtual ~JSONParser()
      {
      }
      
      inline static void captureValue(const BString& key, BeeFishMisc::optional<BString>& value) {
         JSONParser::_onvalues[key] = 
            [&value] (const BString& key, JSON& json) {
               value = json.value();
            };
      }
      
      inline static void streamValue(const BString& key, BeeFishBString::BStringStream::OnBuffer onbuffer) {
         JSONParser::OnKey onkey =
            [onbuffer] (const BString& key, JSON& json) {
               json._string->_onbuffer = onbuffer;
            };

         JSONParser::_onkeys[key] = onkey; 
      }

      inline static void invokeValue(const BString& key, BeeFishJSON::JSONParser::OnValue onvalue) {
         JSONParser::_onvalues[key] = onvalue;
      }

      inline static void clear() {
         JSONParser::_onkeys.clear();;
         JSONParser::_onvalues.clear();
      }

   };

   // Declared in object.h
   inline void Object::keyMatched(Key& key) {
      JSONParser::OnKey onkey = JSONParser::_onkeys[key.value()];
      if (onkey) {
         LoadOnDemand<JSON>* load = key.keyedValue();
         if (!load->_setup)
            load->setup();

         JSON* json = (JSON*)(load->_match);
            onkey(key.value(), *json);
      }

   } 

   // Declared in object.h
   inline void Object::matchedSetItem(ObjectKeyValue* item) {
      const BString& key = item->_key->value();
      JSONParser::OnValue onvalue = JSONParser::_onvalues[key];
      if (onvalue) {
         
         if (!item->_fieldValue->_setup)
            item->_fieldValue->setup();

         JSON* json = (JSON*)(item->_fieldValue->_match);

         onvalue(key, *json);
      }

      //Set::matchedSetItem(item);

   }

}

#endif