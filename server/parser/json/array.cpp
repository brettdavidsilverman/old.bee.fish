#include "json.h"
#include "array.h"

namespace bee::fish::parser::json
{
   JSON* Array::LazyLoadJSON::createItem()
   {
      return new JSON(_array);
   }
   
   void Array::write(ostream& out)
   {
      out << "[";
      for (auto it = begin();
                it != end();
          )
      {
         JSON* json = (*it);
         json->write(out);
         if (++it != end())
            out << ",";
      }
         
      out << "]";
   }
   
   void Array::LazyLoadJSON::onsuccess()
   {  
      JSON& json =
         LazyLoadJSON::match();
         
      array().push_back(&json);
   }
}