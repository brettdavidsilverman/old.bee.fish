#include <iostream>
#include "parser.h"
			
using namespace std;
			
namespace bee::fish::parser {
			
   inline bool test() {
			   
			   bool ok = true;
			  
			   Character character('c');
			   ok &= character.read("c")
			      && character.result();
			   cerr << "Char:\t" << ok << endl;
			   
			   Range range('a', 'z');
			   ok &= range.read("a")
			      && range.result();
			   cerr << "Range:\t" << ok << endl;
			   
			   Word word("Word");
			   ok &= word.read("Word") 
			      && word.result();
			   cerr << "Word:\t" << ok << endl;
			   
			   CIWord ciword("Word");
			   ok &= ciword.read("word")
			      && ciword.result();
			   cerr << "CIWord:\t" << ok << endl;
			   
			   Match match =
			      Word("a") and 
			      Word("b") and
			      Word("c");
			   ok &= match.read("abc")
			      && match.result() == true;
			   cerr << "Match:\t" << ok << endl;
			   
			   Match _and =
			      Word("Brett") and
			      Character(' ') and
			      Word("Silverman");
			   ok &= _and.read("Brett Silverman")
			      && _and.result();
			   cerr << "And:\t" << ok << endl;
			   
			   Match _or =
			      Word("Brett") or
			      Word("Silverman");
			   ok &= _or.read("Silverman")
			      && _or.result();
			   cerr << "Or:\t" << ok << endl;
			   
			   Match _optional =
			      Word("Candy") and
			      ~ Word("Dale") and
			      Word("Silverman");
			   ok &= _optional.read("CandySilverman")
			      && _optional.result();
			   cerr << "Optional:\t" << ok << endl;
			   
			   Match _optional2 =
			      Word("Candy") and
			      ~ Word("Dale");
			   ok &= _optional2.read("Candy")
			      && _optional2.result();
			   cerr << "Optional 2:\t" << ok << endl;
			
			   Match repeat = 
			      Character('*') and
			      Repeat(Character('B')) and
			      Character('*');
			   ok &= repeat.read("*BBB*")
			      && repeat.result();
			   cerr << "Repeat:\t" << ok << endl;
			   
			   Capture capture(Word("Brett"));
			   ok &= capture.read("Brett")
			      && capture.result()
			      && capture.value() == "Brett";
			   cerr << "Capture:\t" << ok << endl;
			   
			   Repeat BlankSpace =
			      Repeat(Character(' ') or Character('\t'));
			   
			   Set set(
			      Character('{'),
			      Capture(
			         Word("item"),
			         [&ok](Capture& item)
			         {
			            ok &= (item.value() == "item");
			         }
			      ),
			      Character(','),
			      Character('}')
			   );
		
			   ok &= set.read("{item,item}")
			      && set.result();
			   cerr << "Set:\t" << ok << endl;
			   
			   Word multipart("Brett");
			   multipart.read("Br", false);
			   
			   ok &= multipart.read("ett")
			      && multipart.result();

			   cerr << "Multipart:\t" << ok << endl;
			   
			   if (ok)
			      cerr << endl << "SUCCESS" << endl;
			   else
			      cerr << endl << "FAIL" << endl;
			
			   return ok;
			   
   }
			
}