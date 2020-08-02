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
   
   virtual ~Character() {
   }
   
   virtual bool
   match(int character)
   {
      bool matched =
         (_character == character);
         
      if (matched) {
         Match::match(character);
         setSuccess(true);
      }
      else {
         setSuccess(false);
      }
      
      return matched;
   }
   
   virtual void write(ostream& out)
   {
      out << "Char(";
      Match::write(out);
      out << ":'";
      Match::write(out, _character);
      out << "'";
      out << ")";
   }
   

};

template<char T>
class Char : public Character {
public:
   Char() : Character(T)
   {}
};


};

#endif
