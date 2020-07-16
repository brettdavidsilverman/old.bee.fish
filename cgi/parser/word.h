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
   
   virtual bool match(int character) {
   
      bool matched = match_char(character);

      if (matched)
      {
         Match::match(character);
         ++_index;
         
         if (_index == _word.end())
            set_success(true);
      }
      else {
         set_success(false);
      }
       
      return matched;
   }
   
   friend ostream& operator <<
   (ostream& out, const Word&  match)
   {
      out << "Word(\"" << match._word << "\")";
      out << (Match&)(match);
      
      return out;
   }
   
   virtual const string& word() const {
      return _word;
   }
   
   virtual Match* copy() const {
      Word* copy = new Word(*this);
      return copy;
   }
   
};


};

#endif
