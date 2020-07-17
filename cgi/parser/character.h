#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

class Character : public Match {
private:
   int _character;

public:
   Character(int character) :
      Match(),
      _character(character)
   {
   }
   
   Character(const Character& source) :
      Match(source),
      _character(source._character)
   {
   }

   virtual ~Character() {
   }
   
   virtual bool
   match(int character)
   {
      bool matched =
         (_character == character);
         
      if (matched) {
         Match::match(character);
         set_success(true);
         
      }
      else {
         set_success(false);
      }
      
      return matched;
   }
   
   virtual void write(ostream& out) const {
      out << "Char('";
      switch (_character) {
      case '\r':
         out << "\\r";
         break;
      case '\n':
         out << "\\n";
         break;
      case '\t':
         out << "\\t";
         break;
      case Match::eof:
         out << "-1";
         break;
      default:
          out << (char)_character;
      }
      out << "'";
      Match::write(out);
      out << ")";
   }
   
   virtual Match* copy() const {
      Character* copy =
         new Character(*this);
      return copy;
   }
   
};


};

#endif
