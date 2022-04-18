#ifndef BEE_FISH__B_SCRIPT_PARSER_H
#define BEE_FISH__B_SCRIPT_PARSER_H

#include "../parser/parser.h"
#include "../json/json.h"
#include "object.h"

namespace BeeFishBScript
{
   
   using namespace BeeFishBString;

   class BScriptParser : public BeeFishJSON::JSONParser
   {
   public:
      BeeFishJSON::JSON _json;

      std::vector<BeeFishBScript::Object> _stack;

   public:

      BScriptParser() :
         JSONParser(_json)
      {
      }
      
      virtual ~BScriptParser()
      {
      }
      
      virtual bool isBScriptParser() {
         return true;
      }

      virtual bool matched() {
         return _json.matched();
      }

      const BeeFishBScript::Object& object() {
         return _stack[0];
      }

      virtual void onbeginobject(Match* match) {
         BeeFishBScript::Object object;
         _stack.push_back(object);
      }

      virtual void onobjectvalue(const BString& key, const BeeFishJSON::JSON& value) {

         BeeFishBScript::Object& object = _stack[_stack.size() - 1];

         switch (value.type()) {
         case BeeFishJSON::Type::JSONNull:
            object[key] = nullptr;
            break;
         case BeeFishJSON::Type::JSONBoolean:
            if (value.value() == "true")
               object[key] = true;
            else if (value.value() == "false")
               object[key] = false;
            else
               throw std::runtime_error("Invalid boolean value");
            break;
         case BeeFishJSON::Type::JSONNumber:
            {
               std::stringstream stream;
               stream << value.value();
               double number = 0;
               stream >> number;
               object[key] = number;
            }
            break;
         case BeeFishJSON::Type::JSONString:
            object[key] = value.value();
            break;
         case BeeFishJSON::Type::JSONArray:
            throw std::runtime_error("Arrays are not supported (yet)");
            break;
         case BeeFishJSON::Type::JSONObject:
            if (_stack.size() >= 2) {
               _stack[_stack.size() - 2][key] = _stack[_stack.size() - 1];
               _stack.pop_back();
            }
            break;
         }

         BeeFishJSON::JSONParser::onobjectvalue(key, value);
      }

      virtual void onendobject(Match* match) {
      }  


   };

//   inline void BeeFishBScript::BScriptParser::Object::matchedSetItem(_KeyValue* item);
}

#endif