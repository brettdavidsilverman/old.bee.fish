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
   
   virtual bool match
   ( 
      int character,
      optional<bool>& success
   )
   {
      bool matched =
         (_character == character);
         
      if (matched) {
         success = true;
         Match::match(character, success);
         onsuccess();
      }
      else {
         success = false;
         onfail();
      }
      
      return matched;
   }
   
   virtual string name()
   {
      ostringstream out;
      out << "\"Char";
      Match::write(out, _character);
      out << "\"";
      return out.str();
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
