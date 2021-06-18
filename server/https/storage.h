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
      
      bool has(const Id& id)
      {
         bee::fish::database::
            Path path(_bookmark);
            
         seek(path, id);
         
         return 
            path.hasData();
      }
      
      BString getItem(const Id& id)
      {
         
         bee::fish::database::
            Path path(_bookmark);
            
         seek(path, id);
         
         BString value;
         
         if (path.hasData())
         {
            path.getData(value);
         }

         return value;
      }
      
      void setItem(
         const Id& id,
         const BString& value
      )
      {
      
         bee::fish::database::
            Path path(_bookmark);
            
         seek(path, id);
         
         path.setData(
            value
         );
         
      }
      
      void removeItem(const Id& id)
      {
         bee::fish::database::
            Path path(_bookmark);
         seek(path, id);
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
      
      void seek(
         Path<PowerEncoding>& path,
         const Id& id
      )
      {
         path << id;
      }
   };

};

#endif