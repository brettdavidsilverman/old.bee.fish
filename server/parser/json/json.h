#ifndef BEE_FISH_PARSER__JSON_H
#define BEE_FISH_PARSER__JSON_H

#include <map>
namespace bee::fish::parser::json
{
   class JSON;
   class Object;
   class Number;
   class String;
   class Array;
   class BlankSpace;
}

#include "../parser.h"
#include "blank-space.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"

using namespace bee::fish::parser;

namespace bee::fish::parser::json
{
   
   class JSON : public And
   {
   protected:
      Match* _parent;
   public:
      JSON(Match* parent = NULL);
            
      class True : public Word
      {
      public:
         True() : Word("true")
         {
         }
         
         virtual void write(ostream& out)
         {
            out << "true";
         }
      };
      
      class False : public Word
      {
      public:
         False(): Word("false")
         {
         }
         
         virtual void write(ostream& out)
         {
            out << "false";
         }

      };
      
      class Null : public Word
      {
      public:
         Null(): Word("null")
         {
         }
         
         virtual void write(ostream& out)
         {
            out << "null";
         }

      };
      
      virtual void write(ostream& out)
      {
         out << item();
      }
      
      Match& item() {
         Or* _or = (Or*)(inputs()[1]);
         
         return _or->item();
      }
      
      friend ostream& operator << 
      (ostream& out, JSON& json)
      {
         json.write(out);
         return out;
      }


   };
   
   
}

#endif