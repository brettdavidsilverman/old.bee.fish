#include <iostream>
#include "parser.h"
			
using namespace std;
			
namespace bee::fish::parser {
			
   bool test() {
			   
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
			   
			   And _and =
			      Word("Brett") and
			      Character(' ') and
			      Word("Silverman");
			   ok &= _and.read("Brett Silverman")
			      && _and.result();
			   cerr << "And:\t" << ok << endl;
			      
			   Or _or =
			      Word("Brett") or
			      Word("Silverman");
			   ok &= _or.read("Silverman")
			      && _or.result();
			   cerr << "Or:\t" << ok << endl;
			   
			   And _not =
			      Word("Candy") and
			      Character(' ') and
			      not Word("Dale")and
			      Word("Silverman");
			   ok &= _not.read("Candy DaleSilverman")
			      && _not.result();
			   cerr << "Not:\t" << ok << endl;
			   
			   if (ok)
			      cerr << endl << "SUCCESS" << endl;
			   else
			      cerr << endl << "FAIL" << endl;
			
			   return ok;
			   
   }
			
}