#ifndef BEE_FISH_SERVER__STORAGE_H
#define BEE_FISH_SERVER__STORAGE_H
#include <exception>
#include "../database/database.h"
#include "../database/path.h"
#include "../power-encoding/power-encoding.h"
#include "../https/base64.h"
#include "../https/wstring.h"
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
      
      bool has(const wstring& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         void* data = path.getData();
         return data != NULL;
      }
      
      wstring getItem(const wstring& key)
      {
            
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         Database::Data* data = path.getData();
         if (data)
         {
            cerr << "Get Item..." << endl;
          wcerr << L"Key:   " << key << endl;
            cerr << "Index: " << path._index << endl;
            Branch& branch = path.getBranch();
            cerr << "Dat ix:" << branch._dataIndex << endl;
            cerr << "Data:  " << data << endl;
            
            Size length =
                   data->_size /
               sizeof(wchar_t) -
                             1;
            wchar_t* value =
               (wchar_t*)(data->data());
               
            return wstring(value, length);
         }
         else
            return L"";
      }
      
      void setItem(
         const wstring& key,
         const wstring& value
      )
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         Size size =
            (value.length() + 1) * 
            sizeof(wchar_t);
         
         Database::Data* data =
            path.setData(
               value.c_str(),
               size
            );
        
         cerr << "Set Item..." << endl;
       wcerr << L"Key:   " << key << endl;
         cerr << "Index: " << path._index << endl;
         Branch& branch = path.getBranch();
         cerr << "Dat ix:" << branch._dataIndex << endl;
         cerr << "Data:  " << data << endl;
      }
      
      void removeItem(const wstring& key)
      {
         bee::fish::database::
            Path path(_bookmark);
         path << key;
         path.deleteData();
      }
   };

};

#endif