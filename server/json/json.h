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
         
      Word*      _null;
      _Boolean*  _boolean;
      _Number*   _number;
      _Array*     _array;
      _String*   _string;
      _Object*   _object;
      Or*        _items;
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
      
         _boolean = new _Boolean();

         _number  = new _Number();
      
         _array   = new _Array();
      
         _string  = new _String();
      
         _object  = new _Object();

     
         _null->_capture = _capture;
         _boolean->_capture = _capture;
         _number->_capture = _capture;
         _array->_capture = _capture;
         _string->_capture = _capture;
         _object->_capture = _capture;
         /*
         _items = new Or(
            new Label("null", _null),
            new Label("boolean",_boolean),
            new Label("number", _number),
            new Label("array",  _array),
            new Label("string", _string),
            new Label("object", _object)
         );
          */
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
        
         _match = _paddedItem;
         
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
         if (_items->matched())
            return _items->item();
         else
            throw runtime_error("No JSON item matched");
      }
      
      bool isNull() const
      {
         return _null->matched();
      }
      
      virtual const BString& value() const
      {
         return item().value();
      }
      
   };
   
   inline const Label JSON = Label("JSON", new _JSON());
   

   // Declared in object.h
   inline void _Object::write(
         ostream& out,
         size_t tabIndex
      ) const
   {
      //_match->write(out, tabIndex);
      // return;
      
      cerr << "_Object";
      writeResult(out);
      cerr << endl
           << tabs(tabIndex)
           << "("
           << endl
           << tabs(tabIndex + 1)
           << "{"
           << endl;
              
      for (auto it  = cbegin();
                it != cend();
                )
      {
         auto pair = *it;
         cerr << tabs(tabIndex + 2)
              << "{"
              << endl
              << tabs(tabIndex + 3)
              << "\"";
         pair.first.writeEscaped(out);
         cerr << "\","
              << endl;
         pair.second->_match->write(out, tabIndex + 3);
         cerr << endl
              << tabs(tabIndex + 2)
              << "}";
         ++it;
         if (it != cend())
            cerr << ",";
         cerr << endl;
      }
         
      cerr << tabs(tabIndex + 1)
           << "}"
           << endl
           << tabs(tabIndex)
           << ")";
   }
   
   // Declared in object.h
   inline void _Object::Field::setup()
   {

      _capture = _object->_capture;
      _capture = true;
      _key = new _String();
      _key->_capture = true;
            
      _fieldValue = new _JSON();
      _fieldValue->_capture = true;

      _match = new And(
         _key,
         new Optional(BlankSpace.copy()),
         new bee::fish::parser::Character(':'),
         new Optional(BlankSpace.copy()),
         _fieldValue
      );
      _match->_capture = true;
      
      _setup = true;
   }

   // Declared in object.h
   inline void _Object::Field::success()
   {
         
      if (_object->_capture)
      {
         BString key = _key->value();
         
         // Add to map
         _object->emplace(key, _fieldValue);
         
      }
            
            
      Match::success();
   }
   
   inline void _Object::Field::write(
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
   
}

#endif