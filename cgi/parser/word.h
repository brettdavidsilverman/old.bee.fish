#include "match.h"
#include <string.h>

namespace Bee::Fish::Parser {

using namespace std;
            
class Word : public Match {
private:
   size_t _index = 0;
   string _word;
   
public:

   Word(const string& word) {
      _word = word;
   }
   
   virtual bool match(char character) {
   
      bool matched =
         (_word[_index] == character);
         
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
