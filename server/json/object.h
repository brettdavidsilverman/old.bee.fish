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
      typedef std::function<void(const BString& key, JSON& json)> OnKey;
      typedef std::map<const BString, OnKey> OnKeys;
      typedef std::function<void(const BString& key, JSON& json)> OnValue;
      typedef std::map<const BString, OnValue> OnValues;
      OnKeys _onkeys;
      OnValues _onvalues;

   public:

      Object() : KeyedSet()
      {
      }

      Object(const OnKeys& onkeys, OnValues& onvalues ) : KeyedSet(), 
         _onkeys(onkeys),
         _onvalues(onvalues) 
      {

      }

      virtual void keyMatched(Key& key) {
         OnKey onkey = _onkeys[key.value()];
         if (onkey) {
            LoadOnDemand<JSON>* load = key.keyedValue();
            if (!load->_setup)
               load->setup();

            JSON* json = (JSON*)(load->_match);
               onkey(key.value(), *json);
         }

      } 

      virtual ObjectKeyValue* createItem() {
         return new ObjectKeyValue(this);
      }

      virtual void matchedSetItem(ObjectKeyValue* item) {
         const BString& key = item->_key->value();
         OnValue onvalue = _onvalues[key];
         if (onvalue) {
            
            if (!item->_fieldValue->_setup)
               item->_fieldValue->setup();

            JSON* json = (JSON*)(item->_fieldValue->_match);

            onvalue(key, *json);
         }

         //Set::matchedSetItem(item);

      }

      virtual void matchedField(const BString& key, const JSON* value) {
         throw 1;
      }
      
      Object::OnKeys& onkeys() {
         return _onkeys;
      }

      Object::OnValues& onvalues() {
         return _onvalues;
      }

      // Defined in JSON.h
      void captureField(const BString& key, BeeFishMisc::optional<BString>& value);
      void streamField(const BString& key, BeeFishBString::BStringStream::OnBuffer onbuffer);
   };

}

#endif