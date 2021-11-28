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
/*
   inline bool testStrings();
   inline bool testObjects();
   inline bool testEmojis();
  */ 
   inline bool test()
   {
   
      bool ok = true;

      try {
         ok &= testIntrinsics();
         ok &= testNumbers();
         ok &= testSets();
         ok &= testArrays();
         /*
         
         ok &= testStrings();
         ok &= testObjects();
      
         ok &= testEmojis();
         */
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
      
      
      ok &= testMatchDelete("True", new Capture(new JSON()), "true", true, "true");

      ok &= testMatchDelete("False", new Capture(new JSON()), "false", true, "false");
      
      ok &= testMatchDelete("Null", new Capture(new JSON()), "null", true, "null");

      ok &= testMatchDelete("False a", new Capture(new JSON()), "a");
      
      cout << endl;
      
      return ok;
   }
  
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;
      
      class CaptureNumber : public Capture {
      public:
         CaptureNumber() : Capture(
            new And(
               new Number(),
               new BeeFishParser::Character('*')
            )
         ) {

         }
      };
     
      ok &= testMatchDelete("Capture",  new CaptureNumber(), "80000", BeeFishMisc::nullopt, "80000");
      ok &= testMatchDelete("Integer", new CaptureNumber(), "800*", true, "800*");
      ok &= testMatchDelete("Negative", new CaptureNumber(), "-800*", true, "-800*");
      ok &= testMatchDelete("Decimal", new CaptureNumber(), "800.01*", true, "800.01*");
      ok &= testMatchDelete("Short exponent", new CaptureNumber(), "800e10*", true, "800e10*");
      ok &= testMatchDelete("Full exponent", new CaptureNumber(), "800E-10*", true, "800E-10*");
      ok &= testMatchDelete("False positive", new CaptureNumber(), "+800*");
      
      cout << endl;
      
      return ok;
   }    

   inline bool testSets()
   {
      cout << "Sets" << endl;
      
      bool ok = true;
      
      class OpenBrace : public BeeFishParser::Character {
      public:
         OpenBrace() : Character('{') {

         }

      };


      class CloseBrace : public BeeFishParser::Character {
      public:
         CloseBrace() : Character('}') {

         }

      };

      class Item : public Word {
      public:
         Item() : Word("item") {

         }
      };

      class Seperator : public BeeFishParser::Character {
      public:
         Seperator() : Character(',') {

         }
      };

      class _Set : public Capture {
      public:
         _Set() : Capture(
            new Set<OpenBrace, Item, Seperator, CloseBrace>()
         )
         {

         }
      };
 
      ok &= testMatchDelete("Set", new _Set(), "{item,item,item}", true, "{item,item,item}");
      ok &= testMatchDelete("Set empty", new _Set(), "{}", true, "{}");
      ok &= testMatchDelete("Set blanks", new _Set(), "{item, item ,item }", true);


      Capture object(
         new Set<OpenBrace, LoadOnDemand<Item>, Seperator, CloseBrace>()
      );
      

      ok &= testMatch("Set LoadOnDemand", &object, "{item,item}", true, "{item,item}");

      class MySetItem : public Capture {
      public:
         MySetItem() : Capture(
            new Word("myset")
         )
         {

         }
      };

      class MySet : public Set<OpenBrace, MySetItem, Seperator, CloseBrace>
      {
      public:
         int _count = 0;
         
         MySet() : Set()
         {
         }
         
         virtual void matchedSetItem(MySetItem* item)
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
      
      ok &= testMatchDelete("Empty array", new Capture(new JSON()), "[]", true, "[]");
      ok &= testMatchDelete("Single array", new Capture(new JSON()), "[0]", true, "[0]");
      ok &= testMatchDelete("Double array", new Capture(new JSON()), "[true,false]", true, "[true,false]");
      ok &= testMatchDelete("Triple array", new Capture(new JSON()), "[1,2,3]", true, "[1,2,3]");
      ok &= testMatchDelete("Embedded array", new Capture(new JSON()), "[0,[]]", true, "[0,[]]");
      ok &= testMatchDelete("Array with blanks", new Capture(new JSON()), "[ 1, true ,null, false]", true,"[ 1, true ,null, false]" );

      cout << endl;
      
      return ok;
      
   }
   /*
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
      Capture stringCharactersCapture(stringCharacters.copy());
      ok &= testMatch("String characters", &stringCharactersCapture, "hello world", BeeFishMisc::nullopt);
      ok &= testResult("String characters value", (stringCharactersCapture.value() == "hello world"));
     
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

      Capture* parser3 = new Capture(new _JSON());

      const char* content = "{\"wifi\":\"WiFi Name\",\"password\":\"password\"}";
      ok &= testMatch("Wifi", parser3, content, true, content);
      delete parser3;
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
*/
      
}

#endif
