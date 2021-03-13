#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"


using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const MatchPtr JSON;

   class _JSON;
   
   class _Object:
      public Match,
      public map<BString, shared_ptr<_JSON> >
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
         MatchPtr match = 
            Set(
               Character('{'),
               Field(this),
               Character(','),
               Character('}')
            );
            
         setMatch(match);
      }
      
      virtual bool match(Char character)
      {
         return Match::match(character);
      }
      
      virtual void success()
      {
         Match::success();
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_Object>(*this);
      }
      
      virtual void write(ostream& out) const
      {
         cerr << "_Object";
         writeResult(out);
         cerr << "()";
      }
      
   public:
   
      class Field : public Match
      {
      public:
         _Object* _object;
         MatchPtr _name;
         MatchPtr _value;
         
         virtual void setup()
         {
            _name = new _String();
            _name->_capture = this->_capture;
            
            _value = new LoadOnDemand(JSON);
            _value->_capture = this->_capture;
            
            MatchPtr match =
               _name and
               ~BlankSpace and
               Character(':') and
               ~BlankSpace and
               _value;
               
            setMatch(match);
            
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
            if (_capture)
            {
               shared_ptr<_JSON> value =
                  static_pointer_cast<_JSON>
                  ( _value->match() );
               
               _object->emplace(
                  _name->value(),
                  value
               );
            }
            
            
            Match::success();
         }
         
         virtual MatchPtrBase copy() const
         {
            return make_shared<Field>(*this);
         }
         
      };
      
   };

   const Label Object = Label("Object", _Object());
 
}

#endif