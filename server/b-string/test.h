#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "b-string.h"

namespace bee::fish::b_string
{
   
   inline bool testEmojis();
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testEmojis();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      std::string str = "Emoji 😀";
      BString bstr(str);
      std::string str2 = bstr;
      
      cout << bstr << endl;
      
      ok &= (str == str2);
      
      cout << endl;
      
      return ok;
   }

      
}

#endif
