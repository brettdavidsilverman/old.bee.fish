#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "null.h"
#include "boolean.h"
#include "number.h"
#include "set.h"
#include "keyed-set.h"
#include "array.h"
#include "string.h"
#include "object.h"
#include "output.h"

#include "../power-encoding/power-encoding.h"


using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON
{
   
   template<
      class Null = Null,
      class Boolean = Boolean,
      class Number = Number,
      class Array = Array,
      class String = String,
      class Object = Object >
   class _JSON : public And

   {
   public:
      Null*      _null;
      Boolean*  _boolean;
      Number*   _number;
      Array*     _array;
      String*   _string;
      Object*   _object;
      Or*        _items;
   public:
      _JSON() : And()
      {
      }
      
      virtual ~_JSON()
      {

      }
      
      virtual void setup(Parser* parser)
      {
         _null    = new Null();
      
         _boolean = new Boolean();
         
         _number  = new Number();
      
         _array   = new Array();

         _string  = new String();

         _object  = new Object();

         _items = new Or{
            _null,
            _boolean,
            _number,
            _array,
            _string,
            _object
         };
         
         _inputs = {
            new Optional(new BlankSpace()),
            _items,
         };
        
         And::setup(parser);

         
         
      }
      
      
      virtual Match& item() const
      {
         if (!_items->matched())
            throw runtime_error("No JSON item matched");

         return _items->item();
      }
      
      bool isNull() const
      {
         return _null->matched();
      }
      
      virtual const BString& value() const
      {
         return item().value();
      }

      virtual bool matched() const {
         return _items->matched();
      }
   };

   class JSON : public _JSON<Null, Boolean, Number, Array, String, Object>
   {

   };


   
}

#endif