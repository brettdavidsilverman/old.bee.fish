#ifndef BEE_FISH_DATABASE__STORAGE_H
#define BEE_FISH_DATABASE__STORAGE_H
#include <exception>
#include <iomanip>
#include "../b-string/string.h"
#include "../power-encoding/power-encoding.h"
#include "authentication.h"
#include "../database/database.h"
#include "../database/path.h"
#include "../id/id.h"

using namespace bee::fish::https;
using namespace bee::fish::id;

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
      
      template<typename Key>
      bool has(const Key& key)
      {
         bee::fish::database::
            Path path(_bookmark);
            
         seek(path, key);
         
         return 
            path.hasData();
      }
      
      template<typename Key>
      BString getItem(const Key& key)
      {
         
         bee::fish::database::
            Path path(_bookmark);
            
         seek(path, key);
         
         BString value;
         
         if (path.hasData())
         {
            path.getData(value);
         }

         return value;
      }
      
      template<typename Key>
      void setItem(
         const Key& key,
         const BString& value
      )
      {
      
         bee::fish::database::
            Path path(_bookmark);
            
         seek(path, key);
         
         path.setData(
            value
         );
         
      }
      
      template<typename Key>
      void removeItem(const Key& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         seek(path, key);
         path.deleteData();
      }
      
      void clear()
      {
         bee::fish::database::
            Path path(_bookmark);
         path.clear();
      }
      
      virtual BString name()
      {
         return "Storage";
      }
      
      template<typename Key>
      void seek(
         Path<PowerEncoding>& path,
         const Key& key
      )
      {
         path << key;
      }
   };

};

#endif