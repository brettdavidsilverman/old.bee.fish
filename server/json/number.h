#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"

namespace bee::fish::json {
      
   class _Number: public Match
   {
   public:
      MatchPtr IntegerCharacter =
         Range('0', '9');
   
      MatchPtr Integer =
         Repeat(IntegerCharacter);
   
      MatchPtr FractionInteger = Integer->copy();
      
      MatchPtr Fraction =
         Character('.') and FractionInteger;
         
      MatchPtr Plus = Character('+');
      MatchPtr Minus = Character('-');
   
      MatchPtr Sign = Plus or Minus;
      
      MatchPtr NumberSign = Minus->copy();
      MatchPtr ExponentSign = Sign->copy();
      
      MatchPtr ExponentInteger = Integer->copy();
      
      MatchPtr Exponent =
         (
            Character('E') or
            Character('e')
         ) and
         ~ExponentSign and
         ExponentInteger;
      
   
   public:
      _Number() : Match()
      {
         setMatch(
            ~NumberSign and
            Integer and
            ~Fraction and
            ~Exponent
         );
      }
      
      _Number(const _Number& source) :
         Match(source)
      {
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<_Number>(*this);
      }
      
      virtual void write(ostream& out) const
      {

         if (matched())
         {
            if (NumberSign->matched())
               out << "-";
            
            out << Integer->value();
            
            if (Fraction->matched())
               out << "." 
                   << FractionInteger->value();
            
            if (Exponent->matched())
               out << "e"
                   << ExponentSign->value()
                   << ExponentInteger->value();
            
         }
         else
            Match::write(out);
      }
      
   };
   
   const Label Number = Label("Number", _Number());
   
}

#endif