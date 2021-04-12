#ifndef BEE_FISH_HTTPS__TEST_H
#define BEE_FISH_HTTPS__TEST_H
#include <fstream>
#include "../test/test.h"

#include "request.h"

using namespace bee::fish::test;

namespace bee::fish::https
{

   inline bool testRequest();

   inline bool test()
   {
   
      bool ok = true;
      
      ok &= testRequest();

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testRequest()
   {
      cout << "Test request" << endl;
      
      bool ok = true;
     
      
      // open the sample session file
      ifstream input("../https/sample-session.txt");
      
      bee::fish::https::Request request;

      ok &= testResult(
         "Parse result",
         (request.read(input, true) == true)
      );
      
      ok &= testResult(
         "Request method",
         (request.method() == "GET")
      );
      
      cout << endl;
      
      return ok;
   }


      
}

#endif
