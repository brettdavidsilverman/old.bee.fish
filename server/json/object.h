#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

//#define CAPTURE_OBJECT
using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern Match JSON;

#ifdef CAPTURE_OBJECT
   class _Object:
      public Match,
      public map<string, string>
   {
   public:
   
      class Field : public Match
      {
      public:
         _Object* _parent;
         string _name;
         string _value;
         
 
         
      public:
         Field(_Object* parent) : Match(
            Capture(
               String,
               _name
            ) and
                  
            ~BlankSpace and
            Character(':') and
            ~BlankSpace and
                  
            Capture(
               LoadOnDemand(JSON),
               _value
            )
         )
         {
            _parent = parent;
         }
         
         Field(const Field& source) :
             Field(source._parent)
         {

            _name = source._name;
            _value = source._value;
         }
         
         ~Field()
         {
         }
         
         virtual void success()
         {
            Match::success();
            _parent->emplace(_name, _value);
         }
         
         virtual Match* copy() const
         {
            Field* copy = new Field(*this);
            return copy;
         }
      };
         
   public:
    
      _Object() : Match(
         Set(
            Character('{'),
            Field(this),
            Character(','),
            Character('}')
         )
      )
      {
      }

      virtual ~_Object()
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object();
      }
      
   };
#else
   class _Object:
      public Match
   {
   public:
   
      class Field : public Match
      {
 
      public:
         Field() : Match(
            String and
                  
            ~BlankSpace and
            Character(':') and
            ~BlankSpace and
                  
            LoadOnDemand(JSON)
         )
         {
         }
         
         Field(const Field& source) :
             Field()
         {

         }
         
         ~Field()
         {
         }
         
         virtual Match* copy() const
         {
            Field* copy = new Field(*this);
            return copy;
         }
      };
         
   public:
    
      _Object() : Match(
         Set(
            Character('{'),
            Field(),
            Character(','),
            Character('}')
         )
      )
      {
      }

      virtual ~_Object()
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object();
      }
      
   };
#endif

   const Match Object = Label("Object", _Object());
 
}

#endif