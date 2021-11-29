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

      const JSON* fieldValue() const {
         return (JSON*)(_fieldValue->_match);
      }

   };

   class ObjectFieldSeperator: public BeeFishParser::Character {
   public:
      ObjectFieldSeperator() : Character(',') {

      }
   };

   typedef std::function<void(const BString& key, const JSON*)> ObjectFunction;

   class Object:
      public Set<ObjectOpenBrace, ObjectKeyValue, ObjectFieldSeperator, ObjectCloseBrace>,
      public map<BString, ObjectFunction>
   {
   protected:

   public:

      Object() : Set()
      {
      }

      Object(const map<BString, ObjectFunction>& functions ) : 
         map<BString, ObjectFunction>(functions) {

      }
      
      virtual void matchedSetItem(ObjectKeyValue* field)
      {
         const BString& key = field->key();
         const JSON* json = field->fieldValue();
         ObjectFunction matchedInvoke;

         if (matchedInvoke = (*this)[key])
            matchedInvoke(key, json);

         matchedField(key, json);

         Set::matchedSetItem(field);
      }
      
      bool contains(const BString& name) const
      {
         return (find(name) != end());
      }

      virtual void matchedField(const BString& key, const JSON* value) {

      }
      
      
   };

}

#endif