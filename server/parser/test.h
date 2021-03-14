#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>
#include "parser.h"

using namespace std;

namespace bee::fish::parser {
   
   inline bool testMatch(
      BString label,
      Match& match,
      string text,
      optional<bool> result = false,
      BString expected = ""
   );
   
   inline bool testResult(
      BString label,
      bool ok
   );
   
   
   inline bool testBasics();
   inline bool testCharacter();
   inline bool testRange();
   inline bool testWord();
   inline bool testCaseInsensitiveWord();
   
   inline bool testRepeat();
   inline bool testAnd();
   inline bool testOr();
   inline bool testNot();
   inline bool testOptional();
   
   inline bool testBString(); 
   
   inline bool testLabel();
   inline bool testCapture();
   inline bool testInvoke();
   
   inline bool testLoadOnDemand();
   
   inline bool testMisc();
   
   
   
   inline bool test() 
   {
    
      bool ok = true;
      
      ok &= testBasics();
      
      ok &= testCharacter();
      ok &= testRange();
      ok &= testWord();
      ok &= testCaseInsensitiveWord();
      
      ok &= testRepeat();
      ok &= testAnd();
      ok &= testOr();
      ok &= testNot();
      ok &= testOptional();
      
      ok &= testBString();
      
      ok &= testLabel();
      ok &= testCapture();
      ok &= testInvoke();
      ok &= testLoadOnDemand();
      ok &= testMisc();
      
      if (ok)
         cout << "SUCCESS";
      else
         cout << "FAIL";
         
      cout << endl;
      
      return ok;
  
   }
   
   inline bool testBasics()
   {
      cout << "Test bootstrap:\t";
      
      Character a('a');
      Match* _a = a.copy();
      
      bool ok =
         _a->read("a") &&
         (_a->result() == true);

      if (ok)
         cout << "ok" << endl;
      else
         cout << "FAIL" << endl;
      
      delete _a;
      
      return ok;
   }
   
   inline bool testCharacter()
   {
      bool ok = true;
      
      class CharA : public Character
      {
      public:
         CharA() : Character('A')
         {
         }
      };
      
      // Character
      CharA characterMatch;
      ok &= testMatch("Character match", characterMatch, "A", true, "A");

      CharA characterNoMatch;
      ok &= testMatch("Character no match", characterNoMatch, "B");
     
      Character any;
      ok &= testMatch("Character any", any, "a", true, "a");
      
      return ok;
   }
   
   inline bool testRange()
   {
      bool ok = true;
      
      // Character
      Range rangeMatch('a', 'z');
      ok &= testMatch("Range match", rangeMatch, "b", true, "b");

      Range rangeNoMatch('a', 'z');
      ok &= testMatch("Range no match", rangeNoMatch, "B");

      return ok;
   }
   
   inline bool testWord()
   {
      bool ok = true;
      
      // Character
      Word wordMatch("Word");
      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");

      Word wordNoMatch("Word");
      ok &= testMatch("Word no match", wordNoMatch, "Wor*");

      return ok;
   }
  
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      CIWord ciWordMatch("ABC");
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");

      CIWord ciWordNoMatch("ABC");
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ");

      return ok;
   }
   
   inline bool testRepeat()
   {
      bool ok = true;
      Repeat repeat(
         new Character()
      );
      ok &= testMatch("Repeat any character match", repeat, "helloworld", nullopt, "helloworld");

      And repeat2 = And(
         new Character('*'),
         new Repeat(new Character('B'), 3, 4),
         new Character('*')
      );
      
      Match* tests[4] =
      {
         repeat2.copy(),
         repeat2.copy(),
         repeat2.copy(),
         repeat2.copy()
      };
      
      ok &= testMatch("Repeat", *tests[0], "*BBB*", true);
      ok &= testMatch("Repeat fail 1", *tests[1],  "*BB*");
      ok &= testMatch("Repeat fail 2", *tests[2], "*BBB");
      ok &= testMatch("Repeat fail 3", *tests[3], "*BBBBB*");

      delete tests[0];
      delete tests[1];
      delete tests[2];
      delete tests[3];
      
      And repeatEmpty = And(
         new Character('*'),
         new Repeat(new Character('B'), 0),
         new Character('*')
      );
      ok &= testMatch("Repeat empty", repeatEmpty, "**", true, "**");

      return ok;
   }
   
   inline bool testAnd()
   {
      bool ok = true;
      
      And testAnd = And(
         new Character('a'),
         new Character('b'),
         new Character('c')
      );
      
      ok &= testMatch("Simple 'and' match", testAnd, "abc", true, "abc");

      Match* testAndNoMatch = testAnd.copy();
      
      ok &= testMatch("Simple 'and' no match", *testAndNoMatch, "abz");
      
      delete testAndNoMatch;
      
      return ok;
   }
   
   inline bool testOr()
   {
      bool ok = true;
      
      Or testOr = Or(
         new Word("true"),
         new Word("false")
      );
      
      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      
      Match* testOrNoMatch =
         testOr.copy();
      
      ok &= testMatch("Simple 'or' no match", *testOrNoMatch, "maybe");
      delete testOrNoMatch;
      
      Or _or = Or(
         new Word("Brett"),
         new Word("Silverman")
      );
      
      Match* or1 = _or.copy();
      Match* or2 = _or.copy();
      Match* or3 = _or.copy();
      
      ok &= testMatch("Or first", *or1, "Brett", true, "Brett");
      ok &= testMatch("Or second", *or2, "Silverman", true, "Silverman");
      ok &= testMatch("Or fail", *or3, "Dale");

      delete or1;
      delete or2;
      delete or3;
      
      return ok;
   }
   
   inline bool testNot()
   {
      bool ok = true;
      
      Not testNot = Not(new Word("ABC"));
      ok &= testMatch("Simple 'not' match", testNot, "abc", true, "a");
      
      Match* testNotNoMatch = testNot.copy();
      
      ok &= testMatch("Simple 'not' no match", *testNotNoMatch, "ABC", false);
      delete testNotNoMatch;
     
      
      Not _not1 (
         new Range('a', 'z')
      );
			  
      ok &= testMatch("Not range match", _not1, "A", true, "A");
      
      Not _not2(
         new Range('a', 'z')
      );
      
      ok &= testMatch("Not range no match", _not2, "a");
 
      return ok;
   }
   
   inline bool testOptional()
   {
   
      bool ok = true;
      
      And testOptional(
         new Word("one"),
         new Optional(new Word("two"))
      );
      
      Match* testOptional12 = testOptional.copy();
      
      ok &= testMatch("Optional one two match", *testOptional12, "onetwo", true, "onetwo");
      delete testOptional12;
      
      Match* testOptional1 = testOptional.copy();
      
      ok &= testMatch("Optional one match", *testOptional1, "one", true, "one");
      delete testOptional1;
      
      And testOptional123 = And(
         new Word("one"),
         Optional2(
            new Word("two"),
            new Word("three")
         )
      );
      
      Match* _testOptional123 = testOptional123.copy();
      
      ok &= testMatch("Optional one two three match", *_testOptional123, "onetwothree", true, "onetwothree");
      delete _testOptional123;
      
      Match* testOptional13 = testOptional123.copy();
      
      ok &= testMatch("Optional one three match", *testOptional13, "onethree", true, "onethree");
      delete testOptional13;
      
      return ok;
      
   }
   
   inline bool testBString()
   {
      bool ok = true;
      Word runes("ᛒᚢᛞᛖ");
     
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 1", runes, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
     
      Word runes2(BString("ᛒᚢᛞᛖ"));

      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 2", runes2, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");

      return ok;
   }
   
   inline bool testLabel() 
   {
      bool ok = true;
      
      // Label
      Label label = Label("A", new Character('A'));
      ok &= testMatch("Label", label, "B", false, "B");
      
      stringstream stream;
      stream << label;
      ok &= testResult("Label stream", "A<false>()" == stream.str());
      
      return ok;
   }
   
   inline bool testCapture()
   {
      bool ok = true;
      
      Word test1 = Word("capture");
      
      ok &= testMatch("Capture simple", test1, "capture", true, "capture");
      ok &= testResult("Capture simple result", "capture" == test1.value());

      BString value;
      Capture testCapture = Capture(test1.copy(), value);
 
      ok &= testMatch("Capture simple using Capture", testCapture, "capture", true, "capture");
      ok &= testResult("Capture simple using Capture result", "capture" == value);
      
      class _Capture : public Match
      {
      
      public:
         BString _name;
         BString _value;
         
         
         _Capture() : Match()
         {
            _match =
               new And(
                  new Capture(
                     new Word("name"),
                     _name
                  ),
                  new Character(' '),
                  new Capture(
                     new Word("value"),
                     _value
                  )
               );
         }
         
         
      };
      
      _Capture capture;
      ok &= testMatch("Capture class", capture, "name value", true, "name value");
      ok &= testResult("Capture class result", (capture._name == "name") && (capture._value == "value"));

      return ok;
   }
   
   inline bool testInvoke()
   {
      bool ok = true;
      
      // Invoke
      BString invokeValue;
      Invoke invoke = Invoke(
         new Word("invoke"),
         [&invokeValue](Match* item)
         {
            invokeValue = item->value();
         } 
      );

      ok &= testMatch("Invoke", invoke, "invoke", true, "invoke");
      ok &= testResult("Invoke value", "invoke" == invokeValue);

      class Test : public Invoke
      {
      public:
         BString _test;
      public:
         Test() : Invoke()
         {
         }
         
         virtual void setup()
         {
            _match = new Word("test");
            
            _function =
               [this](Match* match)
               {
                  this->virtualFunction();
               };
            
            Invoke::setup();
         }
         
         virtual void virtualFunction()
         {
            _test = this->value();
         }
      };

      Test testParser;

      ok &= testMatch("Invoke class virtual", testParser, "test", true, "test");
      ok &= testResult("Invoke class virtual value", "test" == testParser._test);

      return ok;
   }
   
   inline bool testLoadOnDemand()
   {
      bool ok = true;
      
      // Load on demand
      Match* match;
      And loadOnDemand = And(
         new LoadOnDemand(match),
         new Word("David")
      );
      
      match = new Label("Name", new Word("Brett"));
      ok &= testMatch("Load on demand", loadOnDemand, "BrettDavid", true, "BrettDavid"); 
 
      delete match;
      
      return ok;
   }
   
   inline bool testMisc()
   {
   
      bool ok = true;
  
      Match* _and;
      Match* c;
      ok &= testMatch("And",
         *(_and = new And(
            new Word("a"),
            new Word("b"),
            (c = new Word("c"))
         )),
         "abc", true, "abc"
      );
     
      ok &= testResult("And c", c->_value == "c");
      delete _and;
      /*
      ok &= test(
         "And 2",
         Word("Brett") and
         Character(' ') and
         Word("Silverman"),
         true,
         "Brett Silverman"
      );

         
      MatchPtr optional =
         Word("Candy") and
         ~ Word("Dale") and
         ~ Word("Silverman");
      ok &= test("Optional first", optional, true, "CandySilverman");
      ok &= test("Optional second", optional, true, "CandyDaleSilverman");
      ok &= test("Optional end", optional, true, "CandyDale");

      
      */
      
      // Multipart
      Capture multipart(new Word("Brett"));
      multipart.read("Br", false);
      multipart.read("ett");
      
      bool multipartResult =
         multipart.matched();
         
      ok &= testResult("Multipart", multipartResult);
      
      
      return ok;
   
   }
   
   inline bool testMatch(
      BString label,
      Match& match,
      string text,
      optional<bool> result,
      BString expected
   )
   {
      cout << label << ":\t";
      
      bool ok = true;
      Match& parser = match;
      parser._capture = true;
      parser.read(text);
      
      BString value;
      if (parser.matched())
         value = parser.value();
         
      if (result == true && parser._result != true)
         ok = false;
      else if (result == false && parser._result != false)
         ok = false;
      else if (parser._result == true && expected.size())
      {
         if (value != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL "        << parser._result << endl;
         cout << "\tTested\t"   << text << endl;
         cout << "\tExpect\t"   << expected << endl;
         cout << "\tCaptured\t" << parser._value << endl;
         cout << "\tGot\t"      << value << endl;
         cout << "\t"           << parser << endl;
      }
      
      return ok;
   }
   
   inline bool testResult(
      BString label,
      bool ok
   )
   {
      cout << label << ":\t";
      
      string text;
      
      if (ok)
         cout << "ok";
      else
         cout << "FAIL";

      cout << endl;
      
      return ok;
   }
   
}

#endif
