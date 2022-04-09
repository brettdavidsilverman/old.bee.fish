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
   protected:

   public:

      public:
      BScriptParser(Match& match) :
         JSONParser(match)
      {
      }
      
      virtual ~BScriptParser()
      {
      }
      
      virtual bool isBScriptParser() {
         return true;
      }


   };


}

#endif