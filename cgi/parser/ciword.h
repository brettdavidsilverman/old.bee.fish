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
      cout << "CIWord("
           << "\"" << word << "\""
           << ")"
           << endl;
   }
   
   CIWord(const CIWord& source) :
      Word(source)
   {
   }
   
   virtual void write(ostream& out) const {
      out << "CIWord";
   }

   virtual Match* copy() {
      CIWord* copy = new CIWord(*this);
      return copy;
   }
   
protected:
   virtual bool matchChar(int character) {
   
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