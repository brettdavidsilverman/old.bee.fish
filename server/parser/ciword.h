#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H
#include "word.h"
#include <stdio.h>
#include <ctype.h>

namespace bee::fish::parser {

using namespace std;

class CIWord : public Word {
public:
   CIWord(const string& word) :
      Word(word)
   {
      
   }
   
   
   virtual void write(ostream& out) {
      out << "CIWord(";
      Match::write(out);
      out << ":\"" << _word << "\""
          << ")"
          << endl;
   }

   
protected:
   virtual bool match_char(int character) {
   
      if (character == Match::endOfFile)
         return false;
         
      return (
         tolower(*_index) == 
         tolower((char)character)
      );
   }
};

}

#endif
