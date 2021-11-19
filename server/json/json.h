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
#include "../power-encoding/power-encoding.h"
#include "../database/path.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON
{
   
   class _JSON : public Match
   {
   public:
         
      Word*      _null;
      _Boolean*  _boolean;
      _Number*   _number;
      Capture*     _array;
      _String*   _string;
      _Object*   _object;
      Or*        _items;
      
   public:
      _JSON() :
         Match()
      {
      }
      
      _JSON(const _JSON& source) :
         Match()
      {
      }
      
      virtual ~_JSON()
      {

      }
      
      virtual void setup()
      {
         _null    = new Word("null");
      
         _boolean = new _Boolean();

         _number  = new _Number();
      
         _array   = new Capture(
            new _Array()
         );
      
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
         
         _match = new And(
            new Optional(BlankSpace.copy()),
            _items
         );
         
         _setup = true;
         
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
            _items->item().write(out, tabIndex);
         }
         else
         {
            out << tabs(tabIndex) << "JSON";
            writeResult(out);
            out << endl
                << tabs(tabIndex)
                << "("
                << endl;
            if (_match == nullptr)
               out << tabs(tabIndex + 1)
                   << "NULL";
            else
               _items->write(out, tabIndex + 1);
            out << endl
                << tabs(tabIndex)
                << ")";
         }
     
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
      
      virtual const std::vector<Char>& value() const
      {
         return item().value();
      }
      
   };
   
   const Label JSON = Label("JSON", new _JSON());
   

   // Declared in object.h
   void _Object::write(
         ostream& out,
         size_t tabIndex
      ) const
   {
      //_match->write(out, tabIndex);
      // return;
      
      out << "_Object";
      writeResult(out);
      out << endl
           << tabs(tabIndex + 1)
           << "{"
           << endl;
              
      for (auto it  = cbegin();
                it != cend();
                )
      {
         auto pair = *it;
         out  << tabs(tabIndex + 3)
              << "\"";
         pair.first.writeEscaped(out);
         out << "\": ";
         _JSON* value = pair.second;
         value->write(out, tabIndex + 3);
         ++it;
         if (it != cend())
            out << ",";
         out << endl;
      }
         
      out << tabs(tabIndex + 1)
          << "}";
   }
   
   // Declared in object.h
   void _Object::Field::setup()
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
   void _Object::Field::writeKey()
   {

   }
         
   // Declared in object.h
   void _Object::Field::writeValue()
   {
   }
         
   // Declared in object.h
   void _Object::Field::write(
      ostream& out,
      size_t tabIndex
   ) const
   {
      out << tabs(tabIndex)
          << "Object::Field";
      writeResult(out);
      out << endl
          << tabs(tabIndex)
          << "("
          << endl;
          
      if (_key)
         _key->write(out, tabIndex + 1);
      else
         out << tabs(tabIndex + 1)
             << "NULL";
             
      out << ","
          << endl;
          
      if (_fieldValue)
         _fieldValue->write(out, tabIndex + 1);
      else
         out << tabs(tabIndex + 1)
             << "NULL";
             
      out << endl
          << tabs(tabIndex)
          << ")";
   }
   
   
   // Declared in array.h
   void _Array::setup()
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
}

#endif