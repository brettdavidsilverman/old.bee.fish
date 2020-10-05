#ifndef BASE64__BASE64_H
#define BASE64__BASE64_H
#include <openssl/bio.h>
#include <string>

using namespace std;

namespace base64 {

   typedef unsigned char byte;
   
   void _encode(
      const base64::byte* in,
      size_t in_len,
      char** out,
      size_t* out_len);
   
   string encode(const string& source);
   
   void _decode(
      const char* in,
      size_t in_len,
      byte** out,
      size_t* out_len);
      
   string decode(string base64);
}

#endif