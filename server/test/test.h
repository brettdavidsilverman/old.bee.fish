#ifndef BEE_FISH_TEST__TEST
#define BEE_FISH_TEST__TEST

#include "../b-string/string.h"

using namespace bee::fish::b_string;

namespace bee::fish::test
{
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