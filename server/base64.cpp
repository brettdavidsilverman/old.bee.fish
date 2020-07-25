#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "base64.h"

namespace base64 {

   void _encode(
      const base64::byte* in,
      size_t in_len,
      char** out,
      size_t* out_len)
   {
      BIO *buff, *b64f;
      BUF_MEM *ptr;
      
      b64f = BIO_new(BIO_f_base64());
      buff = BIO_new(BIO_s_mem()); 
      buff = BIO_push(b64f, buff);
     
      BIO_set_flags(
         buff,
         BIO_FLAGS_BASE64_NO_NL
      );
      
      BIO_set_close(buff, BIO_CLOSE);
      BIO_write(buff, in, in_len);
      BIO_flush(buff);
      BIO_get_mem_ptr(buff, &ptr);
      (*out_len) = ptr->length;
      (*out) = (char *) malloc(
         ((*out_len) + 1) * sizeof(char)
      );
      
      memcpy(*out, ptr->data, (*out_len));
      (*out)[(*out_len)] = '\0';
      
      BIO_free_all(buff); 
   }
   
   string encode(const string& source)
   {
   
      char* out;
      size_t len;
      const base64::byte*
         bytes = 
            (const base64::byte*)
               source.c_str();
         
     _encode(
         bytes,
         source.size(),
         &out,
         &len
      );
      
      string out_string(out, len);
      free(out);
      return out_string;
   }
   
   void _decode(
      const char* in,
      size_t in_len,
      byte** out,
      size_t* out_len)
   {
      BIO *buff, *b64f;
      b64f = BIO_new(BIO_f_base64());
      buff = BIO_new_mem_buf(
         (void *)in, in_len
      );
      
      buff = BIO_push(b64f, buff);
      (*out) = (byte *) malloc(
         in_len * sizeof(char)
      );
      
      BIO_set_flags(
         buff,
         BIO_FLAGS_BASE64_NO_NL
      );
      
      BIO_set_close(buff, BIO_CLOSE);
      (*out_len) = BIO_read(
         buff, (*out), in_len
      );
      
      (*out) = (byte *) realloc(
         (void *)(*out),
         ((*out_len) + 1) *
            sizeof(byte)
      );
      
      (*out)[(*out_len)] = '\0';
      
      BIO_free_all(buff);
   }
   
   string decode(string base64)
   {
      base64::byte* bytes;
      size_t len;
      
      _decode(
         base64.c_str(),
         base64.size(),
         &bytes,
         &len
      );
      
      string decoded((char*)bytes, len);
      
      free(bytes);
      
      return decoded;
      
   }
}