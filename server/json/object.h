#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"


using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Label JSON;

   class _JSON;
   class _Object; 
   
   class _Object:
      public Set,
      public map<BString, _JSON* >
   {
   protected:
      vector<Match*> _fields;
      
   public:
      _Object() :
         Set(
            true,
            new bee::fish::parser::
               Character('{'),
            new Field(this),
            new bee::fish::parser::
               Character(','),
            new bee::fish::parser::
               Character('}')
         )
      {
      }
      
      _Object(const _Object& source) :
         Set(source)
      {
      }
      
      virtual void matchedSetItem(Match* item)
      {
         Field* field = (Field*)item;
         emplace(field->_key->value(), field->_fieldValue);
         _fields.push_back(field);
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
      
      // Implememted in json.h
      virtual ~_Object();
      
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
         Capture* _key = nullptr;
         _JSON* _fieldValue = nullptr;
         
      public:
      
         Field(_Object* object) :
            _object(object)
         {
         }
         
         // Implemented in json.h
         Field(const Field& source) :
            Match(source),
            _object(source._object)
         {
         }
         
         // Implemented in json.h
         virtual void setup();

         virtual Match* copy() const
         {
            Field* field = new Field(*this);
            
            return field;
         }
         
         // implemented in json.h
         virtual void write(
            ostream& out,
            size_t tabIndex = 0
         ) const;
 
         
      };
   
   };

   const Label Object = Label("Object", new _Object());
 
}

#endif