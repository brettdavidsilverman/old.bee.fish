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
   
   inline bool displayResult(
      const string& value,
      bool result
   );
   
   inline bool test() {
   
      bool ok = true;
  
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
      displayResult(value, value == "Brett");
     
      Repeat BlankSpace =
         Repeat(Character(' ') or Character('\t'));
   
      unsigned int count = 0;
      const Match set = Set(
         Character('{'),
         Capture(
            Word("item"),
            [&count](Capture& item)
            {
               cerr << item.value() << endl;
               if (item.value() == "item")
                  ++count;
            }
         ),
         Character(','),
         Character('}')
      );
     
      ok &= test("Set", set, true, "{item,item,item}");
      ok &= displayResult("count", (count == 3));
      
      ok &= test("Set empty", set, true, "{}");
   
      cerr << "Multipart:" << endl;
      
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
      
      
      const Match item = Word("item");
      
      const Match object = Set(
         Character('{'),
         LoadOnDemand(item),
         Character(','),
         Character('}')
      );
      
      ok &= test("LoadOnDemand", object, true, "{item,item}");
   
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
      
      cerr << label << ":\t" << endl;
      
      string value;
      
      Match parser =
         Capture(
            match,
            value
         )
         and Character(Match::EndOfFile);
      
      if (result)
         ok &= parser.read(test)
            && (parser.result() == true);
      else
         ok &= !parser.read(test) ||
               parser.result() != true;
      
      displayResult(value, ok);
      
      if (!ok)
      {
         cerr << parser << endl;
         throw runtime_error(label);
      }
      
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
         
      cerr << "\t" << /*value << ":" << */ text << endl;
      return result;
   }
   
}