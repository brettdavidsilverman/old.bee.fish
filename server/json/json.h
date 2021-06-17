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

using namespace bee::fish::parser;
using namespace bee::fish::power_encoding;
using namespace bee::fish::database;

namespace bee::fish::json
{
   
   class _JSON : public Match
   {
   public:
         
      Word*      _null;
      _Boolean*  _boolean;
      Capture*   _number;
      _Array*     _array;
      _String*   _string;
      _Object*   _object;
      Or*        _items;
      Path<PowerEncoding>* _path = nullptr;
      
   public:
      _JSON(Path<PowerEncoding>* path = nullptr) :
         Match()
      {
         if (path)
            _path = new Path(*path);
      }
      
      _JSON(const _JSON& source) :
         Match()
      {
         if (source._path)
            _path = new Path(*(source._path));
      }
      
      virtual ~_JSON()
      {
         if (_path)
            delete _path;
      }
      
      virtual void setup()
      {
         _null    = new Word("null");
      
         _boolean = new _Boolean();

         _number  = new _Number();
      
         _array   = new _Array(_path);
      
         _string  = new _String();
      
         _object  = new _Object(_path);

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
      
      out << "_Object";
      writeResult(out);
      out << endl
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
         out << tabs(tabIndex + 2)
              << "{"
              << endl
              << tabs(tabIndex + 3)
              << "\"";
         pair.first.writeEscaped(out);
         out << "\","
              << endl;
         pair.second->_match->write(out, tabIndex + 3);
         out << endl
              << tabs(tabIndex + 2)
              << "}";
         ++it;
         if (it != cend())
            out << ",";
         out << endl;
      }
         
      out << tabs(tabIndex + 1)
           << "}"
           << endl
           << tabs(tabIndex)
           << ")";
   }
   
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
         new bee::fish::parser::Character(':'),
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
      if (_path)
      {
         Path keyPath =
            (*_path)[_key->value()];
         _fieldValue->_path =
            new Path(keyPath);
      }

   }
         
   // Declared in object.h
   inline void _Object::Field::writeValue()
   {
   }
         
   // Declared in object.h
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
   
   
   // Declared in array.h
   inline void _Array::setup()
   {
      _openBrace =
         new bee::fish::parser::Character('[');
      _item = new _JSON(_path);
      _seperator =
         new bee::fish::parser::Character(',');
      _closeBrace =
         new bee::fish::parser::Character(']');
      _capture = false;
         
      Set::setup();
   }
}

#endif