#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"


using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern Match JSON;

   class _Object:
      public Match,
      public map<wstring, Match>
   {
   public:
    
      Match createMatch()
      {
         return Set(
            Character('{'),
            Field(this),
            Character(','),
            Character('}')
         );
      }
      
      _Object() : Match(
         createMatch()
      )
      {
      }
      
      _Object(const _Object& source) :
         Match(
            createMatch()
         ),
         map<wstring, Match>(source)
      {
      }

      virtual ~_Object()
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
      
   public:
      class Field : public Match
      {
      public:
         _Object* _object;
         _String _name;
         LoadOnDemand _value;
         Match _match;
         
         Match createMatch()
         {
            return
               _name and
               ~BlankSpace and
               Character(':') and
               ~BlankSpace and
               _value;
         }
         
      public:
         Field(_Object* object) :
            Match(),
            _object(object),
            _name(),
            _value(JSON)
         {
            _match = createMatch();
         }
         
         Field(const Field& source) :
            Match(),
            _object(source._object),
            _name(source._name),
            _value(source._value)
         {
            _match = createMatch();
         }
         
         virtual bool match(int character)
         {
            bool matched =
               _match.match(character);
               
            if (_match.result() == true)
               success();
            else if (_match.result() == false)
            {
               matched = false;
               fail();
            }
            
            return matched;
         }
         
         ~Field()
         {
         }
         
         virtual void success()
         {
            cerr << _value.item() << endl;
            
            _object->emplace(_name.value(), _value.item());
         }
         
         virtual Match* copy() const
         {
            return new Field(_object);
         }
         
      };
   };

   const Match Object = _Object();
 
}

#endif