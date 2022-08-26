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
      public std::vector<Byte>,
      public BeeFishBString::BStream
   {
   protected:
      size_t       _contentCount = 0;
      const size_t _contentLength;
   public:

      ContentLength(size_t contentLength) :
         _contentLength(contentLength)
      {
      }

      virtual bool matchCharacter(const Char& character) {

         std::vector<Byte>::push_back((Byte)character);
         BeeFishBString::BStream::push_back(character);

         ++_contentCount;

         if ( _contentCount >= _contentLength )
            _result = true;

         return true;
      }

   };
   

};

#endif
