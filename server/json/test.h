#ifndef BEE_FISH_JSON__TEST_H
#define BEE_FISH_JSON__TEST_H

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
      
      //ok &= bee::fish::parser::test();
      if (!ok)
         return false;
         
      ok &= testIntrinsics();
      ok &= testNumbers();
      ok &= testSets();
      /*
      //ok &= testStrings();
      
      ok &= testArrays();
      
      */
      
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
      
      JSON parser;
      
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
      
      JSON parser;
      
      ok &= test("Integer", parser, true, "800");
      ok &= test("Negative", parser, true, "-800");
      ok &= test("Decimal", parser, true, "800.01");
      ok &= test("Short exponent", parser, true, "800e10");
      ok &= test("Full exponent", parser, true, "800E-10");
      ok &= test("False positive", parser, false, "+800");
      
      cout << endl;
      
      return ok;
   }
   

   /*
   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;
      
      _PlainCharacter plainCharacter;
      ok &= test("Plain character", plainCharacter, true, "a");
      ok &= displayResult("Plain character value", (plainCharacter.character() == 'a'));
      
      _Hex hex;
      ok &= test("Hex", hex, true, "0040");
      ok &= displayResult("Hex value", (hex.character() == '@'));

      _EscapedCharacter backSlash;
      ok &= test("Escaped character back slash", backSlash, true, "\\\\");
      ok &= displayResult("Escaped character back slash value", (backSlash.character() == '\\'));
      
      _EscapedCharacter hexCharacter;
      ok &= test("Escaped character hex", hexCharacter, true, "\\u0040");
      ok &= displayResult("Escaped character hex value", (hexCharacter.character() == '@'));

      _StringCharacter stringCharacterPlain;
      ok &= test("String character plain", stringCharacterPlain, true, "a");
      ok &= displayResult("String character plain value", (stringCharacterPlain.character() == 'a'));

      _StringCharacter stringCharacterEscaped;
      ok &= test("String character escaped", stringCharacterEscaped, true, "\\u0040");
      ok &= displayResult("String character escaped value", (stringCharacterEscaped.character() == '@'));
      
      _StringCharacters stringCharacters;
      ok &= test("String characters", stringCharacters, true, "hello world");
      ok &= displayResult("String characters value", (stringCharacters.str() == L"hello world"));
      
      //wstringstream wideStringStream;
      _String _string;
      ok &= test("_String", _string, true, "\"hello world\"");
      ok &= displayResult("_String value", (_string.value() == L"hello world"));
      
      Match parser = JSON;
      ok &= test("Empty string", parser, true, "\"\"");
      ok &= test("Simple string", parser, true, "\"hello\"");
      ok &= test("Unquoted", parser, false, "hello");
      ok &= test("Single quote", parser, false, "\"");
      ok &= test("Escaped quote", parser, true, "\"\\\"\"");
      
      
      cout << endl;
      
      return ok;
   }
      
   */
   
   class SetItem : public Word
   {
   public:
      inline static unsigned int _count;
         
      SetItem() : Word("item")
      {
      }
         
      virtual void success()
      {
         Word::success();
         if (value() == "item")
            ++_count;
         
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<SetItem>();
      }
      
   };
      
   inline bool testSets()
   {
      cout << "Sets" << endl;
      
      bool ok = true;
      
      SetItem::_count = 0;
      
      const MatchPtr set = Set(
         Character('{'),
         SetItem(),
         Character(','),
         Character('}')
      );
 
      ok &= test("Set Item", SetItem(), true, "item");
      SetItem::_count = 0;
      ok &= test("Set", set, true, "{item,item,item}");
      ok &= displayResult("count", (SetItem::_count == 3));
      ok &= test("Set empty", set, true, "{}");
      ok &= test("Set blanks", set, true, "{item, item ,item }");

      MatchPtr item;
      
      const MatchPtr object = Set(
         Character('{'),
         LoadOnDemand(item),
         Character(','),
         Character('}')
      );
      
      item = new Label("item", Word("item"));
      
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
            cerr << item << endl;
            if (item->value() == "myset")
               ++_count;
         }
         
      };
     
      MatchPtrBase mySet = make_shared<MySet>();
      ok &= test("Set with overload", mySet, true, "{myset,myset}");
      shared_ptr<MySet> pointer = std::static_pointer_cast<MySet>(mySet);
      ok &= displayResult("Set with overload result", (pointer->_count == 2));

      cout << endl;
      
      return ok;
      
   }
   /*
   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      Match parser = JSON;
      
      ok &= test("Empty array", parser, true, "[]");
      ok &= test("Single array", parser, true, "[0]");
      ok &= test("Double array",parser, true, "[true,false]");
      ok &= test("Triple array",parser, true, "[1,2,3]");
      ok &= test("Embedded array", parser, true, "[0,[]]");
      ok &= test("Array with blanks", parser, true, "[ 1, true ,null, false]");

      return ok;
      
   }
   
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