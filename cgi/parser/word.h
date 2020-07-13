#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include <string.h>

namespace bee::fish::parser {

using namespace std;
            
class Word : public Match {
protected:
   
   string _word;
   string::const_iterator _index;
   
   virtual bool match_char(int character) {
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
      _index = _word.cbegin();
   }
   
   Word(const Word& source) :
      Match(source),
      _word(source._word)
   {
      _index = _word.cbegin();
   }
   
   virtual optional<bool> match(int character) {
   
      bool matched = match_char(character);
         
      if (matched)
      {
         Match::match(character);
         ++_index;
         
         if (_index == _word.end())
            set_success(true);
      }
      else
         set_success(false);
       
      return _success;
   }
   
   virtual void write(ostream& out) const {
      out << "Word(\"" << _word << "\")";
   }
   
   virtual const string& word() const {
      return _word;
   }
   
   virtual Match* copy() {
      Word* copy = new Word(*this);
      return copy;
   }
   
};


};

#endif
