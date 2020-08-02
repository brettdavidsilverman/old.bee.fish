#include "object.h"

namespace bee::fish::parser::json
{
   void Object::write(ostream& out)
   {
      out << "{";
      
      for (auto it = begin();
                it != end();)
      {
         auto pair = *it;
         const wstring& label =
             pair.first;
                  
         JSON* json = pair.second;
               
         out << "\"";
         Match::write(out, label);
         out << "\""
             << ":"
             << *json;
             
         if (++it != end())
            out << ",";
            
      }
      out << "}";
   }
      
   void Object::Field::onsuccess() {

      Field& field = *this;
      Label& label = field.label();
      JSON& item = field.item();


      if (label.success() &&
          item.success())
      {
         field.object().insert_or_assign(
            label.wvalue(),
            &item
         );
      }
   }

}
