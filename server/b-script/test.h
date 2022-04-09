#ifndef BEE_FISH_B_SCRIPT__TEST_H
#define BEE_FISH_B_SCRIPT__TEST_H

#include "../test/test.h"
#include "../parser/test.h"
#include "../json/json.h"
#include "b-script.h"

using namespace BeeFishParser;

namespace BeeFishBScript
{
   
   inline bool test();


   inline bool testIntrinsics();
   inline bool testNumbers(); 
   inline bool testStrings();
   inline bool testArrays();
   inline bool testObjects();
#ifdef SERVER
   inline bool testStreams();
#endif
   inline bool testOutput();

   inline bool test()
   {
      
      bool ok = true;

      ok &= testIntrinsics();
      ok &= testNumbers();
      ok &= testStrings();
      ok &= testObjects();
      ok &= testOutput();

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testIntrinsics()
   {
      cout << "Intrinsics" << endl;
      
      bool ok = true;
      
      cout << endl;
      
      return ok;
   }
  
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;
      
      cout << endl;
      
      return ok;
   }    

   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;
  
      cout << endl;
      
      return ok;
   }

   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      cout << endl;
      
      return ok;
      
   }


   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;

      cout << endl;
      
      return ok;
      
   }

   inline bool testOutput()
   {
      cout << "Output" << endl;
      
      bool ok = true;
      

      cout << endl;
      
      return ok;
   }

      
}

#endif
