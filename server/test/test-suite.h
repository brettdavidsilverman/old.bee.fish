#ifndef BEE_FISH_TEST__TEST_SUITE
#define BEE_FISH_TEST__TEST_SUITE

#include "../b-string/string.h"
#include "../b-string/test.h"
#include "../parser/test.h"
#include "../json/test.h"
#include "../https/test.h"

namespace bee::fish::test
{

   
   
   inline bool test()
   {
 
      if (!bee::fish::b_string::test())
      {
         cout << "B-String FAILED" << endl;
         return false;
      }
      
      if (!bee::fish::parser::test())
      {
         cout << "Parser FAILED" << endl;
         return false;
      }
      
      if (!bee::fish::json::test())
      {
         cout << "JSON FAILED" << endl;
         return false;
      }
      
      if (!bee::fish::https::test())
      {
         cout << "HTTP FAILED" << endl;
         return false;
      }
      
      return true;
      
   }
   
   
   
}

#endif