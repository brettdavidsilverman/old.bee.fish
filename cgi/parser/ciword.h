#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H
#include "word.h"
#include <stdio.h>
#include <ctype.h>

namespace bee::fish::parser {

using namespace std;

class CIWord : public Word {
public:
   CIWord(const string& word) : Word(word)
   {
      
   }
   
   CIWord(const CIWord& source) :
      Word(source)
   {
   }
   
   virtual void write(ostream& out) const {
      out << "CIWord("
          << "\"" << _word << "\""
          << ")"
          << endl;
   }

   virtual Match* copy() {
      CIWord* copy = new CIWord(*this);
      return copy;
   }
   
protected:
   virtual bool match_char(int character) {
   
      if (character == Match::eof)
         return false;
         
      return (
         tolower(*_index) == 
         tolower((char)character)
      );
   }
};

}

#endif
