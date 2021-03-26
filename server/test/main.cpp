#include <iostream>
#include "../b-string/test.h"
#include "../parser/test.h"
#include "../json/test.h"
#include "../https/test.h"
#include "version.h"

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
     
      if (!bee::fish::b_string::test())
      {
         cout << "B-String FAILED" << endl;
         return 1;
      }
      
      if (!bee::fish::parser::test())
      {
         cout << "Parser FAILED" << endl;
         return 2;
      }
      
      if (!bee::fish::json::test())
      {
         cout << "JSON FAILED" << endl;
         return 3;
      }
      
      if (!bee::fish::https::test())
      {
         cout << "HTTP FAILED" << endl;
         return 4;
      }
      
      cout << "ALL Tests PASSED!" << endl;
      
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