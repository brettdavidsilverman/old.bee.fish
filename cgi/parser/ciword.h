#include "word.h"
#include <stdio.h>
#include <ctype.h>

namespace Bee::Fish::Parser {

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