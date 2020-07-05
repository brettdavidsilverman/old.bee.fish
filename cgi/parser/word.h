#include "match.h"
#include <string.h>

namespace Bee::Fish::Parser {

using namespace std;
            
class Word : public Match {
private:

   int _index = 0;
   string _word;

public:
   Word(const string word) {
      _word = word;
   }
   
   virtual bool match(int character)
   {
      bool matched =
         _word[_index] == character;
         
      if (matched) {
         _value += (char)character;
         ++_index;
         if (_index == _word.length())
            setSuccess(true);
      }
      else
         setFailed(true);
  
      return matched;
   }
   
};



};
