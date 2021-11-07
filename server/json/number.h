#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"

namespace bee::fish::json {
      
   class _Number: public Capture
   {
   public:
      Match* _integerCharacter =
         new Range('0', '9');
   
      Label Integer =
         Label(
            "Integer",
            new Repeat(_integerCharacter)
         );
   
      Match* _fractionInteger = Integer.copy();
      
      Match* _fraction = new And(
         new bee::fish::parser::Character('.'),
         _fractionInteger
      );
         
      bee::fish::parser::Character
         Plus =
            bee::fish::parser::Character('+');
      bee::fish::parser::Character
         Minus =
            bee::fish::parser::Character('-');
   
      Or Sign = Or(Plus.copy(), Minus.copy());
      
      Match* _numberSign = Minus.copy();
      Match* _exponentSign = Sign.copy();
      
      Match* _integer = Integer.copy();
      Match* _exponentInteger = Integer.copy();
      
      Match* _exponent = new And(
         new Or(
            new bee::fish::parser::
               Character('E'),
            new bee::fish::parser::
               Character('e')
         ),
         new Optional(_exponentSign),
         _exponentInteger
      );
      
      Match* _number =
         new And(
            new Optional(_numberSign),
            _integer,
            new Optional(_fraction),
            new Optional(_exponent)
         );

   public:
      _Number() : Capture()
      {
          _match = _number;
      }
      
      _Number(const _Number& source) :
         Capture()
      {
         _match = _number;
      }
      
      virtual Match* copy() const
      {
         return new _Number(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {

         if (matched())
         {
            out << tabs(tabIndex);
            
            if (_numberSign->matched())
               out << "-";
            
            out << _integer->value();
            
            if (_fraction->matched())
               out << "." 
                   << _fractionInteger->value();
            
            if (_exponent->matched())
               out << "e"
                   << _exponentSign->value()
                   << _exponentInteger->value();
            
         }
         else
            Match::write(out, tabIndex);
      }
      
   };
   
   const Label Number = Label("Number", new _Number());
   
}

#endif