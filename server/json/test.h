#ifndef BEE_FISH__JSON__TEST_H
#define BEE_FISH__JSON__TEST_H

#include "../test/test.h"
#include "../parser/test.h"
#include "json.h"
#include "json-parser.h"

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
#ifdef SERVER
   inline bool testStreams();
#endif

   inline bool testEmojis();

   inline bool test()
   {
      
      JSONParser::clear();

      bool ok = true;

      try {
         ok &= testIntrinsics();
         ok &= testNumbers();
         ok &= testSets();
         ok &= testArrays();
         ok &= testStrings();
         ok &= testObjects();
#ifdef SERVER
         ok &= testStreams();
#endif
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

   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;

      PlainCharacter plainCharacter;
      ok &= testMatch("Plain character", &plainCharacter, "a", true);
      ok &= testResult("Plain character value", (plainCharacter.character() == Char('a')));

      Hex hex;
      ok &= testMatch("Hex", &hex, "0040", true, "0040");
      ok &= testResult("Hex value", (hex.character() == Char('@')));
      
      EscapedCharacter backSlash;
      ok &= testMatch("Escaped character back slash", &backSlash, "\\\\", true);
      ok &= testResult("Escaped character back slash value", (backSlash.character() == Char('\\')));
        
      EscapedCharacter hexCharacter;
      ok &= testMatch("Escaped character hex", &hexCharacter, "\\u0040", true);
      ok &= testResult("Escaped character hex value", (hexCharacter.character() == Char('@')));

      StringCharacter stringCharacterPlain;
      ok &= testMatch("String character plain", &stringCharacterPlain, "a", true);
      ok &= testResult("String character plain value", (stringCharacterPlain.character() == Char('a')));

      StringCharacter stringCharacterEscaped;
      ok &= testMatch("String character escaped", &stringCharacterEscaped,  "\\u0040", true);
      ok &= testResult("String character escaped value", (stringCharacterEscaped.character() == Char('@')));
      
      StringCharacters stringCharacters;
      ok &= testMatch("String characters", &stringCharacters, "hello world\\\\", BeeFishMisc::nullopt);
      ok &= testResult("String characters value", (stringCharacters.value() == "hello world\\"));

      ok &= testMatchDelete("String", new String(), "\"hello world\"", true, "hello world");


      ok &= testMatchDelete("Empty string", new JSON(), "\"\"", true, "");
      ok &= testMatchDelete("Simple string", new JSON(), "\"hello\"", true, "hello");
      ok &= testMatchDelete("Unquoted", new JSON(), "hello", false);
      ok &= testMatchDelete("Single quote", new JSON(), "\"", BeeFishMisc::nullopt);
      ok &= testMatchDelete("Escaped quote", new JSON(), "\"\\\"\"", true, "\"");
  
      cout << endl;
      
      return ok;
   }

   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;
      
      
 
      ok &= testMatchDelete("Empty object", new Capture(new JSON()), "{}", true, "{}");
      ok &= testMatchDelete("Single field", new Capture(new JSON()), "{\"field\":true}", true, "{\"field\":true}");
      ok &= testMatchDelete("Double fields", new Capture(new JSON()), "{\"a\":1,\"b\":2}", true, "{\"a\":1,\"b\":2}");
      ok &= testMatchDelete("Triple object", new Capture(new JSON()), "{\"a\":1,\"b\":2,\"c\":[]}", true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= testMatchDelete("Embedded object", new Capture(new JSON()), "{\"obj\":{\"embedded\":true}}", true, "{\"obj\":{\"embedded\":true}}");
      ok &= testMatchDelete("Object with blanks", new Capture(new JSON()), "{ \"field\" : \"string value\" }", true, "{ \"field\" : \"string value\" }");
      
      Object* test = new Object();
      bool hit = false;
      BString _key;
      BString value;
      JSONParser::_onvalues["hello"] = [&hit, &_key, &value](const BString& key, JSON& json) {
         hit = true;
         _key = key;
         value = json.value();
      };
      Capture* capture = new Capture(test);
      ok &= testMatch("Object field", capture, "{\"hello\":1}", true, "{\"hello\":1}");
      ok &= testResult("Object field key hit", hit);
      ok &= testResult("Object key value", (_key == "hello"));
      ok &= testResult("Object field value", (value == "1"));
      delete capture;

      class ObjectTest : public Object {
      protected:
         BString _value;
      public:
         ObjectTest() : Object(
         )
         {
            JSONParser::invokeValue(
               "key",
               [this](const BString& key, JSON& json) 
               {
                  _value = json.value();
               }
            );
         }

         virtual const BString& value() const {
            return _value;
         }
      } ;

      JSONParser::clear();

      ObjectTest* objectTest = new ObjectTest();

      ok &= testMatch("Object test constructor", capture = new Capture(objectTest), "{\"key\":\"hello world\"}", true, "{\"key\":\"hello world\"}");
      ok &= testResult("Object test value", objectTest->value() == "hello world");
      delete capture;

      cout << endl;
      
      return ok;
      
   }

#ifdef SERVER
   inline bool testStreams() 
   {
      
      cout << "Streams" << endl;
      
      bool ok = true;

      JSONParser::clear();

      BeeFishJSON::JSON jsonImage;

      BString last;
      JSONParser::streamValue("image",
         [&last](const BString& buffer) {
            last = buffer;
            cerr << buffer;
         }
      );

      bool secretOk = false;
      JSONParser::invokeValue("secret",
         [&secretOk](const BString& key, const JSON& json) {
            secretOk = (key == "secret") && (json.value() == "mysecret");
         }
      );

      ok &= testFile(
         "JSON with buffered image",
         "server/json/tests/stream.json",
         jsonImage,
         true
      );

      ok &= testResult(
         "JSON with invoke secret",
         true
      );

      cout << endl;
      
      return ok;
   }
#endif

   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      JSON parser1;
      JSON parser2;

      ok &= testMatch("Double unicode", &parser1, "\"\\uD83D\\uDE00\"", true, "😀");
      ok &= testMatch("Emoji 😀", &parser2, "\"😀\"", true, "😀");

      cout << endl;
      
      return ok;
   }
      
}

#endif
