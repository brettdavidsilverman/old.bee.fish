#ifndef BEE_FISH_TEST__TEST
#define BEE_FISH_TEST__TEST

#include "../b-string/string.h"

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
   
   inline int hasArg(
      int argc,
      const char* argv[],
      const BString& arg
   )
   {
      for (int i = 0; i < argc; i++)
      {
         if (arg == argv[i])
            return i;
      }
   
      return -1;
   }
   
}

#endif