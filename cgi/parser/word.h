#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include <string.h>

namespace Bee::Fish::Parser {

using namespace std;
            
class Word : public Match {
protected:
   
   const string& _word;
   string::const_iterator _index;
   
   virtual bool matchChar(int character) {
      if (character == Match::eof)
         return false;
      return (
         (*_index) == (char)character
      );
   }
   
public:

   Word(const string& word) :
      _word(word)
   {
      cout << "Word::Word("
           << "\"" << _word << "\""
           << ")"
           << endl;
      _index = _word.begin();
   }
   
   virtual optional<bool> match(int character) {
   
      bool matched = matchChar(character);
         
      if (matched)
      {
         Match::match(character);
         ++_index;
         
         if (_index == _word.end())
            _success = true;
      }
      else
         _success = false;
         
      return _success;
   }
   

};


};

#endif
