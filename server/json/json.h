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
         
      Word*    _null;
      Word*    _true;
      Word*    _false;
      Or*      _boolean;
      _Number* _number;
      Match*   _array;
      _String* _string;
      _Object* _object;
      Or*      _items;
      And*     _paddedItem;
      
   public:
      _JSON() : Match()
      {
      }
      
      _JSON(const _JSON& source) : Match()
      {
         _capture = source._capture;
      }
      
      virtual void setup()
      {
         _null    = new Word("null");
      
         _true    = new Word("true");
      
         _false   = new Word("false");
      
         _boolean = new Or(_true, _false);

         _number  = new _Number();
      
         _array   = Array.copy();
      
         _string  = new _String();
      
         _object  = new _Object();

     
         _items = new Or(
            _null,
            _boolean,
            _number,
            _array,
            _string,
            _object
         );
         
         _paddedItem = new And(
            new Optional(BlankSpace.copy()),
            _items
         );
         
         _null->_capture    = _capture;
         _boolean->_capture = _capture;
         _number->_capture  = _capture;
         _array->_capture   = _capture;
         _string->_capture  = _capture;
         _object->_capture  = _capture;
         
         _match = _paddedItem;
         
         Match::setup();
         
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
         if (_items->matched())
            return _items->_item;
         else
            throw runtime_error("No JSON item matched");
      }
      
      bool isNull() const
      {
         return _null->matched();
      }
      
      virtual const BString& value() const
      {
         return item()->value();
      }
      /*
      virtual Match* getMatch()
      {
         return this;
      }
      */
      
   };
   
   inline Label JSON = Label("JSON", new _JSON());
}

#endif