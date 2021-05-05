#ifndef BEE_FISH_DATABASE__STORAGE_H
#define BEE_FISH_DATABASE__STORAGE_H
#include <exception>
#include <iomanip>
#include "../b-string/string.h"
#include "../power-encoding/power-encoding.h"
#include "authentication.h"
#include "../database/database.h"
#include "../database/path.h"

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
         path << key;
         return 
            path.hasData();
      }
      
      BString getItem(const BString& key)
      {
         
         bee::fish::database::
            Path path(_bookmark);
            
         path << key;
         
         BString value;
         
         if (path.hasData())
         {
            path.getData(value);
         }
         
         return value;
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
      
      
      
   };

};

#endif