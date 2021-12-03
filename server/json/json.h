#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "null.h"
#include "boolean.h"
#include "number.h"
#include "set.h"
#include "array.h"
#include "string.h"
#include "object.h"

#include "../power-encoding/power-encoding.h"
#include "../database/path.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON
{
   
   class JSON : public And

   {
   protected:
      Object::OnKeys _onobjectkeys;
      Object::OnValues _onobjectvalues;

   public:
      Null*      _null;
      Boolean*  _boolean;
      Number*   _number;
      Array*     _array;
      String*   _string;
      Object*   _object;
      Or*        _items;
   public:
      JSON() : And()
      {
      }
      
      JSON(Object::OnKeys& onobjectkeys, Object::OnValues& onobjectvalues ) : 
         _onobjectkeys(onobjectkeys), 
         _onobjectvalues(onobjectvalues) 
      {
      }

      virtual ~JSON()
      {

      }
      
      virtual void setup()
      {
         _null    = new Null();
      
         _boolean = new Boolean();
         _number  = new Number();
      
         _array   = new Array();
         _string  = new String();
         _object  = new Object(_onobjectkeys, _onobjectvalues);

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
        
         
         And::setup();
         
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

      Object::OnKeys& onobjectkeys() {
         return _onobjectkeys;
      }
      
      Object::OnValues& onobjectvalues() {
         return _onobjectvalues;
      }

      void captureObjectValue(const BString& key, BeeFishMisc::optional<BString>& value) {
         _onobjectvalues[key] = 
            [&value](const BString& key, JSON& json) {
               value = json.value();
            };
      }

      void streamObjectValue(const BString& key, BStringStream::OnBuffer onbuffer) {
         _onobjectkeys[key] = 
            [onbuffer](const BString& key, JSON& json) {
               json._string->_onbuffer = onbuffer;
            };
      }
      
   };

   // Declared in object.h
   inline void Object::captureField(const BString& key, BeeFishMisc::optional<BString>& value) {
      _onvalues[key] = 
         [&value] (const BString& key, JSON& json) {
            value = json.value();
         };
   }
   
   inline void Object::streamField(const BString& key, BeeFishBString::BStringStream::OnBuffer onbuffer) {
      Object::OnKey onkey =
         [onbuffer] (const BString& key, JSON& json) {
            json._string->_onbuffer = onbuffer;
         };

      _onkeys[key] = onkey; 
   }

   
/*   
   // Declared in object.h
   inline void _Object::Field::setup()
   {
      _key = new _String();
            
      _fieldValue = new _JSON();

      _match = new And(
         new Invoke(
            _key,
            [this](Match* match)
            {
               this->writeKey();
            }
         ),
         new Optional(BlankSpace.copy()),
         new BeeFishParser::Character(':'),
         new Optional(BlankSpace.copy()),
         new Invoke(
            _fieldValue,
            [this](Match* match)
            {
               this->writeValue();
            }
         )
      );
      
      _setup = true;
   }
 
   // Declared in object.h
   inline void _Object::Field::writeKey()
   {

   }
         
   // Declared in object.h
   inline void _Object::Field::writeValue()
   {
   }
   // Declared in array.h
   inline void _Array::setup()
   {
      _openBrace =
         new BeeFishParser::Character('[');
      _item = new _JSON();
      _seperator =
         new BeeFishParser::Character(',');
      _closeBrace =
         new BeeFishParser::Character(']');
      _capture = false;
         
      Set::setup();
   }
  */
}

#endif