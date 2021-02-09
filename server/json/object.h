#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern Match JSON;
   /*
   const Match MatchField =   Capture(
               String,
               [](Capture& capture)
               {
               }
            ) and
                  
            ~BlankSpace and
            Character(':') and
            ~BlankSpace and
                  
            Capture(
               LoadOnDemand(JSON),
               [](Capture& capture)
               {
               }
            );
            */
   /*
   class _Object:
      public Match,
      public map<string, string>
   {
   public:
   */
   class Field;
   
    string value;
    
      class Field : public Match
      {
      public:
         string _name;
         string _value;
         
 
         
      public:
         Field() : Match(
            Capture(
               String,
               _name
            ) and
                  
            ~BlankSpace and
            Character(':') and
            ~BlankSpace and
                  
            Capture(
               LoadOnDemand(JSON),
               [](Capture& capture)
               {
               }
            )
         )
         {
            cerr << "Field()" << this << endl;
         }
         
         Field(const Field& source) :
             Field()
         {
            cerr << "Field(source)" << this << endl;
            _name = source._name;
            _value = source._value;
         }
         
         ~Field()
         {
            cerr << "~Field()" << endl;
         }
         
         virtual Match* copy() const
         {
            cerr << "Field::copy()";
            Field* copy = new Field(*this);
            return copy;
         }
      };
         /*
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
         cerr << "_Object()";
      }

      virtual ~_Object()
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object();
      }
      
   };
   
         Match createMatch(Field* field)
         {
            Match MatchField =
               Capture(
                  String,
                  [field](Capture& capture)
                  {
                     cerr << field;
                     //field->_name =
                     //   capture.value();
                  }
               ) and
                  
               ~BlankSpace and
               Character(':') and
               ~BlankSpace and
                  
               Capture(
                  LoadOnDemand(JSON),
                  [](Capture& capture)
                  {
                  
                     //field->_value =
                     //   capture.value();
                  }
               );
               
            return MatchField;
           
         }
         
   */
   const Match Object = Label(
      "Object",
      Set(
         Character('{'),
         Field(),
         Character(','),
         Character('}')
      )
   );
      
  // const Match Object = Label("Object", _Object());
 
}

#endif