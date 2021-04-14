#ifndef BEE_FISH_DATABASE__STORAGE_H
#define BEE_FISH_DATABASE__STORAGE_H
#include <exception>
#include <iomanip>
#include "../b-string/string.h"
#include "../power-encoding/power-encoding.h"
#include "../https/authentication.h"
#include "database.h"
#include "path.h"
using namespace bee::fish::https;

namespace bee::fish::database {

   class Storage
   {
      Authentication& _auth;
      bee::fish::database::
         Path<PowerEncoding> _bookmark;
      
   public:
      Storage(Authentication& auth, BString name) :
         _auth(auth),
         _bookmark(_auth.userData()[name])
      {
      }
      
      bool has(const BString& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path = path[key];
         return 
            path.hasData();
      }
      
      BString getItem(const BString& key)
      {
         
         bee::fish::database::
            Path path(_bookmark);
            
         path << key;
         
         BString data;
         
         if (path.hasData())
         {
            path.getData(data);
         }
         
         return data;
      }
      
      void setItem(
         const BString& key,
         const BString& value
      )
      {
      
         bee::fish::database::
            Path path(_bookmark);
            
         path << key;
         
         path.setData(
            value
         );
      }
      
      void removeItem(const BString& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         path.deleteData();
      }
      
      void clear()
      {
         bee::fish::database::
            Path path(_bookmark);
         path.clear();
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