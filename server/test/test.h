#ifndef BEE_FISH_TEST__TEST
#define BEE_FISH_TEST__TEST

#include "../b-string/string.h"

namespace bee::fish::b_string
{
   inline bool test();
}

namespace bee::fish::parser
{
   inline bool test();
}

namespace bee::fish::json
{
   inline bool test();
}

namespace bee::fish::https
{
   inline bool test();
}

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
   
   inline bool testResult(
      BString label,
      bool ok
   )
   {
      cout << label << ":\t";
      
      string text;
      
      if (ok)
         cout << "ok";
      else
         cout << "FAIL";

      cout << endl;
      
      return ok;
   }
   
}

#endif