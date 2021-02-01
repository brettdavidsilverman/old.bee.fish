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
   
      virtual bool matchChar(int character)
      {
         if (character == Match::EndOfFile)
            return false;
         return
            (
               (*_index) == (char)character
            );
      }
   
   public:

      Word(const string& word) :
         Match(),
         _word(word)
      {
         _index = _word.cbegin();
      }
     
      Word(const Word& source) :
         Word(source._word)
      {
      }
			   
      virtual bool match(int character)
      {
   
         bool matched = matchChar(character);

         if (matched)
         {
            Match::match(character);
            ++_index;
            if (_index == _word.end())
            {
               success();
            }
         
         }
         else
         {
            fail();
         }
       
         return matched;
      }
   
      virtual string name() const
      {
         return "Word" + _word;
      }
      
   
      virtual const string& word() const
      {
         return _word;
      }
   
      virtual Match* copy() const
      {
         return new Word(*this);
      }
      
      virtual void write(ostream& out) const
      {
         Match::write(out);
         
         out << "(\"";
         
         for (char c : word())
         {
            Match::write(out, c);
         }
            
         out << "\")";
      }
   
   };


};

#endif
