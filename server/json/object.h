#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

#include "../power-encoding/power-encoding.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON {
   
   class JSON;
   class Object; 
   
   class ObjectOpenBrace : public BeeFishParser::Character {
   public:
      ObjectOpenBrace() : Character('{') {

      }
   };

   class ObjectCloseBrace : public BeeFishParser::Character {
   public:
      ObjectCloseBrace() : Character('}') {

      }
   };


   class Key : public String {
   protected:
      KeyedItem* _set;
      LoadOnDemand<JSON>* _value;
   public:
      
      Key(KeyedItem* set, LoadOnDemand<JSON>* value) : String(),
         _set(set),
         _value(value)
      {
      }

      virtual void success() {
         String::success();
         if (_set) {
            _set->keyMatched(*this);
         }
      }

      LoadOnDemand<JSON>* keyedValue() {
         return _value;
      }

   };

   class Object;
   
   //
   //class Set;

   class ObjectKeyValue : public And {
   public:

      Key* _key;
      LoadOnDemand<JSON>* _fieldValue;
      KeyedItem* _object;

   public:
      ObjectKeyValue(KeyedItem* object = nullptr) : And (),
         _object(object) 
      {

      }

      virtual void setup() {
         _fieldValue = new LoadOnDemand<JSON>(),
         _key = new Key(_object, _fieldValue),
         _inputs = {
            _key,
            new Optional(new BlankSpace()),
            new BeeFishParser::Character(':'),
            new Optional(new BlankSpace()),
            _fieldValue
         };

         And::setup();

      }
      const BString& key() const {
         return _key->value();
      }

      JSON* fieldValue() {
         return (JSON*)(_fieldValue->_match);
      }

   };

   class ObjectFieldSeperator: public BeeFishParser::Character {
   public:
      ObjectFieldSeperator() : Character(',') {

      }
   };


   class Object:
      public KeyedSet<ObjectOpenBrace, ObjectKeyValue, ObjectFieldSeperator, ObjectCloseBrace>
   {


   public:

      Object() : KeyedSet()
      {
      }

      virtual ObjectKeyValue* createItem() {
         return new ObjectKeyValue(this);
      }

      // Defined in json-parser.h
      virtual void keyMatched(Key& key);
      // Defined in json-parser.h
      virtual void matchedSetItem(ObjectKeyValue* item);
      
   };

}

#endif