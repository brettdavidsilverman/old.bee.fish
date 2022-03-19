#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../power-encoding/power-encoding.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "set.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON {
   
   class JSON;
  
   class ArrayOpenBrace : public BeeFishParser::Character {
   public:
      ArrayOpenBrace() : Character('[') {

      }
   };

   class ArrayCloseBrace : public BeeFishParser::Character {
   public:
      ArrayCloseBrace() : Character(']') {

      }
   };

   class ArraySeperator : public BeeFishParser::Character {
   public:
      ArraySeperator() : Character(',') {

      }
   };

   class Array : public Set<ArrayOpenBrace, LoadOnDemand<JSON>, ArraySeperator, ArrayCloseBrace>
   {
   public:
      size_t _size = 0;

   public:
      Array() : Set()
      {
      }
      
      
      virtual ~Array()
      {
      }
      
      virtual void matchedSetItem(LoadOnDemand<JSON>* item)
      {
         ++_size;
         
         Set::matchedSetItem(item);
      }
   };
    
}

#endif