#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"

namespace bee::fish::parser {

using namespace std;
            
   class Word : public Match {
   protected:
   
      BString _word;
      BString::const_iterator _index;
   
      virtual bool matchChar(const Char& character)
      {
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
			   
      virtual bool match(const Char& character)
      {
         
         bool matched = matchChar(character);
         
         if (matched)
         {
            capture(character);
            
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
   
      const BString& word() const
      {
         return _word;
      }
   
      virtual Match* copy() const
      {
         return new Word(*this);
      }
      
      virtual void write(
         wostream& out,
         size_t tabIndex = 0
      ) const
      {
         write(out, "Word", tabIndex);
      }
   
   protected:
   
      void write(
         wostream& out,
         const BString& name,
         size_t tabIndex
      ) const
      {
         
         out << tabs(tabIndex) << name;
         
         writeResult(out);
         
         out << "(\"";
         
         _word.writeEscaped(out);
         
         out << "\")";
      }
   
   };


};

#endif
