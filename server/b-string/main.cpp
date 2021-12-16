#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "config.h"
#include "string.h"
#include "test.h"
#include "version.h"

using namespace std;
using namespace BeeFishBString;

int main(int argc, const char* argv[]) {
  
   cerr << "bee.fish.parser"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_B_STRING_VERSION
           << endl;

   bool test =
      (hasArg(argc, argv, "-test") != -1);
      
   if (test)
   {
      cout << "Testing B-Strings" << endl;
      if (BeeFishBString::test())
         return 0;
         
      return 1;
   }
 
   cout << "Enter string" << endl;

   while (!cin.eof())
   {
 
      BString line;
      getline(cin, line);
     
      if (line == "")
         break;
      
      Data data = line.toData();
     
      cout << line << endl;
#ifdef SERVER
      cout << data.sha3() << endl;
#endif

   }
  
   cout << "Bye" << endl;
   
   return 0;
}
