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

using namespace BeeFishHTTPS;
using namespace BeeFishId;

namespace BeeFishDatabase {

   class Storage
   {

      typedef BeeFishDatabase::
         Path<PowerEncoding> Path;

      Authentication& _auth;
      Path _bookmark;
   public:
      Storage(Authentication& auth, BString name) :
         _auth(auth),
         _bookmark(_auth.userData()[name])
      {
      }
      
      template<typename Key>
      bool has(const Key& key)
      {
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         return 
            path.hasData();
      }
      
      template<typename Key>
      BeeFishMisc::optional<Data> getItem(
         const Key& key,
         BeeFishMisc::optional<BString>& contentType
      )
      {
         
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         if (path.hasData())
         {
            Data data;
            path.getData(data);

            if (path.contains("content-type")) {
               seek(path, "content-type");
               BString type;
               path.getData(type);
               contentType = type;
            }

            return data;
         }

         return BeeFishMisc::nullopt;
      }
/*      
      template<typename Key>
      void setItem(
         const Key& key,
         const BString& value,
         BeeFishMisc::optional<BString> contentType
      )
      {
      
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         path.setData(
            value
         );
         
         if (contentType.hasValue()) {
            setContentType(path, contentType.value());
         }
      }
*/
      template<typename Key>
      void setItem(
         const Key& key,
         const BeeFishBString::Data& data,
         BeeFishMisc::optional<BString> contentType
      )
      {
      
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         path.setData(
            data
         );
         
         if (contentType.hasValue()) {
            setContentType(path, contentType.value());
         }

      }

      virtual void setContentType(Path path, BString contentType) {
         seek(path, BString("content-type"));
         path.setData(contentType);
      }

      template<typename Key>
      void removeItem(const Key& key)
      {
         BeeFishDatabase::
            Path path(_bookmark);
         
         seek(path, key);
         path.deleteData();

      }
      
      void clear()
      {
         BeeFishDatabase::
            Path path(_bookmark);
         path.clear();
      }
      
      virtual BString name()
      {
         return "Storage";
      }
      
      template<typename Key>
      void seek(
         Path& path,
         const Key& key
      )
      {
         path << key;
      }
   };

};

#endif