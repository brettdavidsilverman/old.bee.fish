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
      Match* match,
      string text,
      optional<bool> result = false,
      BString expected = ""
   );
   
   inline bool testMatchDelete(
      BString label,
      Match* match,
      string text,
      optional<bool> result = false,
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
      MatchPointer characterMatch = Capture(new CharA());
      ok &= testMatch("Character match", characterMatch, "A", true, "A");
      delete characterMatch;
      
      MatchPointer characterNoMatch = CharA();
      ok &= testMatch("Character no match", characterNoMatch, "B");
      delete characterNoMatch;
      
      MatchPointer any = Capture(new Character());
      ok &= testMatch("Character any", any, "a", true, "a");
      delete any;
      
      return ok;
   }
   
   inline bool testRange()
   {
      bool ok = true;
      
      // Character
      MatchPointer rangeMatch =
         Capture(new Range('a', 'z'));
      ok &= testMatch("Range match", rangeMatch, "b", true, "b");
      delete rangeMatch;
      
      MatchPointer rangeNoMatch =
         Capture(new Range('a', 'z'));
      ok &= testMatch("Range no match", rangeNoMatch, "B");
      delete rangeNoMatch;
      
      return ok;
   }
   
   inline bool testWord()
   {
      bool ok = true;
      
      // Character
      MatchPointer wordMatch = 
         Capture(
            new Word("Word")
         );
      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");
      delete wordMatch;
      
      MatchPointer wordNoMatch = Word("Word");
      ok &= testMatch("Word no match", wordNoMatch, "Wor*");
      delete wordNoMatch;
      
      return ok;
   }
   
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      MatchPointer ciWordMatch =
         Capture(
            new CIWord("ABC")
         );
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");
      delete ciWordMatch;
      
      MatchPointer ciWordNoMatch = CIWord("ABC");
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ");
      delete ciWordNoMatch;
      
      return ok;
   }
   
   inline bool testRepeat()
   {
      bool ok = true;
      MatchPointer repeat =
         Capture(
            new Repeat(
               new Character()
            )
         );

      ok &= testMatch("Repeat any character match", repeat, "helloworld", nullopt, "helloworld");
      delete repeat;
      
      MatchPointer repeat2 =
         Capture(
            new And(
               new Character('*'),
               new Repeat(
                  new Character('B'), 3, 4
               ),
               new Character('*')
            )
         );
      
      MatchPointer<Match> tests[] =
      {
         repeat2.copy(),
         repeat2.copy(),
         repeat2.copy(),
         repeat2.copy()
      };
      
      ok &= testMatch("Repeat", tests[0], "*BBB*", true, "*BBB*");
      ok &= testMatch("Repeat fail 1", tests[1],  "*BB*");
      ok &= testMatch("Repeat fail 2", tests[2], "*BBB", nullopt);
      ok &= testMatch("Repeat fail 3", tests[3], "*BBBBB*");

      delete repeat2;
      delete tests[0];
      delete tests[1];
      delete tests[2];
      delete tests[3];
      
      MatchPointer repeatEmpty = Capture(
         new And(
            new Character('*'),
            new Repeat(new Character('B'), 0),
            new Character('*')
         )
      );
 
      ok &= testMatch("Repeat empty", repeatEmpty, "**", true, "**");
      delete repeatEmpty;
      return ok;
   }
   
   inline bool testAnd()
   {
      bool ok = true;
      
      MatchPointer testAnd = Capture(
         new And(
            new Character('a'),
            new Character('b'),
            new Character('c')
         )
      );

      ok &= testMatch("Simple 'and' match", testAnd, "abc", true, "abc");
      
      MatchPointer testAndNoMatch = testAnd.copy();
      
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "abz");
      
      delete testAnd;
      delete testAndNoMatch;
      
      return ok;
   }
   
   inline bool testOr()
   {
      bool ok = true;
      
      MatchPointer testOr = Capture(
         new Or(
            new Word("true"),
            new Word("false")
         )
      );

      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      
      MatchPointer testOrNoMatch =
         testOr.copy();
      
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe");
      
      delete testOr;
      delete testOrNoMatch;
      
      MatchPointer _or = Capture(
         new Or(
            new Word("Brett"),
            new Word("Silverman")
         )
      );
      
      MatchPointer or1 = _or.copy();
      MatchPointer or2 = _or.copy();
      MatchPointer or3 = _or.copy();
      
      ok &= testMatch("Or first", or1, "Brett", true, "Brett");
      ok &= testMatch("Or second", or2, "Silverman", true, "Silverman");
      ok &= testMatch("Or fail", or3, "Dale");

      delete _or;
      delete or1;
      delete or2;
      delete or3;
      
      return ok;
   }
   
   inline bool testNot()
   {
      bool ok = true;
      
      MatchPointer testNot =
         Capture(
            new Not(new Word("ABC"))
         );
      
      ok &= testMatch("Simple 'not' match", testNot, "abc", true);
      
      MatchPointer testNotNoMatch = testNot.copy();
      
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "ABC", false);
      delete testNot;
      delete testNotNoMatch;
      
      MatchPointer _not1 = Capture(
         new Not(
            new Range('a', 'z')
         )
      );

      ok &= testMatch("Not range match", _not1, "A", true);
      delete _not1;
      
      MatchPointer _not2 = Not(
         new Range('a', 'z')
      );

      ok &= testMatch("Not range no match", _not2, "a");
 
      delete _not2;
      
      return ok;
   }
   
   inline bool testOptional()
   {
   
      bool ok = true;
      
      MatchPointer testOptional = Capture(
         new And(
            new Word("one"),
            new Optional(new Word("two"))
         )
      );
      
      MatchPointer testOptional12 = testOptional.copy();
      
      ok &= testMatch("Optional one two match", testOptional12, "onetwo", true, "onetwo");
      delete testOptional12;
      
      MatchPointer testOptional1 = testOptional.copy();
      
      ok &= testMatch("Optional one match", testOptional1, "one", nullopt, "one");
      delete testOptional1;
      delete testOptional;
      
      MatchPointer testOptional123 = Capture(
         new And(
            new Word("one"),
            Optional2(
               new Word("two"),
               new Word("three")
            )
         )
      );
      
      MatchPointer _testOptional123 = testOptional123.copy();
      
      ok &= testMatch("Optional one two three match", _testOptional123, "onetwothree", true, "onetwothree");
      delete _testOptional123;
      
      MatchPointer testOptional13 = testOptional123.copy();
      
      ok &= testMatch("Optional one three match", testOptional13, "onethree", true, "onethree");
      delete testOptional13;
      
      delete testOptional123;
      
      return ok;
      
   }
   
   inline bool testBString()
   {
      bool ok = true;
      MatchPointer runes = Capture(
         new Word("ᛒᚢᛞᛖ")
      );
 
      
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 1", runes, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      delete runes;
      
      MatchPointer runes2 = Capture(
         new Word(BString("ᛒᚢᛞᛖ"))
      );
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 2", runes2, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      delete runes2;
      
      return ok;
   }
   
   inline bool testLabel() 
   {
      bool ok = true;
      
      // Label
      MatchPointer label = Capture(
         new Label("A", new Character('A'))
      );
      
      ok &= testMatch("Label", label, "B", false, "B");
      
      stringstream stream;
      stream << *(label->_match);
      ok &= testResult("Label stream", "A<false>()" == stream.str());
      delete label;
      return ok;
   }
   
   inline bool testCapture()
   {
      bool ok = true;
      
      MatchPointer test1 = Capture(
         new Word("capture")
      );
      
      ok &= testMatch("Capture simple", test1, "capture", true, "capture");
      ok &= testResult("Capture simple result", test1->value() == "capture");

      
      delete test1;
      
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
      
      _Capture* placeHolder;
      
      MatchPointer capture = new Capture(
         placeHolder = new _Capture()
      );
      
      ok &= testMatch("Capture class", capture, "name value", true, "name value");
      ok &= testResult("Capture class result", (placeHolder->_name == "name") && (placeHolder->_value == "value"));
      
      delete capture;
      
      return ok;
   }
   
   inline bool testInvoke()
   {
      bool ok = true;
      
      // Invoke
      BString invokeValue;
      MatchPointer invoke = Invoke(
         new Capture(new Word("invoke")),
         [&invokeValue](Match* item)
         {
            invokeValue = item->value();
         } 
      );
      
      ok &= testMatch("Invoke", invoke, "invoke", true);
      ok &= testResult("Invoke value", invokeValue == "invoke");

      delete invoke;
      
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
            _match = new Capture(
                new Word("test")
            );
            _function =
               [this](Match* match)
               {
                  this->virtualFunction();
               };
            
            Invoke::setup();
         }
         
         virtual void virtualFunction()
         {
            _test = _match->value();
         }
      };

      MatchPointer testParser = new Test();

      ok &= testMatch("Invoke class virtual", testParser, "test", true);
      ok &= testResult("Invoke class virtual value", testParser->_test == "test");

      delete testParser;
      return ok;
   }
   
   inline bool testLoadOnDemand()
   {
      bool ok = true;
      
      // Load on demand
      MatchPointer loadOnDemand = Capture(
         new And(
            new LoadOnDemand(_loadOnDemandItem),
            new Word("David")
         )
      );

      ok &= testMatch("Load on demand", loadOnDemand, "BrettDavid", true, "BrettDavid"); 
 
      delete loadOnDemand;
      
      return ok;
   }
   
   inline bool testRules()
   {
      bool ok = true;
      
      const Character a('a');
      const Character b('b');
      MatchPointer _and = Capture(a and b);
      ok &= testMatch("Rule and", _and, "ab", true, "ab");
      delete _and;
      
      MatchPointer _or = Capture(
          Character('+') or Character('-')
      );
      ok &= testMatch("Rule or", _or, "+", true, "+");
      delete _or;
      
      MatchPointer test1 = Capture(
         Word("start") and
         Repeat(Character('9').copy()) and
         Word("finish")
      );
      ok &= testMatch("Rule test 1", test1, "start9999finish", true, "start9999finish");
      delete test1;
      
      MatchPointer test2 = Capture(
         Word("start") and
         Repeat(not Character('9'))
      );
      ok &= testMatch("Rule test 2", test2, "start0123456789", true, "start012345678");
      delete test2;
      
      MatchPointer test3 = Capture(
         Word("start") and
         ~Word("middle") and
         Word("finish")
      );
      
      ok &= testMatch("Rule test 3", test3, "startfinish", true, "startfinish");
      delete test3;
      
      const MatchPointer optional = Capture(
         Word("Candy") and
         ~ Word("Dale") and
         ~ Word("Silverman")
      );
      
      MatchPointer test  = optional->copy();
      ok &= testMatch("Optional first", test, "CandySilverman", true, "CandySilverman");
      delete test;
      
      test = optional->copy();
      ok &= testMatch("Optional second", test, "CandyDaleSilverman", true, "CandyDaleSilverman");
      delete test;
      
      test = optional->copy();
      ok &= testMatch("Optional end", test, "CandyDale", nullopt, "CandyDale");
      delete test;
      
      delete optional;
      
      
      return ok;
   }
   
   inline bool testMisc()
   {
   
      bool ok = true;
      
      Match* c;
      
      MatchPointer _and = new Capture(
         new And(
            new Word("a"),
            new Word("b"),
            (c = new Capture( new Word("c")) )
         )
      );
      
      ok &= testMatch("And",
         _and,
         "abc", true, "abc"
      );
      
      ok &= testResult("And c", c->_value == "c");
      delete _and;
      
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
      Match* parser,
      string text,
      optional<bool> result,
      BString expected
   )
   {
      cout << label << ":\t";
      
      bool ok = true;
      parser->read(text);
      
      BString value;
      if (parser->matched())
         value = parser->value();
         
      ok = (result == parser->_result);

      if (parser->matched() && expected.size())
      {
         if (value != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL       " << parser->_result << endl;
         cout << "\tTested   " << text << endl;
         cout << "\tExpected " << expected << endl;
         cout << "\tCaptured " << value << endl;
#ifdef DEBUG
         cout << "\t"           << *parser << endl;
#endif
      }
      
      return ok;
   }
   
   inline bool testMatchDelete(
      BString label,
      Match* parser,
      string text,
      optional<bool> result,
      BString expected
   )
   {
      bool ok = testMatch(label, parser, text, result, expected);
      delete parser;
      return ok;
   }
   
   
   
}

#endif
