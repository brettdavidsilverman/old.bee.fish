#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "boolean.h"
#include "number.h"
#include "set.h"
#include "array.h"
/*
#include "string.h"
#include "object.h"
*/

using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   class _JSON : public Match
   {
   public:
      _JSON() : Match()
      {
         setMatch(
            ~BlankSpace and
            Item and
            ~BlankSpace
         );
         
      }
      
      
      public:
         

         MatchPtr Null = new Word("null");

         MatchPtr True = new Word("true");
      
         MatchPtr False = new Word("false");
      
         MatchPtr Boolean = True or False;
        
         MatchPtr Number = new
            bee::fish::json::Number();
           
         MatchPtr Array = 
            bee::fish::json::Array;
           
         MatchPtr Item =
            Null or
            Boolean or
            Number or
            Array;
           
      _JSON(const _JSON& source) : Match(source)
      {
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_JSON>(*this);
      }
      
      virtual void write(ostream& out) const
      {
         if (matched())
         {
            if (Null->matched())
               out << "null";
            else if (True->matched())
               out << "true";
            else if (False->matched())
               out << "false";
            else if (Number->matched())
               out << *Number;
            else if (Array->matched())
               out << *Array;
            else
               out << *(Item->item());
         }
         else
         {
            out << "JSON";
            writeResult(out);
            out << "(" << *_match << ")";
         }
     
      }
   };
   
   MatchPtr JSON = new Label("JSON", _JSON());
}

#endif