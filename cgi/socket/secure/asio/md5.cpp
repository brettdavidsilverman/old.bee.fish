#include "md5.h"

std::string md5(const std::string & str) {
  
   unsigned char result[MD5_DIGEST_LENGTH];
   MD5((unsigned char*)str.c_str(), str.size(), result);

   std::ostringstream sout;
   sout<<std::hex<<std::setfill('0');
   for(long long c: result)
   {
       sout<<std::setw(2)<<(long long)c;
   }
   return sout.str();
}
