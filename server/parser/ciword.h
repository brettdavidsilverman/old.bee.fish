#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H

#include <stdio.h>
#include <ctype.h>
#include "word.h"

namespace bee::fish::parser {

   using namespace std;

   class CIWord : public Word {
   public:
      CIWord(const BString& word) :
         Word(word)
      {
      
      }

      CIWord(const CIWord& source) :
         Word(source._word)
      {
      }
      
      virtual MatchPtr copy() const
      {
         return new CIWord(*this);
      }
      
      virtual void write(ostream& out) const
      {
         Word::write(out, "CIWord");
      }
      
   protected:
      virtual bool matchChar(Char character) {
   
         if (character == BString::EndOfFile)
            return false;
         
         return (
            tolower(*_index) == 
            tolower((char)character)
         );
      }
   };

}

#endif
