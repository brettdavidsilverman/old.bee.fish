#include "md5.h"

namespace bee::fish::server
{

   std::string md5(const std::wstring & wstr) {
  
      unsigned char result[MD5_DIGEST_LENGTH];
      
      MD5(
         (unsigned char*)wstr.data(),
         sizeof(wchar_t) * wstr.size(),
         result
      );

      std::ostringstream sout;
      
      sout << std::hex << std::setfill('0');
      
      for(long long c: result)
      {
          sout << std::setw(2) << (long long)c;
      }
      
      return sout.str();
      
   }

}
