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
      ok &= testArrays();
      ok &= testStrings();
      ok &= testObjects();
      ok &= testEmojis();
      
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

      ok &= testMatch("True", parser.copy(), "true", true, "true");
      ok &= testMatch("False", parser.copy(), "false", true, "false");
      ok &= testMatch("Null", parser.copy(), "null", true, "null");
      ok &= testMatch("False a", parser.copy(), "a");
      
      cout << endl;
      
      return ok;
   }
   
   
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;
      
      _JSON parser;
      
      ok &= testMatch("Integer", parser.copy(), "800", true, "800");
      ok &= testMatch("Negative", parser.copy(), "-800", true, "-800");
      ok &= testMatch("Decimal", parser.copy(), "800.01", true, "800.01");
      ok &= testMatch("Short exponent", parser.copy(), "800e10", true, "800e10");
      ok &= testMatch("Full exponent", parser.copy(), "800E-10", true, "800E-10");
      ok &= testMatch("False positive", parser.copy(), "+800");
      
      cout << endl;
      
      return ok;
   }    
  
   inline bool testSets()
   {
      cout << "Sets" << endl;
      
      bool ok = true;
      
      Set set(
         new Character('{'),
         new Word("item"),
         new Character(','),
         new Character('}')
      );


      ok &= testMatch("Set", set.copy(), "{item,item,item}", true, "{item,item,item}");
      ok &= testMatch("Set empty", set.copy(), "{}", true, "{}");
      ok &= testMatch("Set blanks", set.copy(), "{item, item ,item }", true);

      Word item = Word("item");
      
      Set object(
         new Character('{'),
         new LoadOnDemand(item),
         new Character(','),
         new Character('}')
      );
      

      ok &= testMatch("Set LoadOnDemand", object.copy(), "{item,item}", true, "{item,item}");

      class MySet : public Set
      {
      public:
         int _count = 0;
         
         MySet() : Set(
            new Character('{'),
            new Capture(new Word("myset")),
            new Character(','),
            new Character('}')
         )
         {
         }
         
         virtual void matchedSetItem(Match* item)
         {
            if (item->value() == "myset")
               ++_count;
         }
         
      };
     
      MySet mySet;
      
      ok &= testMatch("Set with overload", mySet, "{myset,myset}", true);
      ok &= testResult("Set with overload result", (mySet._count == 2));

      cout << endl;
      
      return ok;
      
   }
   
   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      ok &= testMatch("Empty array", JSON.copy(), "[]", true, "[]");
      ok &= testMatch("Single array", JSON.copy(), "[0]", true, "[0]");
      ok &= testMatch("Double array", JSON.copy(), "[true,false]", true, "[true,false]");
      ok &= testMatch("Triple array", JSON.copy(), "[1,2,3]", true, "[1,2,3]");
      ok &= testMatch("Embedded array", JSON.copy(), "[0,[]]", true, "[0,[]]");
      ok &= testMatch("Array with blanks", JSON.copy(), "[ 1, true ,null, false]", true,"[ 1, true ,null, false]" );

      cout << endl;
      
      return ok;
      
   }
   
   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;

      _PlainCharacter plainCharacter;
      ok &= testMatch("Plain character", plainCharacter, "a", true);
      ok &= testResult("Plain character value", (plainCharacter.character() == 'a'));
      
      _Hex hex;
      ok &= testMatch("Hex", hex, "0040", true, "0040");
      ok &= testResult("Hex value", (hex.character() == '@'));
      
      
      _EscapedCharacter backSlash;
      ok &= testMatch("Escaped character back slash", backSlash, "\\\\", true);
      ok &= testResult("Escaped character back slash value", (backSlash.character() == '\\'));
      
      _EscapedCharacter hexCharacter;
      ok &= testMatch("Escaped character hex", hexCharacter, "\\u0040", true);
      ok &= testResult("Escaped character hex value", (hexCharacter.character() == '@'));

      _StringCharacter stringCharacterPlain;
      ok &= testMatch("String character plain", stringCharacterPlain, "a", true);
      ok &= testResult("String character plain value", (stringCharacterPlain.character() == 'a'));

      _StringCharacter stringCharacterEscaped;
      ok &= testMatch("String character escaped", stringCharacterEscaped,  "\\u0040", true);
      ok &= testResult("String character escaped value", (stringCharacterEscaped.character() == '@'));
      
      _StringCharacters stringCharacters;
      ok &= testMatch("String characters", stringCharacters, "hello world", true);
      ok &= testResult("String characters value", (stringCharacters.value() == "hello world"));
     
      _String _string;
      _string._capture = true;
      ok &= testMatch("_String", _string, "\"hello world\"", true, "hello world");


      _JSON parser;
      ok &= testMatch("Empty string", parser.copy(), "\"\"", true, "");
      ok &= testMatch("Simple string", parser.copy(), "\"hello\"", true, "hello");
      ok &= testMatch("Unquoted", parser.copy(), "hello", false);
      ok &= testMatch("Single quote", parser.copy(), "\"", true);
      ok &= testMatch("Escaped quote", parser.copy(), "\"\\\"\"", true, "\"");
      
      cout << endl;
      
      return ok;
   }

   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;
      
      const Match& parser = JSON;
 
      ok &= testMatch("Empty object", parser.copy(), "{}", true, "{}");
      ok &= testMatch("Single field", parser.copy(), "{\"field\":true}", true, "{\"field\":true}");
      ok &= testMatch("Double fields", parser.copy(), "{\"a\":1,\"b\":2}", true, "{\"a\":1,\"b\":2}");
      ok &= testMatch("Triple object", parser.copy(), "{\"a\":1,\"b\":2,\"c\":[]}", true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= testMatch("Embedded object", parser.copy(), "{\"obj\":{\"embedded\":true}}", true, "{\"obj\":{\"embedded\":true}}");
      ok &= testMatch("Object with blanks", parser.copy(), "{ \"field\" : \"string value\" }", true, "{ \"field\" : \"string value\" }");


      _Object* test = new _Object();
      
      ok &= testMatch("Object field", *test, "{\"hello\":1}", true, "{\"hello\":1}");
      ok &= testResult("Object field contains", test->contains("hello"));

      if (ok)
      {
         ok &= testResult("Object field value", (*test)["hello"]->value() == "1");
      }
      
      delete test;
      
      Match* testJSON = JSON.copy();
      
      ok &= testMatch("Object field string value", *testJSON, "{\"hello\":\"world\"}", true, "{\"hello\":\"world\"}");
      _Object* item =
         (_Object*)(testJSON->item());
      
      ok &= testResult("Object field contains 2", item->contains("hello"));

      if (ok)
      {
         ok &= testResult("Object field string value", (*item)["hello"]->value() == "world");
      }
      
      delete testJSON;

      cout << endl;
      
      return ok;
      
   }

   
   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      _JSON parser1;
      _JSON parser2;
      ok &= testMatch("Double unicode", parser1, "\"\\uD83D\\uDE00\"", true, "😀");
      ok &= testMatch("Emoji 😀", parser2, "\"😀\"", true, "😀");
      cout << endl;
      
      return ok;
   }

      
}

#endif
