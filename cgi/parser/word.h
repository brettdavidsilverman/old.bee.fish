#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include <string.h>

namespace Bee::Fish::Parser {

using namespace std;
            
class Word : public Match {
protected:
   size_t _index = 0;
   string _word;
   
   virtual bool matchChar(int character) {
      if (character == Match::eof)
         return false;
      return (_word[_index] == (char)character);
   }
   
public:

   Word(const string& word) {
      _word = word;
   }
   
   virtual bool match(int character) {
   
      bool matched = matchChar(character);
         
      if (matched)
      {
         Match::match(character);
         ++_index;
         
         if (_index == _word.length())
            setSuccess(true);
      }
      else
         setSuccess(false);
         
      return matched;
   }
   

};


};

#endif
