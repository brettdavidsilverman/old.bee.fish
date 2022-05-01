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
      std::vector<BeeFishBScript::Variable> _stack;

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

      const BeeFishBScript::Variable& value() {
         return _stack[0];
      }

      virtual void onvalue(BeeFishJSON::JSON* json) {
//         cerr << "onvalue" << endl;
         _stack.push_back(
            createVariable(json)
         );
         
         BeeFishJSON::JSONParser::onvalue(json);
      }

      virtual void onbeginobject(Match* match) {
//         cerr << "onbeginobject" << endl;
         _stack.push_back(
            BeeFishBScript::Object()
         );
         BeeFishJSON::JSONParser::onbeginobject(match);
      }

      virtual void onobjectvalue(const BString& key, const BeeFishJSON::JSON* value) {

//         cerr << "onobjectvalue" << endl;
         
         std::shared_ptr<BeeFishBScript::Object> object = _stack[_stack.size() - 2];
         (*object)[key] = _stack[_stack.size() - 1];

         if (_stack.size() > 1)
            _stack.pop_back();


         BeeFishJSON::JSONParser::onobjectvalue(key, value);
      }

      virtual void onendobject(Match* match) {
//         cerr << "onendobject" << endl;
         if (_stack.size() > 1)
           _stack.pop_back();
         BeeFishJSON::JSONParser::onendobject(match);
      }  


      virtual void onbeginarray(Match* match) {
//         cerr << "onbeginarray" << endl;
         _stack.push_back(
            BeeFishBScript::Array()
         );
         JSONParser::onbeginarray(match);
      }

      virtual void onarrayvalue(Match* match) {
//         cerr << "onarrayvalue" << endl;
         std::shared_ptr<Array> array = _stack[_stack.size() - 2];;
         array->push_back(_stack[_stack.size() - 1]);
         if (_stack.size() > 1)
            _stack.pop_back();

         JSONParser::onarrayvalue(match);
      }

      virtual void onendarray(Match* match) {
//         cerr << "onendarray" << endl;
         if (_stack.size() > 1)
           _stack.pop_back();
         JSONParser::onendarray(match);
      }

      Variable createVariable(const BeeFishJSON::JSON* value) {

         switch (value->type()) {
         case BeeFishJSON::Type::UNDEFINED:
            return Variable::Undefined();
         case BeeFishJSON::Type::__NULL:
            return nullptr;
         case BeeFishJSON::Type::BOOLEAN:
            if (value->value() == "true")
               return true;
            else if (value->value() == "false")
               return false;
            else
               throw std::runtime_error("Invalid boolean value");
         case BeeFishJSON::Type::NUMBER:
            {
               double number =  atof(value->value().c_str());
               return number;
            }
         case BeeFishJSON::Type::STRING:
            return value->value();
         case BeeFishJSON::Type::ARRAY:
            return BeeFishBScript::Array();

         case BeeFishJSON::Type::OBJECT:
            return BeeFishBScript::Object();
         default:
            throw std::logic_error("Invalid object type");
         }

      }

   };

//   inline void BeeFishBScript::BScriptParser::Object::matchedSetItem(_KeyValue* item);
}

#endif