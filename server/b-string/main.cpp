#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "string.h"
#include "test.h"

using namespace std;
using namespace bee::fish::b_string;

int main(int argc, const char* argv[]) {
  
   if (!bee::fish::b_string::test())
      return 1;
     
   cout << "Enter string" << endl;
   
   while (!cin.eof())
   {
      BString line;
      getline(cin, line);
     
      if (line == "")
         break;
     
      cout << line << endl;
   }
  
   cout << "Bye" << endl;
   
   return 0;
}
