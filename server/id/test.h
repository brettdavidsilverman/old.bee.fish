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
      
      Id id = Id::fromKey("/jGPjHieGPjHicT4x4nhw+MeJ4cPiPjHhk+MeJyT4x4nJw+MeJ4cY+MeJxPjHieGPjHicnE+MeJPjHicnGH4cnGPhycT4ck+HGT4cY+HE+GT4Y+E+DycZPJxjyZPJjxjxOR8k8jkyQA=");

      ok &= testResult(
         "Id from key",
         (id._name == "Brett Silverman")
      );
      
      cout << endl;
      
      return ok;
   }
   

      
}

#endif
