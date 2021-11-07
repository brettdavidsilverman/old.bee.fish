#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace BeeFishParser {

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
      
      Character(const Char& character) :
         _character(character),
         _any(false)
      {
      }
      
      Character(const Character& source) :
         _character(source._character),
         _any(source._any)
      {
      }

      virtual bool match(const Char& character)
      {
         bool matched =
            ( _any ||
              ( _character == character )
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

      virtual Match* copy() const
      {
         return new Character(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) << "Character";
         
         writeResult(out);
         
         if (_any)
         {
            out << "()";
         }
         else
         {
            out << "('";
            
            out << _character;
          
            out << "')";
         }
      }
      
   };

};


#endif
