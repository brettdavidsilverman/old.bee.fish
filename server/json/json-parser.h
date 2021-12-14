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

      OnKeys _onkeys = {};
      OnValues _onvalues {};

   public:
      JSONParser(Match& match) :
         Parser(match)
      {
      }
      
      virtual ~JSONParser()
      {
      }
      
      void captureValue(const BString& key, BeeFishMisc::optional<BString>& value) {
         _onvalues[key] = 
            [&value] (const BString& key, JSON& json) {
               value = json.value();
            };
      }
      
      void streamValue(const BString& key, BeeFishBString::BStringStream::OnBuffer onbuffer) {
         OnKey onkey =
            [onbuffer] (const BString& key, JSON& json) {
               json._string->_onbuffer = onbuffer;
            };
         _onkeys[key] = onkey; 

      }

      void invokeValue(const BString& key, BeeFishJSON::JSONParser::OnValue onvalue) {
         _onvalues[key] = onvalue;
      }

      void clear() {
         _onkeys.clear();;
         _onvalues.clear();
      }

   };

   // Declared in object.h
   inline void Object::matchedKey(String& key, LoadOnDemand<JSON>& value) {

      JSONParser* parser = dynamic_cast<JSONParser*>(_parser);
      if (parser) {
         if (parser->_onkeys.count(key.value()) > 0) {
            JSONParser::OnKey onkey = parser->_onkeys[key.value()];
            JSON* json = dynamic_cast<JSON*>(value._match);
            if (!json->_setup)
               json->setup(parser);
            onkey(key.value(), *json);
         }
      }

   } 

   // Declared in object.h
   inline void Object::matchedSetItem(Object::_KeyValue* item) {
      
      JSONParser* parser = dynamic_cast<JSONParser*>(_parser);
      if (parser) {
         const BString& key = item->_key->value();
         if (parser->_onvalues.count(key) > 0) {
            JSONParser::OnValue onvalue = parser->_onvalues[key];
            if (!item->_value->_setup)
               item->_value->setup(parser);

            JSON* json = dynamic_cast<JSON*>(item->_value->_match);

            onvalue(key, *json);
         }

      }

      Set::matchedSetItem(item);

   }

}

#endif