#ifndef BEE_FISH_SERVER__STORAGE_H
#define BEE_FISH_SERVER__STORAGE_H
#include <exception>
#include "../database/database.h"
#include "../database/path.h"
#include "../power-encoding/power-encoding.h"
#include "../https/base64.h"
#include "authentication.h"

using namespace bee::fish::database;

namespace bee::fish::server {

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
      
      string getItem(const string& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         char* value = (char*)path.getData();
         string _string;
         if (value)
            _string = string(value);
         return _string;
      }
      
      void setItem(
         const string& key,
         const string& value
      )
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         string base64 = base64::encode(value);
         path.setData(
            base64.c_str(),
            base64.length() + 1
         );
      }
   };

};

#endif