#ifndef BEE_FISH_JSON__OUTPUT_H
#define BEE_FISH_JSON__OUTPUT_H

#include <map>
#include "../b-string/string.h"

using namespace BeeFishBString;

namespace BeeFishJSONOutput {
   
   using namespace std;

   class Object;

   class Object : public map<BString, Object> {
   public:
      BeeFishMisc::optional<BString> _value;
      Object() {

      }

      Object(std::initializer_list<value_type> list) : map(list) 
      {
      }

      Object(const Object& source) : map<BString, Object>(source) {
         _value = source._value;
      }

      Object(const BString& value) {
         _value = value;
      }

      virtual void write(ostream& out, int tabs = 0) const {


         if (_value.hasValue()) {
            //out << string(tabs * 3, ' ');
            out << "\"";
            _value.value().writeEscaped(out);
            out << "\"";
            return;
         }

         out << "{" << endl;;

         for (Object::const_iterator it = cbegin(); it != cend();) {
            auto pair = *it;
            const BString& key = pair.first;
            const Object& value = pair.second;
            out << string((tabs + 1) * 3, ' ');
            out << "\"";
            key.writeEscaped(out);
            out << "\": ";
            value.write(out, tabs + 1);
            ++it;
            if (it != cend())
               out << "," << endl;
         }

         out << endl << string(tabs * 3, ' ');
         out << "}";

         return;
      }

      friend ostream& operator << (ostream& out, const Object& object) {
         object.write(out);
         return out;
      }

   };
   
}

#endif