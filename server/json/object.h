#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

#include "../power-encoding/power-encoding.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishBScript {
   class BScriptParser;
}

namespace BeeFishJSON {
   
   class JSON;
   class Object; 

   class ObjectOpenBrace : public And {
   public:
      ObjectOpenBrace() : And(
         new BeeFishParser::Character('{'),
         new Optional(new BlankSpace())
      )
      {

      }

   };

   class ObjectCloseBrace : public And {
   public:
      ObjectCloseBrace() : And(
         new Optional(new BlankSpace()),
         new BeeFishParser::Character('}') 
      )
      {

      }
   };

   class ObjectFieldSeperator: public BeeFishParser::Character {
   public:
      ObjectFieldSeperator() : Character(',') {

      }
   };

   class ObjectKeyValueSeperator : public BeeFishParser::Character {

   public:
      ObjectKeyValueSeperator() : Character(':') {

      }
   };

   typedef String ObjectKey;
   typedef LoadOnDemand<JSON> ObjectValue;

   class Object:
      public KeyedSet<
         ObjectOpenBrace, 
         ObjectKey,
         ObjectKeyValueSeperator,
         ObjectValue,
         ObjectFieldSeperator,
         ObjectCloseBrace
      >
   {
   public:
      typedef std::function<void(const BString& key, JSON& value)> OnKeyValue;
      OnKeyValue _onkeyvalue = nullptr;
   public:

      Object() : KeyedSet()
      {
      }

      BeeFishBScript::BScriptParser* bScriptParser() {
         return (BeeFishBScript::BScriptParser*)_parser;
      }

      virtual void onbeginset(Match* match) {
         if (_parser->isBScriptParser()) {
            //BeeFishBScript::Object object;
            //bScriptParser()->_stack.push_back(object);
            cout << "onbeginset" << endl;
         }
      }

      virtual void onendset(Match* match) {
         if (_parser->isBScriptParser()) {
            //std::vector<BeeFishBScript::Object>& stack = bScriptParser()->_stack;
            //if (stack.size() > 1)
            //   stack.pop_back();
            cout << "onendset" << endl;
         }
      }
/*
         virtual void matchedSetItem(_KeyValue* item) {
            const BString& key = item->_key->value();
            LoadOnDemand<BeeFishJSON::JSON>* value = item->_value;
            const BeeFishJSON::JSON* json = (const BeeFishJSON::JSON*)(value->_match);
            cout << key << endl;
            
            BScriptParser* _parser = parser();
            std::vector<BeeFishBScript::Object>& stack = _parser->_stack;
            BeeFishBScript::Object& object = stack[stack.size() - 1];
            
            switch (json->type())
            {
            case BeeFishJSON::Type::JSONNull:
               object[key] = Variable(nullptr);
               break;
            case BeeFishJSON::Type::JSONNumber: {

               std::stringstream stream;
               stream << json->value();
               double number;
               stream >> number;
               object[key] = Variable(number);
               break;
            }
            case BeeFishJSON::Type::JSONString:
               object[key] = Variable(json->value());
               break;
            case BeeFishJSON::Type::JSONArray:
               object[key] = Variable(BeeFishBScript::Array());
               break;
            case BeeFishJSON::Type::JSONObject:
               break;
            default:
               cerr << "Invalid JSON type" <<  endl;
            }

            BeeFishJSON::Object::matchedSetItem(item);
         }
*/
      // Defined in json-parser.h
      virtual void matchedKey(String& key, LoadOnDemand<JSON>& value);
      // Defined in json-parser.h
      virtual void matchedSetItem(_KeyValue* item);

      virtual void setOnKeyValue(OnKeyValue onkeyvalue) {
         _onkeyvalue = onkeyvalue;
      }
   };

}

#endif