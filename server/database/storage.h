#ifndef BEE_FISH_DATABASE__STORAGE_H
#define BEE_FISH_DATABASE__STORAGE_H
#include <exception>
#include <iomanip>
#include "database.h"
#include "path.h"
#include "../power-encoding/power-encoding.h"
#include "../https/base64.h"
#include "../https/wstring.h"
#include "../https/authentication.h"


namespace bee::fish::database {

   class Storage 
   {
      Authentication& _auth;
      bee::fish::database::
         Path<PowerEncoding> _bookmark;
      
   public:
      Storage(Authentication& auth, string name) :
         _auth(auth),
         _bookmark(_auth.userData() << name)
      {
      }
      
      bool has(const string& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         void* data = path.getData();
         return data != NULL;
      }
      
      string getItem(const string& key)
      {
         
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         Database::Data* data = path.getData();
         if (data)
         {
            Size length =
                   data->_size /
               sizeof(char) -
                             1;
            char* value =
               (char*)(data->data());
               
            return string(value, length);
         }
         else
            return "";
      }
      
      void setItem(
         const string& key,
         const string& value
      )
      {
      
         bee::fish::database::
            Path path(_bookmark);
            
         path << key;
         
         Size size =
            (value.length() + 1) * 
            sizeof(char);
         
         path.setData(
            value.c_str(),
            size
         );
      }
      
      void removeItem(const string& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         path.deleteData();
      }
      /*
      void write(ostream& out, const wstring& key)
      {
         out << "\"";
         out << hex;
         for ( auto it = key.begin();
                    it != key.end();
                    ++it )
         {
            wchar_t wc = *it;
            unsigned int high =
               (wc & 0xFF00) >> 8;
            unsigned int low =
               wc & 0x00FF;
            out << high << ":" << low;
            
            if (it < key.end() - 1)
               out << ",";
         }
         
         out << "\"" << endl;
         
         out << dec;
      }
      */
      
      
   };

};

#endif