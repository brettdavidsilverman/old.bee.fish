#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>
#include "parser.h"
#include "../test/test.h"

using namespace std;

using namespace bee::fish::test;

namespace bee::fish::parser {
   
   inline bool testMatch(
      BString label,
      Match& match,
      string text,
      bool result = false,
      BString expected = ""
   );
   
   inline bool testMatch(
      BString label,
      Match* match,
      string text,
      bool result = false,
      BString expected = ""
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
   
   extern Word _loadOnDemandItem;
   
   inline bool testLoadOnDemand();
   
   inline Word _loadOnDemandItem = Word("Brett");
   
   inline bool testRules();
   
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
      ok &= testRules();
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
         (_a->_result == true);

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

      ok &= testMatch("Repeat any character match", repeat, "helloworld", true, "helloworld");

      And repeat2 = And(
         new Character('*'),
         new Repeat(new Character('B'), 3, 4),
         new Character('*')
      );
      
      Match* tests[] =
      {
         repeat2.copy(),
         repeat2.copy(),
         repeat2.copy(),
         repeat2.copy()
      };
      
      ok &= testMatch("Repeat", tests[0], "*BBB*", true, "*BBB*");
      ok &= testMatch("Repeat fail 1", tests[1],  "*BB*");
      ok &= testMatch("Repeat fail 2", tests[2], "*BBB", true);
      ok &= testMatch("Repeat fail 3", tests[3], "*BBBBB*");

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
      
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "abz");
      
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
      
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe");
      
      Or _or = Or(
         new Word("Brett"),
         new Word("Silverman")
      );
      
      Match* or1 = _or.copy();
      Match* or2 = _or.copy();
      Match* or3 = _or.copy();
      
      ok &= testMatch("Or first", or1, "Brett", true, "Brett");
      ok &= testMatch("Or second", or2, "Silverman", true, "Silverman");
      ok &= testMatch("Or fail", or3, "Dale");

      return ok;
   }
   
   inline bool testNot()
   {
      bool ok = true;
      
      Not testNot = Not(new Word("ABC"));
      
      ok &= testMatch("Simple 'not' match", testNot, "abc", true);
      
      Match* testNotNoMatch = testNot.copy();
      
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "ABC", false);
     
      
      Not _not1 (
         new Range('a', 'z')
      );

      ok &= testMatch("Not range match", _not1, "A", true);
      
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
      
      ok &= testMatch("Optional one two match", testOptional12, "onetwo", true, "onetwo");
      
      Match* testOptional1 = testOptional.copy();
      
      ok &= testMatch("Optional one match", testOptional1, "one", true, "one");
      
      And testOptional123 = And(
         new Word("one"),
         Optional2(
            new Word("two"),
            new Word("three")
         )
      );
      
      Match* _testOptional123 = testOptional123.copy();
      
      ok &= testMatch("Optional one two three match", _testOptional123, "onetwothree", true, "onetwothree");
      
      Match* testOptional13 = testOptional123.copy();
      
      ok &= testMatch("Optional one three match", testOptional13, "onethree", true, "onethree");
      
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
      ok &= testResult("Capture simple result", test1.value() == "capture");

      BString value;
      Capture testCapture = Capture(test1.copy(), value);
 
      ok &= testMatch("Capture simple using Capture", testCapture, "capture", true, "capture");
      ok &= testResult("Capture simple using Capture result", value == "capture");
      
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
            _match->_capture = true;
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
      invoke._match->_capture = true;
      
      ok &= testMatch("Invoke", invoke, "invoke", true, "invoke");
      ok &= testResult("Invoke value", invokeValue == "invoke");

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
            _match->_capture = true;
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
      ok &= testResult("Invoke class virtual value", testParser._test == "test");

      return ok;
   }
   
   inline bool testLoadOnDemand()
   {
      bool ok = true;
      
      // Load on demand
      And loadOnDemand = And(
         new LoadOnDemand(_loadOnDemandItem),
         new Word("David")
      );
      loadOnDemand._capture = true;
      ok &= testMatch("Load on demand", loadOnDemand, "BrettDavid", true, "BrettDavid"); 
 
      return ok;
   }
   
   inline bool testRules()
   {
      bool ok = true;
      
      const Character a('a');
      const Character b('b');
      MatchPointerBase _and = a and b;
      ok &= testMatch("Rule and", _and, "ab", true, "ab");
      
      MatchPointer _or = Character('+') or Character('-');
      ok &= testMatch("Rule or", _or, "+", true, "+");
      
      MatchPointer test1 =
         Word("start") and
         Repeat(Character('9').copy()) and
         Word("finish");
      ok &= testMatch("Rule test 1", test1, "start9999finish", true, "start9999finish");
      
      MatchPointer test2 =
         Word("start") and
         Repeat(not Character('9'));
      ok &= testMatch("Rule test 2", test2, "start0123456789", true, "start012345678");
      
      MatchPointer test3 =
         Word("start") and
         ~Word("middle") and
         Word("finish");
      ok &= testMatch("Rule test 3", test3, "startfinish", true, "startfinish");
      
      const MatchPointer optional =
         Word("Candy") and
         ~ Word("Dale") and
         ~ Word("Silverman");
      ok &= testMatch("Optional first", optional->copy(), "CandySilverman", true, "CandySilverman");
      ok &= testMatch("Optional second", optional->copy(), "CandyDaleSilverman", true, "CandyDaleSilverman");
      ok &= testMatch("Optional end", optional->copy(), "CandyDale", true, "CandyDale");
      delete optional;
      
      
      return ok;
   }
   
   inline bool testMisc()
   {
   
      bool ok = true;
      
      Match* c;
      
      Match* _and = new And(
         new Word("a"),
         new Word("b"),
         (c = new Word("c"))
      );
            
      _and->_capture = true;
      c->_capture = true;
      
      ok &= testMatch("And",
         *_and,
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

         
      
      */
      
      // Multipart
      Capture multipart(new Word("Brett"));
      multipart.read("Br");
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
      bool result,
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
         
      ok = (result == parser.matched());

      if (parser.matched() && expected.size())
      {
         if (value != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL       " << parser._result << endl;
         cout << "\tFlag     " << parser._capture << endl;
         cout << "\tTested   " << text << endl;
         cout << "\tExpected " << expected << endl;
         cout << "\tCaptured " << value << endl;
#ifdef DEBUG
         cout << "\t"           << parser << endl;
#endif
      }
      
      return ok;
   }
   
   inline bool testMatch(
      BString label,
      Match* match,
      string text,
      bool result,
      BString expected
   )
   {
      //Capture capture = Capture(match);
     // match->getMatch()->_capture = true;
      
      bool ok = testMatch(label, *match, text, result, expected);
      
      delete match;
      
      return ok;
   }
   
   
}

#endif
