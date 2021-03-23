#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"


using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern Label JSON;

   class _JSON;
   
   class _Object:
      public Match,
      public map<BString, _JSON* >
   {
   public:
    
      _Object()
      {
      }
      
      _Object(const _Object& source)
      {
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
            
         Match::setup();
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         cerr << "_Object";
         writeResult(out);
         cerr << "()";
      }
      
      bool contains(BString name)
      {
         return (count(name) > 0);
      }
      
   public:
   
      class Field : public Match
      {
      public:
         _Object* _object;
         _String* _name;
         Match* _value;
         
         virtual void setup()
         {
            _capture = _object->_capture;
            
            _name = new _String();
            _name->_capture = _capture;
            
            _value = new LoadOnDemand(JSON);
            _value->_capture = _capture;
               
            _match = new And(
               _name,
               new Optional(BlankSpace.copy()),
               new Character(':'),
               new Optional(BlankSpace.copy()),
               _value
            );
            
            Match::setup();
            
         }
         
      public:
         Field(_Object* object) :
            _object(object)
         {
            _capture = object->_capture;
         }
         
         Field(const Field& source) :
            _object(source._object)
         {
            _capture =
               source._object->_capture;
         }
         
         virtual void success()
         {
            if (_capture)
            {
               _JSON* value =
                  (_JSON* )
                  ( _value->_match );
                  
               _object->emplace(
                  _name->value(),
                  value
               );
            }
            
            
            Match::success();
         }
         
         virtual Match* copy() const
         {
            return new Field(*this);
         }
         
      };
      
   };

   const Label Object = Label("Object", new _Object());
 
}

#endif