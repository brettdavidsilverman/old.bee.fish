#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include <string.h>

namespace bee::fish::parser {

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
      _word(word),
      _index(_word.cbegin())
   {
   }
   
   Word(const Word& source) :
      Match(source),
      _word(source._word),
      _index(_word.cbegin())
   {
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
   
   virtual void write(ostream& out) const {
      out << "Word(\"" << _word << "\")";
   }
   
   virtual Match* copy() {
      Word* copy = new Word(*this);
      return copy;
   }
   
};


};

#endif
