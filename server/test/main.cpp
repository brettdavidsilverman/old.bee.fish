#include <iostream>
#include "test.h"
#include "version.h"
#include "../b-string/test.h"
#include "../parser/test.h"
#include "../json/test.h"
#include "../https/test.h"

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
);


int main(int argc, const char* argv[])
{
   try
   {
   
      std::cout << "Bee.Fish Test Suite" << std::endl;
      std::cout 
        << "C++ run time: "
           << __cplusplus
           << std::endl
        << "Version: "
           << BEE_FISH_TEST_VERSION
           << std::endl;
 
      cout << "Testing..." << endl;
     
      bool ok = bee::fish::test::test();
      
      if (ok)
         cout << "ALL Tests PASSED!" << endl;
      else
         cout << "ERROR!" << endl;
         
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << "\n";
      return -1;
   }

   return 0;
}

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
)
{
   for (int i = 0; i < argc; i++)
   {
      if (arg == argv[i])
         return i;
   }
   
   return -1;
}