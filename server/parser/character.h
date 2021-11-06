#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"
#include "utf-8.h"
#include "../b-string/character.h"

namespace bee::fish::parser {

   class Character : public Match {
   protected:
      bee::fish::b_string::Character _character;
      bee::fish::parser::UTF8Character _utf8Character;

      bool _any;
      
   public:
      Character() :
         _character(-1),
         _any(true)
      {
      }
      
      Character(const bee::fish::b_string::Character& character) :
         _character(character),
         _any(false)
      {
      }
      
      Character(const bee::fish::b_string::Character::Value& character) :
         _character(character),
         _any(false)
      {
      }

      Character(const Character& source) :
         _character(source._character),
         _utf8Character(source._utf8Character),
         _any(source._any)
      {
      }

      virtual bool match(const char& character)
      {
         bool matched = _utf8Character.match(character);

         if (matched && _utf8Character.result() == true) 
            matched =
               ( _any ||
                  ( _character == character )
               );
         
         if (matched)
         {
            capture(*this);
            success();
         }
         else
         {
            fail();
         }
      
         return matched;
      }

      operator bee::fish::b_string::Character::Value () const {
         return (bee::fish::b_string::Character::Value)_character;
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

typedef bee::fish::parser::Character Char;

#endif
