#ifndef BEE_FISH_HTTPS__TEST_H
#define BEE_FISH_HTTPS__TEST_H
#include <fstream>
#include <filesystem>
#include "../test/test.h"

#include "request.h"

using namespace std::filesystem;
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
      path filePath("../https/request.txt");
      ifstream input(filePath);
      bee::fish::https::Request request;
      request.read(input);
      
      cerr <<"****" << request._result << endl;
      
      ok &= testResult(
         "Parse result",
         (request._result == true)
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
