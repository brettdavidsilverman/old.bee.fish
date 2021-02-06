#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iomanip>
#include <iostream>       // std::cout, std::hex
#include <string>         // std::string, std::u16string
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8_utf16

#include "../parser/parser.h"

namespace bee::fish::json {
   const Match Quote = Label(
      "Quote",
      Character('\"')
   );

   const Match PlainCharacter =
      not (
         Character('\\') or
         Quote or
         Character('\r') or
         Character('\n') or
         Character(Match::EndOfFile)
      );
   
   const Match Hex =
      Range('0', '9') or
      Range('a', 'f') or
      Range('A', 'F');
      
   const Match UnicodeHex =
      Character('u') and (
         Hex and
         Hex and
         Hex and
         Hex
      );
      
   const Match EscapedCharacter =
      Character('\\') and (
         Character('\\') or
         Character('b') or
         Character('f') or
         Character('r') or
         Character('n') or
         Character('t') or
         Character('\"') or
         UnicodeHex
      );
    
   const Match StringCharacter =
      PlainCharacter or
      EscapedCharacter;
         
   const Match String = Label(
      "String",
      Quote and
      Repeat(StringCharacter, 0) and
      Quote
   );
   
}

#endif