#ifndef BEE_FISH__JSON__TEST_H
#define BEE_FISH__JSON__TEST_H

#include "../parser/test.h"
#include "json.h"


using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   inline bool test();


   inline bool testIntrinsics();
   
   inline bool testNumbers();
   inline bool testSets();
   inline bool testArrays();
   inline bool testStrings();
   inline bool testObjects();
   inline bool testEmojis();
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testIntrinsics();
      ok &= testNumbers();
      ok &= testSets();
      /*
      ok &= testArrays();
      ok &= testStrings();
      
      ok &= testObjects();
      ok &= testEmojis();
      */
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
      Capture json(new _JSON());
      
      ok &= testMatchDelete("True", json.copy(), "true", true, "true");

      ok &= testMatchDelete("False", json.copy(), "false", true, "false");
      
      ok &= testMatchDelete("Null", json.copy(), "null", true, "null");

      ok &= testMatchDelete("False a", json.copy(), "a");
      
      cout << endl;
      
      return ok;
   }
  
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;
      
      _Number parser;
      
      Capture number(
         parser and
         bee::fish::parser::Character('*')
      );
     
      ok &= testMatchDelete("Integer", number.copy(), "800*", true, "800*");
      ok &= testMatchDelete("Negative", number.copy(), "-800*", true, "-800*");
      ok &= testMatchDelete("Decimal", number.copy(), "800.01*", true, "800.01*");
      ok &= testMatchDelete("Short exponent", number.copy(), "800e10*", true, "800e10*");
      ok &= testMatchDelete("Full exponent", number.copy(), "800E-10*", true, "800E-10*");
      ok &= testMatchDelete("False positive", number.copy(), "+800*");
      
      cout << endl;
      
      return ok;
   }    
  
   inline bool testSets()
   {
      cout << "Sets" << endl;
      
      bool ok = true;
      
      Capture set(
         new Set(
            true,
            new bee::fish::parser::Character('{'),
            new Word("item"),
            new bee::fish::parser::Character(','),
            new bee::fish::parser::Character('}')
         )
      );
 
      ok &= testMatchDelete("Set", set.copy(), "{item,item,item}", true, "{item,item,item}");
      ok &= testMatchDelete("Set empty", set.copy(), "{}", true, "{}");
      ok &= testMatchDelete("Set blanks", set.copy(), "{item, item ,item }", true);

      Word item = Word("item");
      
      Capture object(
         new Set(
            true,
            new bee::fish::parser::Character('{'),
            new LoadOnDemand(item),
            new bee::fish::parser::Character(','),
            new bee::fish::parser::Character('}')
         )
      );
      

      ok &= testMatchDelete("Set LoadOnDemand", object.copy(), "{item,item}", true, "{item,item}");

      class MySet : public Set
      {
      public:
         int _count = 0;
         
         MySet() : Set(
            true,
            new bee::fish::parser::
               Character('{'),
               
            new Capture(new Word("myset")),
            
            new bee::fish::parser::
               Character(','),
               
            new bee::fish::parser::
               Character('}')
         )
         {
         }
         
         virtual void matchedSetItem(Match* item)
         {
            if (item->value() == "myset")
               ++_count;
               
            Set::matchedSetItem(item);
         }
         
      };
     
      MySet mySet;
      ok &= testMatch("Set with overload", &mySet, "{myset,myset}", true);
      ok &= testResult("Set with overload result", (mySet._count == 2));

      cout << endl;
      
      return ok;
      
   }
   
   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      ok &= testMatchDelete("Empty array", JSON.copy(), "[]", true, "[]");
      ok &= testMatchDelete("Single array", JSON.copy(), "[0]", true, "[0]");
      ok &= testMatchDelete("Double array", JSON.copy(), "[true,false]", true, "[true,false]");
      ok &= testMatchDelete("Triple array", JSON.copy(), "[1,2,3]", true, "[1,2,3]");
      ok &= testMatchDelete("Embedded array", JSON.copy(), "[0,[]]", true, "[0,[]]");
      ok &= testMatchDelete("Array with blanks", JSON.copy(), "[ 1, true ,null, false]", true,"[ 1, true ,null, false]" );

      cout << endl;
      
      return ok;
      
   }
   
   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;

      _PlainCharacter plainCharacter;
      ok &= testMatch("Plain character", &plainCharacter, "a", true);
      ok &= testResult("Plain character value", (plainCharacter.character() == 'a'));
      
      _Hex hex;
      ok &= testMatch("Hex", &hex, "0040", true, "0040");
      ok &= testResult("Hex value", (hex.character() == '@'));
      
      
      _EscapedCharacter backSlash;
      ok &= testMatch("Escaped character back slash", &backSlash, "\\\\", true);
      ok &= testResult("Escaped character back slash value", (backSlash.character() == '\\'));
      
      _EscapedCharacter hexCharacter;
      ok &= testMatch("Escaped character hex", &hexCharacter, "\\u0040", true);
      ok &= testResult("Escaped character hex value", (hexCharacter.character() == '@'));

      _StringCharacter stringCharacterPlain;
      ok &= testMatch("String character plain", &stringCharacterPlain, "a", true);
      ok &= testResult("String character plain value", (stringCharacterPlain.character() == 'a'));

      _StringCharacter stringCharacterEscaped;
      ok &= testMatch("String character escaped", &stringCharacterEscaped,  "\\u0040", true);
      ok &= testResult("String character escaped value", (stringCharacterEscaped.character() == '@'));
      
      _StringCharacters stringCharacters;
      ok &= testMatch("String characters", &stringCharacters, "hello world", nullopt);
      ok &= testResult("String characters value", (stringCharacters.value() == "hello world"));
     
      Capture _string(new _String());
      ok &= testMatch("_String", &_string, "\"hello world\"", true, "hello world");


      Capture parser(new _JSON());
      ok &= testMatchDelete("Empty string", parser.copy(), "\"\"", true, "");
      ok &= testMatchDelete("Simple string", parser.copy(), "\"hello\"", true, "hello");
      ok &= testMatchDelete("Unquoted", parser.copy(), "hello", false);
      ok &= testMatchDelete("Single quote", parser.copy(), "\"", nullopt);
      ok &= testMatchDelete("Escaped quote", parser.copy(), "\"\\\"\"", true, "\"");
      
      cout << endl;
      
      return ok;
   }
/*
   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;
      
      const Match& parser = JSON;
 
      ok &= testMatchDelete("Empty object", parser.copy(), "{}", true, "{}");
      ok &= testMatchDelete("Single field", parser.copy(), "{\"field\":true}", true, "{\"field\":true}");
      ok &= testMatchDelete("Double fields", parser.copy(), "{\"a\":1,\"b\":2}", true, "{\"a\":1,\"b\":2}");
      ok &= testMatchDelete("Triple object", parser.copy(), "{\"a\":1,\"b\":2,\"c\":[]}", true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= testMatchDelete("Embedded object", parser.copy(), "{\"obj\":{\"embedded\":true}}", true, "{\"obj\":{\"embedded\":true}}");
      ok &= testMatchDelete("Object with blanks", parser.copy(), "{ \"field\" : \"string value\" }", true, "{ \"field\" : \"string value\" }");


      _Object* test = new _Object();
      
      ok &= testMatchDelete("Object field", *test, "{\"hello\":1}", true, "{\"hello\":1}");
      ok &= testResult("Object field contains", test->contains("hello"));

      if (ok)
      {
         ok &= testResult("Object field value", (*test)["hello"]->value() == "1");
      }
      
      delete test;
      
      _JSON* testMatchDelete = new _JSON();
      
      ok &= testMatchDelete("Object field string value", *testMatchDelete, "{\"hello\":\"world\"}", true, "{\"hello\":\"world\"}");
      _Object* item =
         testMatchDelete->_object;
      
      ok &= testResult("Object field contains 2", item->contains("hello"));

      if (ok)
      {
         ok &= testResult("Object field string value", (*item)["hello"]->value() == "world");
      }
      
      delete testMatchDelete;

      cout << endl;
      
      return ok;
      
   }

   
   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      _JSON parser1;
      _JSON parser2;
      ok &= testMatchDelete("Double unicode", parser1, "\"\\uD83D\\uDE00\"", true, "😀");
      ok &= testMatchDelete("Emoji 😀", parser2, "\"😀\"", true, "😀");
      cout << endl;
      
      return ok;
   }
*/
      
}

#endif
