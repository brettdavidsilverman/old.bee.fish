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
      
      bool contains(const BString& name)
      {
         return (count(name) > 0);
      }
      
   public:
   
      class Field : public Match
      {
      public:
         _Object* _object;
         _String* _name;
         LoadOnDemand* _fieldValue;
         
         virtual void setup()
         {
            _capture = _object->_capture;
            
            _name = new _String();
            _name->_capture = _capture;
            
            _fieldValue = new LoadOnDemand(JSON);
            _fieldValue->_capture = _capture;

            _match = new And(
               _name,
               new Optional(BlankSpace.copy()),
               new Character(':'),
               new Optional(BlankSpace.copy()),
               _fieldValue
            );
            
            Match::setup();
            
         }
         
      public:
         Field(_Object* object) :
            _object(object)
         {
         }
         
         Field(const Field& source) :
            _object(source._object)
         {
         }
         
         virtual void success()
         {
         
            
            if (_object->_capture)
            {
               Label* jsonLabel = 
                  static_cast<Label*>
                  (_fieldValue->_match);
                  
               Match* jsonMatch =
                  jsonLabel->_match;
                  
               _JSON* json = (_JSON*)jsonMatch;
                  
               _object->emplace(
                  _name->value(),
                  json
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