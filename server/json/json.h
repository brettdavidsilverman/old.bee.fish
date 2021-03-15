#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
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
         
      Match* _null    = new Word("null");

      Match* _true    = new Word("true");
      
      Match* _false   = new Word("false");
      
      Match* _boolean = new Or(_true, _false);
    
      Match* _number  = Number.copy();
      
      Match* _array   = Array.copy();
      
      Match* _string  = String.copy();

      Match* _object  = new _Object();

      Or* _item = new Or(
         _null,
         _boolean,
         _number,
         _array,
         _string,
         _object
      );
         
      And* _paddedItem = new And(
         new Optional(BlankSpace.copy()),
         _item,
         new Optional(BlankSpace.copy())
      );
      
   public:
      _JSON() : Match()
      {
         _match = _paddedItem;
      }
      
      _JSON(const _JSON& source) : Match()
      {
         _match = _paddedItem;
      }
      
      virtual Match* copy() const
      {
         return new _JSON(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         if (matched())
         {
            out << tabs(tabIndex) << *item();
         }
         else
         {
            out << tabs(tabIndex) << "JSON";
            writeResult(out);
            out << "(" << *_match << ")";
         }
     
      }
      
      Match* item() const
      {
         if (_item->matched())
            return _item->_item;
         else
            throw runtime_error("No JSON item matched");
      }
      
      virtual BString value() const
      {
         return item()->value();
      }
      
      virtual Match* getMatch()
      {
         return this;
      }
      
      
   };
   
   inline Label JSON = Label("JSON", new _JSON());
}

#endif