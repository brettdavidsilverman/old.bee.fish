#ifndef BEE_FISH_HTTPS__RESPONSE_HEADERS_H
#define BEE_FISH_HTTPS__RESPONSE_HEADERS_H
#include <vector>
#include <filesystem>

using namespace std;

namespace bee::fish::https {

   class Session;
   
   typedef multimap<string, BString>
      ResponseHeadersBase;
   
   class ResponseHeaders :
      public ResponseHeadersBase
   {
   public:
      ResponseHeaders()
      {
      }

      void replace(string key, BString value)
      {
         erase(key);
         emplace(key, value);
      }
      
   };
   

}

#endif