#ifndef BEE_FISH_B_STRING__STRING_H
#define BEE_FISH_B_STRING__STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <openssl/md5.h>
#include "../power-encoding/power-encoding.h"
#include "utf-8.h"
#include "char.h"
#include "b-string.h"
#include "data.h"
#include "bit-stream.h"
#include "base64.h"

namespace bee::fish::b_string
{
   
   inline PowerEncoding& operator <<
   ( 
      PowerEncoding& stream,
      const char* string
   )
   {
      return operator <<
      (stream, BString(string));
   }

   inline BString Data::md5() const
   {

      Byte result[MD5_DIGEST_LENGTH];
      memset(result, 0, MD5_DIGEST_LENGTH);
         
      MD5(
         (Byte*)(c_str()),
         size(),
         result
      );
         
      Data data(result, MD5_DIGEST_LENGTH);
         
      return data.toHex();
         
   }
   
   inline BString Data::sha3() const
   {
      uint32_t digest_length = SHA512_DIGEST_LENGTH;
      const EVP_MD* algorithm = EVP_sha3_512();
      uint8_t* digest = static_cast<uint8_t*>(OPENSSL_malloc(digest_length));
      EVP_MD_CTX* context = EVP_MD_CTX_new();
      EVP_DigestInit_ex(context, algorithm, nullptr);
      EVP_DigestUpdate(context, c_str(), size());
      EVP_DigestFinal_ex(context, digest, &digest_length);
      EVP_MD_CTX_destroy(context);
      Data dataDigest(digest, SHA512_DIGEST_LENGTH);
      BString output = dataDigest.toHex();
      OPENSSL_free(digest);
      return output;
   }
   
   inline BString Data::toHex() const
   {
      std::stringstream stream;
         
      for( uint16_t chunk : *this)
      {
         stream << std::hex 
                << std::setw(2)
                << std::setfill('0')
                << chunk;
      }
      
      return stream.str();
      
   }
}

#endif


