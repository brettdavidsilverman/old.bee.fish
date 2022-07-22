#ifndef BEE_FISH_ID__TEST_H
#define BEE_FISH_ID__TEST_H

#include "id.h"
#include "../test/test.h"

using namespace BeeFishTest;

namespace BeeFishId
{
   inline bool testId();
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testId();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testId()
   {
      cout << "Id" << endl;
      
      bool ok = true;
      
      Id id = Id::fromKey("/jHhw+MeJxPjHieGT4x4nhwU");

      ok &= testResult(
         "Id from key",
         (id._name == "Test")
      );
      
      cout << endl;
      
      return ok;
   }
   

      
}

#endif
