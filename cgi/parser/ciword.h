#include "word.h"
#include <stdio.h>
#include <ctype.h>

namespace Bee::Fish::Parser {

using namespace std;

class CIWord : public Word {
public:
   CIWord(const string& word) : Word(word)
   {}
   
protected:
   virtual bool matchChar(int character) {
   
      if (character == Match::eof)
         return false;
         
      return (
         tolower(_word[_index]) == 
         tolower(character)
      );
   }
};

}