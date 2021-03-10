#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "boolean.h"
#include "number.h"
#include "set.h"
#include "array.h"
#include "string.h"
#include "object.h"


using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   class _JSON : public Match
   {
   public:
      _JSON() : Match()
      {
      }
      
   public:
         
      MatchPtr _null = new Word("null");

      MatchPtr _true = new Word("true");
      
      MatchPtr _false = new Word("false");
      
      MatchPtr _boolean = _true or _false;
        
      MatchPtr _number = Number;
           
      MatchPtr _string = String;
            
      MatchPtr _array = Array;
           
      MatchPtr _object = Object;
      
      MatchPtr _item =
         _null or
         _boolean or
         _number or
         _string or
         _array or
         _object;
           
      _JSON(const _JSON& source)
      {
      }
      
      virtual void setup()
      {
         setMatch(
            ~BlankSpace and
            _item and
            ~BlankSpace
         );
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_JSON>(*this);
      }
      
      virtual void write(ostream& out) const
      {
         if (matched())
         {
            out << *item();
         }
         else
         {
            out << "JSON";
            writeResult(out);
            out << "(" << *_match << ")";
         }
     
      }
      
      MatchPtr item() const
      {
         if (_null->matched())
            return _null;
         else if (_true->matched())
            return _true;
         else if (_false->matched())
            return _false;
         else if (_number->matched())
            return _number;
         else if (_array->matched())
            return _array;
         else if (_string->matched())
            return _string;
         else if (_object->matched())
            return _object;
         else
            throw runtime_error("No JSON item matched");
      }
      
      virtual BString value() const
      {
         return item()->value();
      }
      
   };
   
   const MatchPtr JSON = new Label("JSON", _JSON());
}

#endif