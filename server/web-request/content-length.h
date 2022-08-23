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
      public BeeFishBString::BStream
   {
   protected:
      size_t _contentCount = 0;
      size_t _contentLength = -1;
   public:

      ContentLength(size_t contentLength) :
         _contentLength(contentLength)
      {
      }

      virtual bool matchCharacter(const Char& character) {

         push_back((uint8_t)character);

         ++_contentCount;

         if ( _contentCount >= _contentLength )
            _result = true;

         return true;
      }

   };
   

};

#endif
