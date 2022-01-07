#ifndef BEE_FISH_JSON__OUTPUT_H
#define BEE_FISH_JSON__OUTPUT_H

#include <map>
#include <unordered_map>
#include "../b-string/string.h"
#include "../misc/constants.h"

using namespace BeeFishBString;

using namespace std;

namespace BeeFishJSONOutput {
   

   class Object;
   class Variable;

   typedef nullptr_t Null;
   typedef bool Boolean; 
   typedef double Number;
   typedef BString String;
   typedef vector<Variable> Array;

   typedef std::map<BString, Variable> Map;

   class Object : public Map {
   public:

      Object() {

      }

      Object(std::initializer_list<value_type> list) : map(list) 
      {
      }

      Object(const Object& source) : map<BString, Variable>(source) {
      }


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
   };
   

   class Variable {
   public:
      enum Type {
         UNDEFINED,
         _NULL_,
         BOOLEAN,
         NUMBER,
         STRING,
         OBJECT,
         ARRAY
      } _type;

      union Value {
         Boolean _boolean;
         Number _number;
         String _string;
         Object _object;
         Array _array;

         Value() {

         }

         Value(Type type, const Value& source) {
            switch (type) {
            case Type::UNDEFINED:
            case Type::_NULL_:
               break;
            case Type::BOOLEAN:
               _boolean = source._boolean;
               break;
            case Type::NUMBER:
               _number = source._number;
               break;
            case Type::STRING:
               new (&_string) BString(source._string);
               break;
            case Type::OBJECT:
               new (&_object) Object(source._object);
               break;
            case Type::ARRAY:
               new (&_array) Array(source._array);
               break;
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
            }
         }

         ~Value() {
         }

      } _value;

      Variable() :
         _type(Type::UNDEFINED)
      {

      } 

      Variable(const Variable& source) : 
         _type(source._type),
         _value(source._type, source._value)
      {
         
      }

      Variable(const Null& _nullptr) {
         _type = Type::_NULL_;
      }

      Variable(const Boolean& boolean) {
         _type = Type::BOOLEAN;
         _value._boolean = boolean;
      }

      Variable(const Number& number) {
         _type = Type::NUMBER;
         _value._number = number;
      }

      Variable(const int& number) : Variable(Number(number)) {
      }

      Variable(const unsigned int& number) : Variable(Number(number)) {
      }

      Variable(const void *& pointer) : Variable(Number((unsigned long)pointer)) {
      }

      Variable(const String& value) {
         _type = Type::STRING;
         new (&_value._string) String(value);
      }

      Variable(const char* value) : Variable(BString(value)) {
      }

      Variable(const Object& value) {
         _type = Type::OBJECT;
         new (&_value._object) Object(value);
      }

      Variable(std::initializer_list<Object::value_type> list) :
         Variable(Object(list))
      {
      }

      Variable(const Array& value) {
         _type = Type::ARRAY;
         new (&_value._array) Array(value);
      }

      Variable(std::initializer_list<Array::value_type> list) :
         Variable(Array(list))
      {
      }

      virtual ~Variable() {
         switch (_type) {
         case Type::UNDEFINED:
         case Type::_NULL_:
         case Type::BOOLEAN:
         case Type::NUMBER:
            break;
         case Type::STRING:
            _value._string.~String();
            break;
         case Type::OBJECT:
            _value._object.~Object();
            break;
         case Type::ARRAY:
            _value._array.~Array();
            break;
         }

      }

      virtual Variable& operator = (const Variable& source) {
         this->~Variable();
         _type = source._type;
         new (&_value) Value(source._type, source._value);
         return *this;
      }

      virtual void write(ostream& out, size_t tabIndex = 0) const {
         switch (_type) {
         case Type::UNDEFINED:
            out << "undefined";
            break;
         case Type::_NULL_:
            out << "null";
            break;
         case Type::BOOLEAN:
            if (_value._boolean)
               out << "true";
            else
               out << "false";
            break;
         case Type::NUMBER:
            out << _value._number;
            break;
         case Type::STRING:
            out << "\"";
            _value._string.writeEscaped(out);
            out << "\"";
            break;
         case Type::OBJECT:
            _value._object.write(out, tabIndex + 1);
            break;
         case Type::ARRAY:
            out << string((tabIndex + 1) * TabSpaces, ' ') << "[" << endl;
            for  (Array::const_iterator it = _value._array.cbegin();
                   it != _value._array.cend();)
            {
               const Variable& element = *it;
               out << string((tabIndex + 2) * TabSpaces, ' ');
               element.write(out, tabIndex + 1);
               if (++it != _value._array.cend())
                  out << ",";
               out << endl;
            }
            out << string((tabIndex + 1) * TabSpaces, ' ') << "]";
            break;
         default:
            throw std::logic_error("JSON::Variable::write");
            return;
         }
      }

      friend ostream& operator << (ostream& out, const Variable& variable) {
         variable.write(out);
         return out;
      }


   };

   inline void Object::write(ostream& out, size_t tabs) const {


      out << "{" << endl;;

      for (Object::const_iterator it = cbegin(); it != cend();) {
         auto pair = *it;
         const BString& key = pair.first;
         const Variable& value = pair.second;
         out << string((tabs + 1) * TabSpaces, ' ');
         out << "\"";
         key.writeEscaped(out);
         out << "\": ";
         value.write(out, tabs);
         ++it;
         if (it != cend())
            out << "," << endl;
      }

      out << endl << string(tabs * TabSpaces, ' ');
      out << "}";

      return;
   }
   

}

#endif