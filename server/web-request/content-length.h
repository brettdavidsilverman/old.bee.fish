#ifndef BEE_FISH_WEB__CONTENT_LENGTH_H
#define BEE_FISH_WEB__CONTENT_LENGTH_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"

using namespace BeeFishParser;
      
namespace BeeFishWeb {

   class ContentLength :
      public Match,
      public BStream
   {
   public:
      size_t _contentCount;
      size_t _contentLength = 0;
   public:

      
      ContentLength() :
         _contentCount(0),
         _contentLength(0)
      {
      }

      virtual void setup(Parser* parser, Headers* headers) {
         Match::setup(parser);
         if (headers->contains("content-length") ) {
            BString contentLengthString = (*headers)["content-length"];
            _contentLength = atol(contentLengthString.c_str());
            parser->setDataBytes(_contentLength);
         }
      }

      virtual bool matchCharacter(const Char& character) {
         
         ++_contentCount;
         
         if (_contentCount == _contentLength)
            _result = true;

//         if (Match::matchCharacter(character))
          BStream::push_back((Byte)character);

         return true;

      }

      virtual void flush() {
         BStream::flush();
      }

   };
   

};

#endif
