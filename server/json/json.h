#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "boolean.h"
#include "number.h"
#include "set.h"
/*
#include "array.h"
#include "string.h"
#include "object.h"

#include "set.h"
*/

using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   class JSON : public Match
   {
   public:
      JSON() : Match()
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
           
        MatchPtr Item =
           Null or
           Boolean or
           Number;
           
      JSON(const JSON& source) : Match(source)
      {
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<JSON>(*this);
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
            else
               out << *(Item->item());
         }
         else
            Match::write(out);
              
     
      }
   };
   
}

#endif