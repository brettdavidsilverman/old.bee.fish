#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

   class Character : public Match {
   protected:
      Char _character;
      bool _any;
      
   public:
      Character() :
         _character(-1),
         _any(true)
      {
      }
      
      Character(Char character) :
         _character(character),
         _any(false)
      {
      }
      
      Character(const Character& source) :
         _character(source._character),
         _any(source._any)
      {
      }

      virtual bool match(Char character)
      {
         bool matched =
            (character != BString::EndOfFile) &&
            ( _any ||
             (_character == character)
            );
         
         if (matched)
         {
            capture(character);
            success();
         }
         else
         {
            fail();
         }
      
         return matched;
      }
   
      virtual MatchPtrBase copy() const
      {
         return make_shared<Character>(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Character";
         
         writeResult(out);
         
         if (_any)
         {
            out << "()";
         }
         else
         {
            out << "('";
            
            BString::writeEscaped(out, _character);
          
            out << "')";
         }
      }
      
   };

};


#endif
