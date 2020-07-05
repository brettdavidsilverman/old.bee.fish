#include <iostream>
#include "parser.h"

using namespace std;
using namespace Bee::Fish::Parser;

int main(int argc, char* argv[]) {
   cout << "Hello World" << endl;
   
   // Character
   Character character('A');
   character.read("A");
   cout << character << endl;
   
   // Range
   Range range('a', 'z');
   range.read("b");
   cout << range << endl;
   
   // Word
   Word word("Brett");
   word.read("Bre");
   cout << word << endl;
   
   return 0;
}