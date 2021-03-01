#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>
#include "parser.h"

using namespace std;

namespace bee::fish::parser {
   
   inline bool test(
      BString label,
      MatchPtr parser,
      bool result,
      const string& test
   );
   
   inline bool testMatch(
      BString label,
      MatchPtr match,
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
   
   inline bool testCapture();
   
   inline bool testMisc();
   
   inline bool testResult(
      BString label,
      BString expected,
      BString actual
   );
   
   inline bool displayResult(
      BString value,
      bool result
   );
   
   
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
      ok &= testMisc();
      ok &= testCapture();
      
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
      MatchPtr _a(a);
      
      bool ok =
         _a->read("a") &&
         (_a->result() == true);

      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL" << endl;
      }
      
     // _a.reset();
      
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
 
      return ok;
   }
   
   inline bool testAnd()
   {
      bool ok = true;
      
      MatchPtr testAnd =
         Character('a') and
         Character('b') and
         Character('c');
      
      ok &= testMatch("Simple 'and' match", testAnd, "abc", true, "abc");

      MatchPtr testAndNoMatch = testAnd->copy();
      
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "abz");
      
      return ok;
   }
   
   inline bool testOr()
   {
      bool ok = true;
      
      MatchPtr testOr =
         Word("true") or
         Word("false");
      
      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      
      MatchPtr testOrNoMatch(testOr->copy());
      
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe");
      
      return ok;
   }
   
   inline bool testNot()
   {
      bool ok = true;
      
      MatchPtr testNot = not Word("ABC");
      ok &= testMatch("Simple 'not' match", testNot, "abc", true, "a");
      
      Not testNotNoMatch(testNot->copy());
 
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "ABC", false);
    
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
      
      MatchPtr testOptional12 = testOptional.copy();
      
      ok &= testMatch("Optional one two match", testOptional12, "onetwo", true, "onetwo");
      
      MatchPtr testOptional1 = testOptional.copy();
      
      ok &= testMatch("Optional one match", testOptional1, "one", true, "one");

      MatchPtr testOptional123 =
         Word("one") and
         Optional2(
            new Word("two"),
            new Word("three")
         );
      
      MatchPtr _testOptional123 = testOptional123->copy();
      
      ok &= testMatch("Optional one two three match", _testOptional123, "onetwothree", true, "onetwothree");
      
      MatchPtr testOptional13 = testOptional123->copy();
      
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
   
   inline bool testCapture()
   {
      bool ok = true;
      
      MatchPtr test1 = Word("capture");
      
      MatchPtr testCapture = Capture(test1);
      
      ok &= testMatch("Test simple capture", testCapture, "capture", true, "capture");
      ok &= testResult("Test simple capture result", "capture", testCapture->value());

      MatchPtr matchBrett = Capture(Word("Brett"));
      
      ok &= testMatch("Capture", matchBrett, "Brett", true, "Brett");
      ok &= testResult("Capture result", "Brett", matchBrett->value());

      class _Capture : public M
      {
      
      public:
         BString _name;
         BString _value;
         
         
         _Capture() : M(
            Capture(
               Word("name"),
               _name
            ) and
            Character(' ') and
            Capture(
               Word("value"),
               _value
            )
         )
         {
         }
         
         
      };
      
      _Capture capture;
      capture.read("name value");
      cout << capture << endl;
     // ok &= testMatch("Capture class", pointer, "name value", true, "name value");
      ok &= displayResult("Capture class result", (capture._name == "name") && (capture._value == "value"));
      cout << capture._name << endl;
      /*
      CapturePtr template_;
      LoadOnDemand loadOnDemand = LoadOnDemand(template_);

      //template_ = _Capture2();
      ok &= testMatch("Capture class load on demand", loadOnDemand, "name value", true, "name value");
      CapturePtr item = (CapturePtr)loadOnDemand._item;
      ok &= displayResult("Capture class load on demand result", (item._name == "name") && (item._value == "value"));
      */
      return ok;
   }
   
   inline bool testMisc() {
   
      bool ok = true;
  
      ok &= test("Character", Character('c'), true, "c");
      ok &= test("Range", Range('a', 'z'), true, "a");
      ok &= test("Word", Word("Word"), true, "Word");
      ok &= test("CIWord", CIWord("CIWord"), true, "ciword");


      MatchPtr c;
      ok &= test("And",
         Word("a") and 
         Word("b") and
         (c = Word("c")),
         true, "abc"
      );
     
      ok &= test(
         "And 2",
         Word("Brett") and
         Character(' ') and
         Word("Silverman"),
         true,
         "Brett Silverman"
      );

      MatchPtr _or =
         Word("Brett") or
         Word("Silverman");
         
      ok &= test("Or first", _or->copy(), true, "Brett");
      ok &= test("Or second", _or->copy(), true, "Silverman");
      ok &= test("Or fail", _or->copy(), false, "Dale");
   
      MatchPtr optional =
         Word("Candy") and
         ~ Word("Dale") and
         ~ Word("Silverman");
      ok &= test("Optional first", optional, true, "CandySilverman");
      ok &= test("Optional second", optional, true, "CandyDaleSilverman");
      ok &= test("Optional end", optional, true, "CandyDale");

      MatchPtr repeat = 
         Character('*') and
         Repeat(Character('B'), 3, 4) and
         Character('*');
      ok &= test("Repeat", repeat->copy(), true, "*BBB*");
      ok &= test("Repeat fail 1", repeat->copy(), false, "*BB*");
      ok &= test("Repeat fail 2", repeat->copy(), false, "*BBB");
      ok &= test("Repeat fail 3", repeat->copy(), false, "*BBBBB*");

      MatchPtr repeatEmpty = 
         Character('*') and
         Repeat(Character('B'), 0) and
         Character('*');
      ok &= test("Repeat empty", repeatEmpty, true, "**");

 
      // Load on demand
      MatchPtr loadOnDemandItem;
      
      MatchPtr loadOnDemand = 
         LoadOnDemand(loadOnDemandItem) and
         Word("David");
         
      loadOnDemandItem = Label("Name", Word("Brett"));
      
      ok &= test("Load on demand", loadOnDemand, true, "BrettDavid");

      
      MatchPtr matchBrett = Capture(Word("Brett"));
      
      ok &= test("Capture", matchBrett, true, "Brett");
    
      ok &= testResult("Capture Brett", "Brett", matchBrett->value());
     
      /*
      // Multipart
      Capture multipart(Word("Brett"));
      multipart.read("Br", false);
   
      bool multipartResult
         = multipart.read("ett")
         && multipart.result();
      ok &= displayResult("Multipart", multipartResult);
      
      const MatchPtr source =
         Word("Hello") and
         Word("World");
      MatchPtr copy = source and Character('.');
      ok &= test("Copy and", copy, true, "HelloWorld.");
      
      // Label
      const MatchPtr label = Label("A", Character('A'));
      ok &= test("Label", label, false, "B");
      
      stringstream stream;
      stream << *label;
      ok &= testResult("Label stream", "A<false>()", stream.str());
      
      
      // Invoke
      BString invokeValue;
      Invoke invoke(
         Word("invoke"),
         [&invokeValue](Capture& item)
         {
            invokeValue = item.value();
         } 
      );
      
      ok &= test("Invoke", invoke, true, "invoke");
      ok &= testResult("Invoke value", "invoke", invokeValue);
      */
      return ok;
   
   }
   
   inline bool testMatch(
      BString label,
      MatchPtr parser,
      string text,
      optional<bool> result,
      BString expected
   )
   {
      cout << label << ":\t";
      
      bool ok = true;
      Capture capture(parser);
      capture.read(text);
      
      if (result == true && capture._result != true)
         ok = false;
      else if (result == false && capture._result != false)
         ok = false;
      else if (capture._result == true && expected.size())
      {
         
         if (capture.value() != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL "      << capture._result << endl;
         cout << "\tTested\t" << text << endl;
         cout << "\tExpect\t" << expected << endl;
         cout << "\tGot\t"    << capture.value() << endl;
         cout << "\t"         << capture << endl;
      }
      
      return ok;
   }
   
   inline bool test(
      BString label,
      MatchPtr parser,
      bool result,
      const string& test
   )
   {
      bool ok = true;
      
      cout << label << ":\t";
      
      if (result)
         ok &= (parser->read(test) == true);
      else
         ok &= (parser->read(test) == false);
      
      displayResult("", ok);
      
      if (!ok)
      {
         cout << endl << *parser << endl;
         //throw runtime_error((string)label);
      }
      
      return ok;
   }
  

   inline bool testResult(
      BString label,
      BString expected,
      BString actual
   )
   {
      cout << label << ":\t";
      
      bool ok = (expected == actual);
      
      displayResult("", ok);
      
      return ok;
   }
   
   inline bool displayResult(
      BString value,
      bool result
   )
   {
      string text;
      if (result)
         text = "ok";
      else
         text = "FAIL";

      if (value != "")
         cout << value << ":\t";

      cout << text << endl;
      return result;
   }
   
}

#endif