#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

#include "../power-encoding/power-encoding.h"
#include "../database/path.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"


using namespace bee::fish::parser;
using namespace bee::fish::database;
using namespace bee::fish::power_encoding;

namespace bee::fish::json {
   
   extern const Label JSON;

   class _JSON;
   class _Object; 
   
   class _Object:
      public Set,
      public map<BString, _JSON* >
   {
   protected:
      Path<PowerEncoding>* _path = nullptr;
      
   public:
      _Object(Path<PowerEncoding>* path = nullptr) :
         Set()
      {
         if (path)
            _path = new Path(*path);
      }
      
      _Object(const _Object& source) :
         Set(source)
      {
         if (source._path)
            _path = new Path(*(source._path));
      }
      
      virtual ~_Object()
      {
         if (_path)
            delete _path;
          
         _path = nullptr;
      }
      
      virtual void setup()
      {
         _openBrace = new bee::fish::parser::
               Character('{'),
         _item = new Field(this),
         _seperator = new bee::fish::parser::
               Character(','),
         _closeBrace = new bee::fish::parser::
               Character('}'),
         _capture = true;
         
         Set::setup();
      }
      
      virtual void matchedSetItem(Match* item)
      {
         Field* field = (Field*)item;
         emplace(field->_key->value(), field->_fieldValue);
         
         Set::matchedSetItem(item);
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
      
      // Implemented in json.h
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const;
      
      bool contains(const BString& name) const
      {
         return (find(name) != end());
      }
      
   public:
   
      
      class Field : public Match
      {
      public:
         _Object* _object;
         _String* _key = nullptr;
         _JSON* _fieldValue = nullptr;
         Path<PowerEncoding>* _path = nullptr;
         
      public:
      
         Field(_Object* object) :
            _object(object)
         {
            if (_object->_path)
               _path =
                  new Path(*(_object->_path));
         }
         
         // Implemented in json.h
         Field(const Field& source) :
            Match(source),
            _object(source._object)
         {
            if (source._path)
               _path = new Path(*(source._path));
         }
         
         virtual ~Field()
         {
            if (_path)
               delete _path;
         }
         
         // Implemented in json.h
         virtual void setup();

         virtual Match* copy() const
         {
            Field* field = new Field(*this);
            
            return field;
         }
         
         // Implemented in json.h
         virtual void write(
            ostream& out,
            size_t tabIndex = 0
         ) const;
 
         // Implemented in json.h
         void writeKey();
         
         // Implemented in json.h
         void writeValue();
         
      };
   
   };

   const Label Object = Label("Object", new _Object());
 
}

#endif