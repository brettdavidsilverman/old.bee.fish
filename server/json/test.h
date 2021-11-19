#ifndef BEE_FISH__JSON__TEST_H
#define BEE_FISH__JSON__TEST_H

#include "../parser/test.h"
#include "json.h"


using namespace BeeFishParser;

namespace BeeFishJSON
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

      try {
         ok &= testIntrinsics();
         ok &= testNumbers();
         ok &= testSets();
         ok &= testArrays();
         
         ok &= testStrings();
         ok &= testObjects();
      
         ok &= testEmojis();
      }      
      catch (std::exception& ex) {
         cout << "Exception: " << ex.what() << std::endl;
         ok = false;
      }
      catch(...) {
         cout << "Unkown Exception" << std::endl;
         ok = false;
      }

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
         BeeFishParser::Character('*')
      );
     
      ok &= testMatchDelete("Capture",  new _Number(), "80000", BeeFishMisc::nullopt, "80000");
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
      
      Capture* set = new Capture(
         new Set(
            new BeeFishParser::Character('{'),
            new Word("item"),
            new BeeFishParser::Character(','),
            new BeeFishParser::Character('}'),
            false
         )
      );
 
      ok &= testMatchDelete("Set", set->copy(), "{item,item,item}", true, "{item,item,item}");
      ok &= testMatchDelete("Set empty", set->copy(), "{}", true, "{}");
      ok &= testMatchDelete("Set blanks", set->copy(), "{item, item ,item }", true);

      delete set;

      Word* item = new Word("item");
      
      Capture object(
         new Set(
            new BeeFishParser::Character('{'),
            new LoadOnDemand(item),
            new BeeFishParser::Character(','),
            new BeeFishParser::Character('}'),
            true
         )
      );
      

      ok &= testMatch("Set LoadOnDemand", &object, "{item,item}", true, "{item,item}");

      class MySet : public Set
      {
      public:
         int _count = 0;
         
         MySet() : Set(
            new BeeFishParser::
               Character('{'),
               
            new Capture(new Word("myset")),
            
            new BeeFishParser::
               Character(','),
               
            new BeeFishParser::
               Character('}'),
            
            true
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
     
      MySet* mySet = new MySet();
      ok &= testMatch("Set with overload", mySet, "{myset,myset}", true);
      ok &= testResult("Set with overload result", (mySet->_count == 2));
      delete mySet;
      
      cout << endl;
      
      return ok;
      
   }
   
   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      Capture json(new _JSON());
      
      ok &= testMatchDelete("Empty array", json.copy(), "[]", true, "[]");
      ok &= testMatchDelete("Single array", json.copy(), "[0]", true, "[0]");
      ok &= testMatchDelete("Double array", json.copy(), "[true,false]", true, "[true,false]");
      ok &= testMatchDelete("Triple array", json.copy(), "[1,2,3]", true, "[1,2,3]");
      ok &= testMatchDelete("Embedded array", json.copy(), "[0,[]]", true, "[0,[]]");
      ok &= testMatchDelete("Array with blanks", json.copy(), "[ 1, true ,null, false]", true,"[ 1, true ,null, false]" );

      cout << endl;
      
      return ok;
      
   }
   
   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;

      _PlainCharacter plainCharacter;
      ok &= testMatch("Plain character", &plainCharacter, "a", true);
      ok &= testResult("Plain character value", (plainCharacter.character() == Char('a')));
      
      Capture hex(new _Hex());
      ok &= testMatch("Hex", &hex, "0040", true, "0040");
      ok &= testResult("Hex value", (hex._match->character() == Char('@')));
      
      
      _EscapedCharacter backSlash;
      ok &= testMatch("Escaped character back slash", &backSlash, "\\\\", true);
      ok &= testResult("Escaped character back slash value", (backSlash.character() == Char('\\')));
      
      _EscapedCharacter hexCharacter;
      ok &= testMatch("Escaped character hex", &hexCharacter, "\\u0040", true);
      ok &= testResult("Escaped character hex value", (hexCharacter.character() == Char('@')));

      _StringCharacter stringCharacterPlain;
      ok &= testMatch("String character plain", &stringCharacterPlain, "a", true);
      ok &= testResult("String character plain value", (stringCharacterPlain.character() == Char('a')));

      _StringCharacter stringCharacterEscaped;
      ok &= testMatch("String character escaped", &stringCharacterEscaped,  "\\u0040", true);
      ok &= testResult("String character escaped value", (stringCharacterEscaped.character() == Char('@')));
      
      _StringCharacters stringCharacters;
      ok &= testMatch("String characters", &stringCharacters, "hello world", BeeFishMisc::nullopt);
      ok &= testResult("String characters value", (stringCharacters.value() == "hello world"));
     
      _String _string;
      ok &= testMatch("_String", &_string, "\"hello world\"", true, "hello world");


      _JSON parser;
      ok &= testMatchDelete("Empty string", parser.copy(), "\"\"", true, "");
      ok &= testMatchDelete("Simple string", parser.copy(), "\"hello\"", true, "hello");
      ok &= testMatchDelete("Unquoted", parser.copy(), "hello", false);
      ok &= testMatchDelete("Single quote", parser.copy(), "\"", BeeFishMisc::nullopt);
      ok &= testMatchDelete("Escaped quote", parser.copy(), "\"\\\"\"", true, "\"");
      
      cout << endl;
      
      return ok;
   }

   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;
      
      Capture parser(new _JSON());
 
      ok &= testMatchDelete("Empty object", parser.copy(), "{}", true, "{}");
      ok &= testMatchDelete("Single field", parser.copy(), "{\"field\":true}", true, "{\"field\":true}");
      ok &= testMatchDelete("Double fields", parser.copy(), "{\"a\":1,\"b\":2}", true, "{\"a\":1,\"b\":2}");
      ok &= testMatchDelete("Triple object", parser.copy(), "{\"a\":1,\"b\":2,\"c\":[]}", true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= testMatchDelete("Embedded object", parser.copy(), "{\"obj\":{\"embedded\":true}}", true, "{\"obj\":{\"embedded\":true}}");
      ok &= testMatchDelete("Object with blanks", parser.copy(), "{ \"field\" : \"string value\" }", true, "{ \"field\" : \"string value\" }");
      

      _Object* test = new _Object();
      Capture* capture = new Capture(test);
      ok &= testMatch("Object field", capture, "{\"hello\":1}", true, "{\"hello\":1}");
      ok &= testResult("Object field contains", test->contains("hello"));

      if (ok)
      {
         ok &= testResult("Object field value", (*test)["hello"]->value() == "1");
      }
      
      delete capture;
      
      _JSON* json = new _JSON();
      Capture capture2(json);
      
      ok &= testMatch("Object field string value", &capture2, "{\"hello\":\"world\"}", true, "{\"hello\":\"world\"}");
      _Object* object = json->_object;
      
      ok &= testResult("Object field contains 2", object->contains("hello"));

      if (ok)
      {
         ok &= testResult("Object field string value", (*object)["hello"]->value() == "world");
      }

      cout << endl;
      
      return ok;
      
   }


   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      _JSON parser1;
      _JSON parser2;
      ok &= testMatch("Double unicode", &parser1, "\"\\uD83D\\uDE00\"", true, "😀");
      ok &= testMatch("Emoji 😀", &parser2, "\"😀\"", true, "😀");
      cout << endl;
      
      return ok;
   }

      
}

#endif
