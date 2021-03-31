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

namespace bee::fish::power_encoding
{

   inline PowerEncoding&
   PowerEncoding::operator <<
   (const BString& str)
   {
      writeBit(true);
         
      for (const Char& character : str)
      {
         writeBit(true);
         *this << character;
      }
         
      writeBit(false);
         
      return *this;
   }
      
   inline PowerEncoding&
   PowerEncoding::operator >>
   (BString& bstring)
   {
      bool bit = readBit();
      if (!bit)
         throw runtime_error("Expected 'true' bit");
         
      bstring.clear();
      
      Char character;
         
      while (readBit())
      {
         *this >> character;
         bstring.push_back(character);
      }
         
      return *this;
         
   }
      
   inline PowerEncoding&
   PowerEncoding::operator <<
   (const Char& character)
   {
      *this << character._character;
      /*
      for (bool bit : character)
      {
         writeBit(bit);
      }
      */
      return *this;
   }
      
   inline PowerEncoding&
   PowerEncoding::operator >>
   (Char& character)
   {
      *this >> character._character;
      
      /*character.clear();
      size_t count = 1;
      bool bit;
      while (count > 0)
      {
         bit = readBit();
         
         if (bit)
            ++count;
         else
            --count;
            
         character.push_back(bit);
      }
      */
      return *this;
   }
      
   inline PowerEncoding&
   PowerEncoding::operator <<
   (const char* str)
   {
      BString string(str);
      return *this << (string);
   }
   
}

namespace bee::fish::b_string
{

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


