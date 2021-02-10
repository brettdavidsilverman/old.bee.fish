#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>
#include "parser.h"

using namespace std;

namespace bee::fish::parser {
   
   inline bool test(
      const string& label,
      Match parser,
      bool result,
      const string& test
   );
   
   inline bool testBasics();
   
   inline bool testResult(
      const string& label,
      const string& expected,
      const string& actual
   );
   
   inline bool displayResult(
      const string& value,
      bool result
   );
   
   inline bool test() {
   
      bool ok = true;
  
      
      ok &= testBasics();
      
      ok &= test("Character", Character('c'), true, "c");
      ok &= test("Range", Range('a', 'z'), true, "a");
      ok &= test("Word", Word("Word"), true, "Word");
      ok &= test("CIWord", CIWord("CIWord"), true, "ciword");
      ok &= test("And",
         Word("a") and 
         Word("b") and
         Word("c"),
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

      Match _or =
         Word("Brett") or
         Word("Silverman");
         
      ok &= test("Or first", _or, true, "Brett");
      ok &= test("Or second", _or, true, "Silverman");
      ok &= test("Or fail", _or, false, "Dale");
   
      Match optional =
         Word("Candy") and
         ~ Word("Dale") and
         ~ Word("Silverman");
      ok &= test("Optional first", optional, true, "CandySilverman");
      ok &= test("Optional second", optional, true, "CandyDaleSilverman");
      ok &= test("Optional end", optional, true, "CandyDale");
      ok &= test("Optional fail", optional, false, "CandyBrett");

      Match repeat = 
         Character('*') and
         Repeat(Character('B'), 3) and
         Character('*');
      ok &= test("Repeat", repeat, true, "*BBB*");
      ok &= test("Repeat fail 1", repeat, false, "*BB*");
      ok &= test("Repeat fail 2", repeat, false, "*BBB");

      Match repeatEmpty = 
         Character('*') and
         Repeat(Character('B'), 0) and
         Character('*');
      ok &= test("Repeat empty", repeatEmpty, true, "**");

      // Load on demand
      Match loadOnDemandItem;
      Match loadOnDemand =
         LoadOnDemand(loadOnDemandItem) and Word("David");
      loadOnDemandItem = Label("item", Word("Brett"));
      ok &= test("Load on demand", loadOnDemand, true, "BrettDavid");

      ok &= test("Capture", Capture(Word("Brett")), true, "Brett");
     
      string value;
      Match captureFunc = Capture(
         Word("Brett"),
         [&value](Capture& item)
         {
            value = item.value();
         }
      );
      ok &= test("Capture func", captureFunc, true, "Brett");
      ok &= testResult("Capture func result", "Brett", value);
      class _Capture : public Capture
      {
      
      public:
         string _value;
         
         _Capture() : Capture(
            Word("captured"),
            _value
         )
         {
         }
      };
      ok &= test("Capture class value", _Capture(), true, "captured");
      class _Capture2 : public Match
      {
      
      public:
         string _name;
         string _value;
         
         
         _Capture2() : Match(
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
      ok &= test("Capture class value 2", _Capture2(), true, "name value");
      
      Match capture2;
      Match loadOnDemand2 = LoadOnDemand(capture2);
      capture2 = _Capture2();
      ok &= test("Capture class 2 load on demand", loadOnDemand2, true, "name value");

      cerr << "Multipart:\t";
      
      Capture multipart(Word("Brett"));
      multipart.read("Br", false);
   
      bool multipartResult
         = multipart.read("ett")
         && multipart.result();
      ok &= displayResult(multipart.value(), multipartResult);
      
      const Match source =
         Word("Hello") and
         Word("World");
      Match copy = source and Character('.');
      ok &= test("Copy and", copy, true, "HelloWorld.");
         
      // Label
      const Match label = Label("A", Character('A'));
      ok &= test("Label", label, false, "B");
      
      stringstream stream;
      stream << label;
      ok &= testResult("Label stream", "A<?>()", stream.str());
      
      
      if (ok)
         cerr << endl << "SUCCESS" << endl;
      else
         cerr << endl << "FAIL" << endl;
      
      return ok;
   
   }

   inline bool test(
      const string& label,
      Match match,
      bool result,
      const string& test
   )
   {
      bool ok = true;
      
      cerr << label << ":\t";
      
      string value;
      
      Match parser =
         Capture(
            match,
            value
         )
         and Character(Match::EndOfFile);
      
      if (result)
         ok &= (parser.read(test) == true);
      else
         ok &= (parser.read(test) == false);
      
      displayResult(value, ok);
      
      if (!ok)
      {
         cerr << endl << parser << endl;
         throw runtime_error(label);
      }
      
      return ok;
   }
  
   inline bool testBasics()
   {
      cerr << "Test bootstrap:\t";
      
      Character a('a');
      Match _a = a;
      
      bool ok =
         _a.read("a") &&
         (_a.result() == true);

      if (ok)
         cerr << "ok" << endl;
      else
      {
         cerr << "FAIL" << endl;
         return false;
      }
      
      return ok;
   }
   
   inline bool testResult(
      const string& label,
      const string& expected,
      const string& actual
   )
   {
      cerr << label << ":\t";
      
      bool ok = (expected == actual);
      
      displayResult(actual, ok);
      
      return ok;
   }
   
   inline bool displayResult(
      const string& value,
      bool result
   )
   {
      string text;
      if (result)
         text = "ok";
      else
         text = "FAIL";

     // cerr << "|" << value << "|\t";

      cerr << text << endl;
      return result;
   }
   
}

#endif