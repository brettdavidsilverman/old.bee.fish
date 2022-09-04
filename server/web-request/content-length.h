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
   public:
      size_t       _contentCount;
      const size_t _contentLength;
   public:

      ContentLength(size_t contentLength) :
         _contentCount(0),
         _contentLength(contentLength)
      {
         reserve(_contentLength);
      }

      virtual bool matchCharacter(const Char& character) {
         
         ++_contentCount;

         if ( _contentCount > _contentLength )
            return false;
         
         if (_contentCount == _contentLength)
            _result = true;

         std::vector<Byte>::push_back((Byte)character);
         BeeFishBString::BStream::push_back(character);

         return true;
      }

   };
   

};

#endif
