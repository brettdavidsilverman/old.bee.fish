#ifndef BEE_FISH__JSON__TEST_H
#define BEE_FISH__JSON__TEST_H

#include "../parser/test.h"
#include "json.h"


using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   
   inline bool test();

   inline bool testStrings();
   inline bool testNumbers();
   inline bool testIntrinsics();
   inline bool testSets();
   inline bool testArrays();
   inline bool testObjects();
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= bee::fish::parser::test();
      if (!ok)
         return false;
         
      ok &= testIntrinsics();
      ok &= testNumbers();
      ok &= testSets();
      ok &= testArrays();
      ok &= testStrings();
      
  
      //ok &= testObjects();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testIntrinsics()
   {
      cout << "Intrinsics" << endl;
      
      bool ok = true;
      
      _JSON parser;
      
      ok &= test("True", parser, true, "true");
      ok &= test("False", parser, true, "false");
      ok &= test("Null", parser, true, "null");
      ok &= test("False a", parser, false, "a");
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;
      
      _JSON parser;
      
      ok &= test("Integer", parser, true, "800");
      ok &= test("Negative", parser, true, "-800");
      ok &= test("Decimal", parser, true, "800.01");
      ok &= test("Short exponent", parser, true, "800e10");
      ok &= test("Full exponent", parser, true, "800E-10");
      ok &= test("False positive", parser, false, "+800");
      
      cout << endl;
      
      return ok;
   }    
  
   inline bool testSets()
   {
      cout << "Sets" << endl;
      
      bool ok = true;
      
      MatchPtr set = Set(
         Character('{'),
         Word("item"),
         Character(','),
         Character('}')
      );


      ok &= testMatch("Set", set->copy(), "{item,item,item}", true, "{item,item,item}");
      ok &= testMatch("Set empty", set->copy(), "{}", true, "{}");
      ok &= testMatch("Set blanks", set->copy(), "{item, item ,item }", true);

      return ok;
      
      MatchPtr item;
      
      MatchPtr object = new Set(
         Character('{'),
         LoadOnDemand(item),
         Character(','),
         Character('}')
      );
      
      item = Label("item", Word("item"));
      
      ok &= test("Set LoadOnDemand", object, true, "{item,item}");

      class MySet : public Set
      {
      public:
         int _count = 0;
         
         MySet() : Set(
            Character('{'),
            Word("myset"),
            Character(','),
            Character('}')
         )
         {
         }
         
         virtual void matchedSetItem(MatchPtr item)
         {
            if (item->value() == "myset")
               ++_count;
         }
         
      };
     
      MatchPtrBase mySet = make_shared<MySet>();
      
      shared_ptr<MySet> _pointer = std::static_pointer_cast<MySet>(mySet);

      ok &= testMatch("Set with overload", mySet, "{myset,myset}", true, "{myset,myset}");
      
      ok &= displayResult("Set with overload result", (_pointer->_count == 2));

      cout << endl;
      
      return ok;
      
   }
   
   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      const MatchPtr parser = JSON;
      
      ok &= testMatch("Empty array", parser->copy(), "[]", true, "[]");
      ok &= testMatch("Single array", parser->copy(), "[0]", true, "[0]");
      ok &= testMatch("Double array", parser->copy(), "[true,false]", true, "[true,false]");
      ok &= testMatch("Triple array", parser->copy(), "[1,2,3]", true, "[1,2,3]");
      ok &= testMatch("Embedded array", parser->copy(), "[0,[]]", true, "[0,[]]");
      ok &= testMatch("Array with blanks", parser->copy(), "[ 1, true ,null, false]", true,"[ 1, true ,null, false]" );

      return ok;
      
   }
   
   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;
      
      MatchPtr plainCharacter =
         new _PlainCharacter();
         
      ok &= testMatch("Plain character", plainCharacter, "a", true, "a");
      ok &= displayResult("Plain character value", (plainCharacter->character() == 'a'));
      
      MatchPtr hex = new _Hex();
      ok &= testMatch("Hex", hex, "0040", true, "0040");
      ok &= displayResult("Hex value", (hex->character() == '@'));

      
      MatchPtr backSlash =
         new _EscapedCharacter();
      ok &= testMatch("Escaped character back slash", backSlash, "\\\\", true, "\\\\");
      ok &= displayResult("Escaped character back slash value", (backSlash->character() == '\\'));
      
      MatchPtr hexCharacter =
         new _EscapedCharacter();
      ok &= testMatch("Escaped character hex", hexCharacter, "\\u0040", true, "\\u0040");
      ok &= displayResult("Escaped character hex value", (hexCharacter->character() == '@'));

      MatchPtr stringCharacterPlain =
         new _StringCharacter();
      ok &= testMatch("String character plain", stringCharacterPlain, "a", true, "a");
      ok &= displayResult("String character plain value", (stringCharacterPlain->character() == 'a'));

      MatchPtr stringCharacterEscaped =
         new _StringCharacter();
      ok &= testMatch("String character escaped", stringCharacterEscaped,  "\\u0040", true, "\\u0040");
      ok &= displayResult("String character escaped value", (stringCharacterEscaped->character() == '@'));
      
      MatchPtr stringCharacters =
         new _StringCharacters();
      ok &= testMatch("String characters", stringCharacters, "hello world", nullopt, "hello world");
      ok &= displayResult("String characters value", (stringCharacters->value() == "hello world"));
     
      MatchPtr _string = new _String();
      ok &= testMatch("_String", _String(), "\"hello world\"", true, "hello world");

      _JSON parser;
      ok &= testMatch("Empty string", parser, "\"\"", true, "");
      ok &= testMatch("Simple string", parser, "\"hello\"", true, "hello");
      ok &= testMatch("Unquoted", parser, "hello", false);
      ok &= testMatch("Single quote", parser, "\"", nullopt);
      ok &= testMatch("Escaped quote", parser, "\"\\\"\"", true, "\"");
      
      cout << endl;
      
      return ok;
   }
  
   /*
   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;
      
      Match parser = _Object();
      
      ok &= test("Empty object", parser, true, "{}");
      ok &= test("Single field", parser, true, "{\"field\":true}");
      ok &= test("Double fields",parser, true, "{\"a\":1,\"b\":2}");
      ok &= test("Triple object",parser, true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= test("Embedded object", parser, true, "{\"obj\":{\"embedded\":true}}");
      ok &= test("Object with blanks", parser, true, "{ \"field\" : \"string value\" }");

      return ok;
      
   }
   */

      
}

#endif
