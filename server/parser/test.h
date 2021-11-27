#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>

#include "misc.h"

#include "parser.h"

#include "../test/test.h"

using namespace std;
using namespace BeeFishTest;

namespace BeeFishParser {
   
   bool testMatch(
      BString label,
      Match* match,
      string text,
      BeeFishMisc::optional<bool> result = false,
      BString expected = ""
   );
   
   bool testMatchDelete(
      BString label,
      Match* match,
      string text,
      BeeFishMisc::optional<bool> result = false,
      BString expected = ""
   );

   
   bool testBasics();
   bool testCharacter();
   bool testRange();
   bool testWord();
   bool testCaseInsensitiveWord();
   
   bool testRepeat();
   bool testAnd();
   bool testOr();
   bool testNot();
   bool testOptional();
   
   bool testBString(); 
   
   bool testLabel();
   bool testCapture();
   bool testInvoke();
   
   extern Word* _loadOnDemandItem;
   
   bool testLoadOnDemand();
   
   
   bool testMisc();
   
   
   
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
      cout << "Test basics:\t";
      
      Character a('a');
      Match* _a = a.copy();
      
      Parser parser(*_a);
      
      bool ok =
         parser.read("a") &&
         (parser.result() == true);

      delete _a;

      if (ok)
         cout << "ok" << endl;
      else
         cout << "FAIL" << endl;
      
      
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
      MatchPointer<Capture> characterMatch(
         new Capture(new CharA())
      );
      ok &= testMatch("Character match", characterMatch, "A", true, "A");
      delete characterMatch;
      
      MatchPointer<CharA> characterNoMatch(new CharA());
      ok &= testMatch("Character no match", characterNoMatch, "B");
      delete characterNoMatch;
      
      MatchPointer<Capture> any(
         new Capture(new Character())
      );
      ok &= testMatch("Character any", any, "a", true, "a");
      delete any;
      
      return ok;
   }
   
   inline bool testRange()
   {
      bool ok = true;
      
      // Character
      MatchPointer<Capture> rangeMatch(
         new Capture(new Range('a', 'z'))
      );

      ok &= testMatch("Range match", rangeMatch, "b", true, "b");
      delete rangeMatch;
      
      MatchPointer<Capture> rangeNoMatch(
         new Capture(new Range('a', 'z'))
      );
      ok &= testMatch("Range no match", rangeNoMatch, "B");
      delete rangeNoMatch;
      
      return ok;
   }
   
   inline bool testWord()
   {
      bool ok = true;
      
      // Character
      MatchPointer<Capture> wordMatch(
         new Capture(
            new Word("Word")
         )
      );
      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");
      delete wordMatch;
      
      MatchPointer<Word> wordNoMatch(new Word("Word"));
      ok &= testMatch("Word no match", wordNoMatch, "Wor*");
      delete wordNoMatch;
      
      return ok;
   }
   
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      Capture* ciWordMatch = new
         Capture(
            new CIWord("ABC")
         );
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");
      delete ciWordMatch;
      
      CIWord* ciWordNoMatch = new CIWord("ABC");
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ");
      delete ciWordNoMatch;
      
      return ok;
   }
   
   inline bool testRepeat()
   {
      bool ok = true;
      Capture* repeat = new
         Capture(
            new Repeat(
               new Character()
            )
         );

      ok &= testMatch("Repeat any character match", repeat, "helloworld", BeeFishMisc::nullopt, "helloworld");
      delete repeat;
      
      Capture* repeat2 = new
         Capture(
            new And(
               new Character('*'),
               new Repeat(
                  new Character('B'), 3, 4
               ),
               new Character('*')
            )
         );
      
      Match* tests[] =
      {
         repeat2->copy(),
         repeat2->copy(),
         repeat2->copy(),
         repeat2->copy()
      };
      
      ok &= testMatch("Repeat", tests[0], "*BBB*", true, "*BBB*");
      ok &= testMatch("Repeat fail 1", tests[1],  "*BB*");
      ok &= testMatch("Repeat fail 2", tests[2], "*BBB", BeeFishMisc::nullopt);
      ok &= testMatch("Repeat fail 3", tests[3], "*BBBBB*");

      delete repeat2;
      delete tests[0];
      delete tests[1];
      delete tests[2];
      delete tests[3];
      
      Capture* repeatEmpty = new Capture(
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
      
      Capture* testAnd = new Capture(
         new And(
            new Character('a'),
            new Character('b'),
            new Character('c')
         )
      );

      ok &= testMatch("Simple 'and' match", testAnd, "abc", true, "abc");
      
      Match* testAndNoMatch = testAnd->copy();
      
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "abz");
      
      delete testAnd;
      delete testAndNoMatch;
      
      return ok;
   }
   
   inline bool testOr()
   {
      bool ok = true;
      
      Capture* testOr = new Capture(
         new Or(
            new Word("true"),
            new Word("false")
         )
      );

      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      
      Match* testOrNoMatch =
         testOr->copy();
      
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe");
      
      delete testOr;
      delete testOrNoMatch;
      
      Capture* _or = new Capture(
         new Or(
            new Word("Brett"),
            new Word("Silverman")
         )
      );
      
      Match* or1 = _or->copy();
      Match* or2 = _or->copy();
      Match* or3 = _or->copy();
      
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
      
      Capture* testNot = new
         Capture(
            new Not(new Word("ABC"))
         );
      
      ok &= testMatch("Simple 'not' match", testNot, "abc", true);
      
      Match* testNotNoMatch = testNot->copy();
      
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "ABC", false);
      delete testNot;
      delete testNotNoMatch;
      
      Capture* _not1 = new Capture(
         new Not(
            new Range('a', 'z')
         )
      );

      ok &= testMatch("Not range match", _not1, "A", true);
      delete _not1;
      
      Match* _not2 = new Not(
         new Range('a', 'z')
      );

      ok &= testMatch("Not range no match", _not2, "a");
 
      delete _not2;
      
      return ok;
   }
   
   inline bool testOptional()
   {
   
      bool ok = true;
      
      Capture* testOptional = new Capture(
         new And(
            new Word("one"),
            new Optional(new Word("two"))
         )
      );
      
      Match* testOptional12 = testOptional->copy();
      
      ok &= testMatch("Optional one two match", testOptional12, "onetwo", true, "onetwo");
      delete testOptional12;
      
      Match* testOptional1 = testOptional->copy();
      
      ok &= testMatch("Optional one match", testOptional1, "one", BeeFishMisc::nullopt, "one");
      delete testOptional1;
      delete testOptional;
            
      return ok;
      
   }
   
   inline bool testBString()
   {
      bool ok = true;
      Capture* runes = new Capture(
         new Word("ᛒᚢᛞᛖ")
      );
 
      
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 1", runes, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      delete runes;
      
      Capture* runes2 = new Capture(
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
      Capture* label = new Capture(
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
      
      Capture* test1 = new Capture(
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
      
      Capture* capture = new Capture(
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
      Invoke* invoke = new Invoke(
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

      Test* testParser = new Test();

      ok &= testMatch("Invoke class virtual", testParser, "test", true);
      ok &= testResult("Invoke class virtual value", testParser->_test == "test");

      delete testParser;
      return ok;
   }
   
   inline bool testLoadOnDemand()
   {
      bool ok = true;

      Word* _loadOnDemandItem = new Word("Brett");
   

      // Load on demand
      Capture* loadOnDemand = new Capture(
         new And(
            new LoadOnDemand(_loadOnDemandItem),
            new Word("David")
         )
      );

      ok &= testMatch("Load on demand", loadOnDemand, "BrettDavid", true, "BrettDavid"); 
 
      delete loadOnDemand;
      
      return ok;
   }
   
  
   inline bool testMisc()
   {
   
      bool ok = true;
      
      Match* c;
      
      Capture* _and = new Capture(
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
      
      ok &= testResult("And c", c->value() == "c");
      delete _and;
      
      // Multipart
      Capture multipart(new Word("Brett"));
      Parser parser(multipart);
      BeeFishMisc::optional<bool> matched;
      parser.read("Br");
      parser.read("ett");
      
      bool multipartResult =
         multipart.matched() &&
         parser.result() == true;
         
      ok &= testResult("Multipart", multipartResult);
      
      
      return ok;
   
   }
   
   inline bool testMatch(
      BString label,
      Match* match,
      string text,
      BeeFishMisc::optional<bool> result,
      BString expected
   )
   {
      cout << label << ":\t";
      
      bool ok = true;
      Parser parser(*match);
      parser.read(text);
      
      BString value;

      if (match->matched())
         value = match->value();

      ok = (result == match->result());

      if (match->matched() && expected.size())
      {
         if (value != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL. Got  ";
         if (parser.result() == true)
            cout << "true";
         else if (parser.result() == false)
            cout << "false";
         else
            cout << "?";
         cout << endl;
         cout << "\tTested   " << text << endl;
         cout << "\tExpected " << expected << endl;
         cout << "\tCaptured " << value << endl;
#ifdef DEBUG
         cout << "\t"          << *match << endl;
#endif
      }
      
      return ok;
   }
   
   inline bool testMatchDelete(
      BString label,
      Match* parser,
      string text,
      BeeFishMisc::optional<bool> result,
      BString expected
   )
   {
      bool ok = testMatch(label, parser, text, result, expected);
      delete parser;
      return ok;
   }
   
   
   
}

#endif
