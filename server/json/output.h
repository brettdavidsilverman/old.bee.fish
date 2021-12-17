#ifndef BEE_FISH_JSON__OUTPUT_H
#define BEE_FISH_JSON__OUTPUT_H

#include <map>
#include <unordered_map>
#include "../b-string/string.h"

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

   const int TabSpaces = 3;
   typedef map<BString, Variable> Map;

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

   };
   

   class Variable {
   public:
      enum Type {
         _NULL,
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
            case _NULL:
               break;
            case BOOLEAN:
               _boolean = source._boolean;
               break;
            case NUMBER:
               _number = source._number;
               break;
            case STRING:
               new (&_string) BString(source._string);
               break;
            case OBJECT:
               new (&_object) Object(source._object);
               break;
            case ARRAY:
               new (&_array) Array(source._array);
               break;
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
            }
         }

         ~Value() {
         }

      } _value;

      Variable(const Variable& source) : 
         _type(source._type),
         _value(source._type, source._value)
      {
         
      }

      Variable(const Null& _nullptr) {
         _type = _NULL;
      }

      Variable(const Boolean& boolean) {
         _type = BOOLEAN;
         _value._boolean = boolean;
      }

      Variable(const Number& number) {
         _type = NUMBER;
         _value._number = number;
      }

      Variable(const int& number) : Variable(Number(number)) {
      }

      Variable(const String& value) {
         _type = STRING;
         new (&_value._string) String(value);
      }

      Variable(const char* value) : Variable(BString(value)) {
      }

      Variable(const Object& value) {
         _type = OBJECT;
         new (&_value._object) Object(value);
      }

      Variable(std::initializer_list<Object::value_type> list) :
         Variable(Object(list))
      {
      }

      Variable(const Array& value) {
         _type = ARRAY;
         new (&_value._array) Array(value);
      }

      Variable(std::initializer_list<Array::value_type> list) :
         Variable(Array(list))
      {
      }

      virtual ~Variable() {
         switch (_type) {
         case _NULL:
         case BOOLEAN:
         case NUMBER:
            break;
         case STRING:
            _value._string.~String();
            break;
         case OBJECT:
            _value._object.~Object();
            break;
         case ARRAY:
            _value._array.~Array();
            break;
         }

      }

      virtual void write(ostream& out, size_t tabIndex = 0) const {
         switch (_type) {
         case _NULL:
            out << "null";
            break;
         case BOOLEAN:
            if (_value._boolean)
               out << "true";
            else
               out << "false";
            break;
         case NUMBER:
            out << _value._number;
            break;
         case STRING:
            out << "\"";
            _value._string.writeEscaped(out);
            out << "\"";
            break;
         case OBJECT:
            _value._object.write(out, tabIndex + 1);
            break;
         case ARRAY:
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