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

   class ObjectKeyValue : public And {
   protected:
      String* _key;
      LoadOnDemand<JSON>* _fieldValue;
   public:
      ObjectKeyValue() : And (
         _key = new String(),
         new Optional(new BlankSpace()),
         new BeeFishParser::Character(':'),
         new Optional(new BlankSpace()),
         _fieldValue = new LoadOnDemand<JSON>()
      ) {

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
      public Set<ObjectOpenBrace, ObjectKeyValue, ObjectFieldSeperator, ObjectCloseBrace>
   {
   public:
      typedef std::function<void(const BString& key, JSON& json)> Function;
      typedef std::map<const BString, Function> Functions;
      Functions _functions;

   public:

      Object() : Set()
      {
      }

      Object(const Functions& functions ) : Set(), 
         _functions(functions) {

      }
      
      virtual void matchedSetItem(ObjectKeyValue* field)
      {
         const BString& key = field->key();
         JSON* json = field->fieldValue();
         Function matchedInvoke;

         if (matchedInvoke = _functions[key])
            matchedInvoke(key, *json);

         matchedField(key, json);

         Set::matchedSetItem(field);
      }
      
      virtual void matchedField(const BString& key, const JSON* value) {

      }
      
      Object::Functions& functions() {
         return _functions;
      }

      // Defined in JSON.h
      void captureField(const BString& key, BeeFishMisc::optional<BString>& value);
   };

}

#endif