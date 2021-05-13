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
      public Match,
      public map<BString, Label* >
   {
   public:
      _Object()
      {
        
      }
      
      _Object(const _Object& source)
      {
         _capture = source._capture;
      }
      
      virtual void setup()
      {
         _match = new
            Set(
               new Character('{'),
               new Field(this),
               new Character(','),
               new Character('}')
            );
            
         _match->_capture = _capture;
         _setup = true;
        
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
         Label* _fieldValue = nullptr;
         
      public:
         Field(_Object* object) :
            _object(object)
         {
         }
         
         Field(const Field& source) :
            _object(source._object)
         {
            _capture = source._capture;
         }
         
         // implemnted in json.h
         virtual void setup();
         
         // implemnted in json.h
         virtual void success();
         
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