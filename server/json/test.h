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
      
      ok &= bee::fish::parser::test();
      if (!ok)
         return false;
         
      ok &= testStrings();
      ok &= testNumbers();
      ok &= testIntrinsics();
      ok &= testSets();
      ok &= testArrays();
      ok &= testObjects();
      
      if (ok)
         cerr << "SUCCESS" << endl;
      else
         cerr << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testStrings()
   {
      cerr << "Strings" << endl;
      
      bool ok = true;
      
      Match parser = JSON;
      
      ok &= test("Empty string", parser, true, "\"\"");
      ok &= test("Simple string", parser, true, "\"hello\"");
      ok &= test("Unquoted", parser, false, "hello");
      ok &= test("Single quote", parser, false, "\"");
      ok &= test("Escaped quote", parser, true, "\"\\\"\"");
      //ok &= test("Unicode hex", parser, true, "\\u0040");

      cerr << endl;
      
      return ok;
   }
   
   inline bool testNumbers()
   {
      cerr << "Numbers" << endl;
      
      bool ok = true;
      
      Match parser = JSON;
      
      ok &= test("Integer", parser, true, "800");
      ok &= test("Negative", parser, true, "-800");
      ok &= test("Decimal", parser, true, "800.01");
      ok &= test("Short exponent", parser, true, "800e10");
      ok &= test("Full exponent", parser, true, "800E-10");
      ok &= test("False positive", parser, false, "+800");
      
      cerr << endl;
      
      return ok;
   }
   
   inline bool testIntrinsics()
   {
      cerr << "Intrinsics" << endl;
      
      bool ok = true;
      
      Match parser = JSON;
      
      ok &= test("True", parser, true, "true");
      ok &= test("False", parser, true, "false");
      ok &= test("Null", parser, true, "null");
      ok &= test("False a", parser, false, "a");
      
      cerr << endl;
      
      return ok;
   }
   
   class SetItem : public Capture
   {
   public:
      inline static unsigned int _count;
         
      SetItem() : Capture(Word("item"))
      {
      }
         
      virtual void success()
      {
         Capture::success();
         if (value() == "item")
            ++_count;
         
      }
      
      virtual Match* copy() const
      {
         return new SetItem();
      }
      
   };
      
   inline bool testSets()
   {
      cerr << "Sets" << endl;
      
      bool ok = true;
      
      
      
      SetItem::_count = 0;
      
      const Match set = Set(
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

      const Match item =
         Label("item", Word("item"));
      
      const Match object = Set(
         Character('{'),
         LoadOnDemand(item),
         Character(','),
         Character('}')
      );
      
      ok &= test("LoadOnDemand", object, true, "{item,item}");

      cerr << endl;
      
      return ok;
      
   }
   
   inline bool testArrays()
   {
      cerr << "Arrays" << endl;
      
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
      cerr << "Objects" << endl;
      
      bool ok = true;
      
      Match parser = JSON;
      
      ok &= test("Empty object", parser, true, "{}");
      ok &= test("Single field", parser, true, "{\"field\":true}");
      ok &= test("Double fields",parser, true, "{\"a\":1,\"b\":2}");
      ok &= test("Triple object",parser, true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= test("Embedded object", parser, true, "{\"obj\":{\"embedded\":true}}");
      ok &= test("Object with blanks", parser, true, "{ \"field\" : \"string value\" }");

      return ok;
      
   }
   

      
}

#endif