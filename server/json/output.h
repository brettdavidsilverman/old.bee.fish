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

   };
   
}

#endif