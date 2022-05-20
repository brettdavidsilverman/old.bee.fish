#ifndef BEE_FISH_B_SCRIPT__OBJECT_H
#define BEE_FISH_B_SCRIPT__OBJECT_H

#include <map>
#include <vector>
#include <memory>

#include "../b-string/string.h"
#include "../misc/constants.h"
#include "../json/json.h"

namespace BeeFishBScript {
   
   using namespace BeeFishBString;
   using namespace std;

   class Object;
   class Variable;

   typedef nullptr_t Null;
   typedef bool Boolean; 
   typedef double Number;
   typedef BString String;
   typedef vector<Variable> Array;
   typedef BStream Stream;

   typedef std::map<BString, Variable> Map;
   typedef std::vector<BString> Table;
   typedef std::initializer_list<Map::value_type> List;
   typedef std::initializer_list<Array::value_type> ArrayList;

   typedef std::shared_ptr<BeeFishBScript::Object> ObjectPointer;

   #define undefined BeeFishBScript::Variable::Undefined()

   class Object : public Map {
   protected:
      Table _table;
      typedef Table::const_iterator const_iterator;
   public:

      Object() {

      }

      Object(List list)
      {
         loadMap(list);
      }

      Object(const Object& source) : Map(source), _table(source._table) {
      }

      virtual ~Object() {

      }

      virtual void clear() {
         Map::clear();
         _table.clear();
      }

      virtual void apply(const ObjectPointer value);

      virtual pair<Object::iterator,bool> insert(const Object::value_type& pair);

      Variable& operator[] (const BString& key) {

         if (count(key) == 0)
            _table.push_back(key);

         return Map::operator[](key);
         
      }

      const Variable& operator[] (const BString& key) const;
      
      void loadMap(List list);

      virtual void write(ostream& out, size_t tabs = 0) const;

      friend ostream& operator << (ostream& out, const Object& object) {
         object.write(out);
         return out;
      }

      virtual std::string str() const {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      virtual BString bstr() const {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      const_iterator cbegin() const {
         return _table.cbegin();
      }

      const_iterator cend() const {
         return _table.cend();
      }

      bool contains(const BString& key) {
         return (this->count(key) > 0);
      }

   };
   

   struct Variable {
   public:
      BeeFishJSON::Type _type;

      union Value {
         Boolean _boolean;
         Number _number;
         String _string;
         std::shared_ptr<Array> _array;
         ObjectPointer _object;

         Value() {
         }

         Value(BeeFishJSON::Type type, const Value& source) {
            switch (type) {
            case BeeFishJSON::Type::UNDEFINED:
            case BeeFishJSON::Type::__NULL:
               break;
            case BeeFishJSON::Type::BOOLEAN:
               _boolean = source._boolean;
               break;
            case BeeFishJSON::Type::NUMBER:
               _number = source._number;
               break;
            case BeeFishJSON::Type::STRING:
               new (&_string) BString(source._string);
               break;
            case BeeFishJSON::Type::ARRAY:
               new (&_array) std::shared_ptr<Array>(source._array);
               break;
            case BeeFishJSON::Type::OBJECT:
               new (&_object) std::shared_ptr<Object>(source._object);
               break;
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
            }
         }

         ~Value() {
         }

      } _value;

      Variable() :
         _type(BeeFishJSON::Type::UNDEFINED)
      {

      } 

      Variable(const Variable& source) : 
         _type(source._type),
         _value(source._type, source._value)
      {
      }

      Variable(const Null& _nullptr) {
         _type = BeeFishJSON::Type::__NULL;
      }

      Variable(const Boolean& boolean) {
         _type = BeeFishJSON::Type::BOOLEAN;
         _value._boolean = boolean;
      }

      Variable(const Number& number) {
         _type = BeeFishJSON::Type::NUMBER;
         _value._number = number;
      }

      Variable(const int& number) : Variable(Number(number)) {
      }

      Variable(const unsigned int& number) : Variable(Number(number)) {
      }

      Variable(const void *& pointer) : Variable(Number((unsigned long)pointer)) {
      }

      Variable(const String& value) {
         _type = BeeFishJSON::Type::STRING;
         new (&_value._string) String(value);
      }

      Variable(const char* value) : Variable(BString(value)) {
      }

      Variable(const std::string& value) : Variable(BString(value)) {
      }

      Variable(const Array& value) {
         _type = BeeFishJSON::Type::ARRAY;
         new (&_value._array) std::shared_ptr<Array>(new Array(value));
      }

      Variable(ArrayList list) :
         Variable(Array(list))
      {
      }

      Variable(const Object& value) {
         _type = BeeFishJSON::Type::OBJECT;
         new (&_value._object) std::shared_ptr<Object>(new Object(value));
      }

      Variable(ObjectPointer value) {
         _type = BeeFishJSON::Type::OBJECT;
         new (&_value._object) ObjectPointer(value);
      }

      Variable(List list) :
         Variable(Object(list))
      {
      }

      static Variable& Undefined() {
         static Variable _undefined;
         return _undefined;
      }

      virtual ~Variable() {
         switch (_type) {
         case BeeFishJSON::Type::UNDEFINED:
         case BeeFishJSON::Type::__NULL:
         case BeeFishJSON::Type::BOOLEAN:
         case BeeFishJSON::Type::NUMBER:
            break;
         case BeeFishJSON::Type::STRING:
            _value._string.~String();
            break;
         case BeeFishJSON::Type::ARRAY:
            _value._array.~shared_ptr<Array>();
            break;
         case BeeFishJSON::Type::OBJECT:
            _value._object.~shared_ptr<Object>();
            break;
         }

      }

      virtual Variable& operator = (const Variable& source) {
         this->~Variable();
         _type = source._type;
         new (&_value) Value(source._type, source._value);
         return *this;
      }

      virtual bool operator == (const Variable& compare) const {

         if (_type == BeeFishJSON::Type::UNDEFINED && compare._type == BeeFishJSON::Type::UNDEFINED)
            return true;

         if (_type == compare._type) {
            return (memcmp(&_value, &compare._value, sizeof(Value)) == 0);
         }

         return false;

      }

      virtual bool operator != (const Variable& compare) const {

         if (_type == BeeFishJSON::Type::UNDEFINED && compare._type == BeeFishJSON::Type::UNDEFINED)
            return false;

         if (_type == compare._type) {
            return (memcmp(&_value, &compare._value, sizeof(Value)) != 0);
         }

         return true;

      }

      virtual bool operator == (const Null compare) const {

         if (_type == BeeFishJSON::Type::__NULL)
            return true;

         return false;

      }

      virtual bool operator != (const Null compare) const {

         if (_type != BeeFishJSON::Type::__NULL)
            return true;

         return false;

      }

      virtual bool operator == (bool compare) const {

         return (_type == BeeFishJSON::Type::BOOLEAN) && (_value._boolean == compare);

      }

      virtual bool operator == (double compare) const {

         return (_type == BeeFishJSON::Type::NUMBER) && (_value._number == compare);

      }

      virtual bool operator == (const char* compare) const {

         return (_type == BeeFishJSON::Type::STRING) && (_value._string == BString(compare));

      }

      friend ostream& operator << (ostream& out, const Variable& variable) {
         variable.write(out);
         return out;
      }

      virtual void write(ostream& out, size_t tabIndex = 0) const {
         switch (_type) {
         case BeeFishJSON::Type::UNDEFINED:
            out << "undefined";
            break;
         case BeeFishJSON::Type::__NULL:
            out << "null";
            break;
         case BeeFishJSON::Type::BOOLEAN:
            if (_value._boolean)
               out << "true";
            else
               out << "false";
            break;
         case BeeFishJSON::Type::NUMBER:
            out << _value._number;
            break;
         case BeeFishJSON::Type::STRING:
            out << "\"";
            _value._string.writeEscaped(out);
            out << "\"";
            break;
         case BeeFishJSON::Type::ARRAY:
         {

            out << "[";            
            Array* array = _value._array.get();

            for  (Array::const_iterator it = array->cbegin();
                   it != array->cend();)
            {
               const Variable& element = *it;
               element.write(out, tabIndex + 1);
               if (++it != array->cend())
                  out << ", ";
            }

            out << "]";
            break;
         }
         case BeeFishJSON::Type::OBJECT:
            _value._object->write(out, tabIndex + 1);
            break;
         default:
            throw std::logic_error("JSON::Variable::write");
            return;
         }
      }

      virtual BString bstr() const {
         std::stringstream stream;
         stream << *this;
         return stream.str();
      }

      BeeFishJSON::Type type() const {
         return _type;
      }

#define CHECK_TYPE(type) {if (_type != type) throw std::runtime_error("Cannot cast variable to type");}

      operator Boolean& (){
         CHECK_TYPE(BeeFishJSON::Type::BOOLEAN);
         return _value._boolean;
      }

      operator Number& () {
         CHECK_TYPE(BeeFishJSON::Type::NUMBER);
         return _value._number;
      }

      operator String& () {
         CHECK_TYPE(BeeFishJSON::Type::STRING);
         return _value._string;
      } 

      operator std::shared_ptr<Array> () {
         CHECK_TYPE(BeeFishJSON::Type::ARRAY);
         return _value._array;
      }

      operator std::shared_ptr<Object>() {
         CHECK_TYPE(BeeFishJSON::Type::OBJECT);
         return _value._object;
      }

   };

   inline void Object::write(ostream& out, size_t tabs) const {

      ostream& output = out;
      output << "{" << endl;;

      bool emptySet = (_table.size() == 0);

      for (Object::const_iterator it = cbegin(); it != cend();) {
         const BString& key = *it;
         const Variable& value = at(key);
         output << string(tabs * TabSpaces, ' ');
         output << "\"";
         key.writeEscaped(output);
         output << "\": ";
         value.write(output, tabs);
         ++it;
         if (it != _table.cend())
            output << "," << endl;
      }

      if (!emptySet)
         output << endl;

      output << string(tabs * TabSpaces, ' ');
      output << "}";

      return;
   }
   
   inline void Object::loadMap(List list) {
      for (List::iterator it = list.begin(); it != list.end(); ++it) {
         insert(*it);
      }
   }

   inline pair<Object::iterator,bool> Object::insert(const Object::value_type& pair) {
      _table.push_back(pair.first);
      return Map::insert(pair);
   }

   inline void Object::apply(const ObjectPointer value) {
      const Object object = *value;
      for (auto it = object.cbegin(); it != object.cend(); ++it) {
         const BString& key = *it;
         (*this)[key] = object[key];
      }
   }

   inline const Variable& Object::operator[] (const BString& key) const {

      if (count(key) == 0)
         return Variable::Undefined();

      return Map::at(key);
      
   }


}

#endif