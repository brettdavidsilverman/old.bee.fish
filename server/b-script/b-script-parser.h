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


   };

//   inline void BeeFishBScript::BScriptParser::Object::matchedSetItem(_KeyValue* item);
}

#endif