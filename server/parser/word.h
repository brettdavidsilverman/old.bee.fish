#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include "b-string.h"

namespace bee::fish::parser {

using namespace std;
            
   class Word : public Match {
   protected:
   
      BString _word;
      BString::const_iterator _index;
   
      virtual bool matchChar(Char character)
      {
         if (character == Match::EndOfFile)
            return false;
         return
            (
               (*_index) == character
            );
      }
   
   public:

      Word(BString word) :
         Match(),
         _word(word)
      {
         _index = _word.cbegin();
      }
     
      Word(const Word& source) :
         Word(source._word)
      {
      }
			   
      virtual bool match(Char character)
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
   
      virtual const BString& word() const
      {
         return _word;
      }
   
      virtual Match* copy() const
      {
         return new Word(*this);
      }
      
      virtual void write(ostream& out) const
      {
         write(out, "Word");
      }
   
   protected:
   
      virtual void write(
         ostream& out,
         const string& name
      ) const
      {
         
         out << name;
         
         writeResult(out);
         
         out << "(\"";
         
         BString::writeEscaped(out, _word);
         
         out << "\")";
      }
   
   };


};

#endif
