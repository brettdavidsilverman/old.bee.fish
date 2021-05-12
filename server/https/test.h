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
   
   inline bool testFile(
      string label,
      path file,
      bee::fish::https::Request& request,
      optional<bool> result
   );
   
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
      bee::fish::https::Request requestHeadersOnly;
      ok &= testFile(
         "Request with only headers",
         "../https/tests/request.txt",
         requestHeadersOnly,
         nullopt
      );

      ok &= testResult(
         "Request get",
         requestHeadersOnly.method() == "GET"
      );
      
      ok &= testResult(
         "Request has no body",
         requestHeadersOnly.hasBody() == false
      );
      
      bee::fish::https::Request requestBody;
      ok &= testFile(
         "Request with body",
         "../https/tests/request-body.txt",
         requestBody,
         true
      );
      
      ok &= testResult(
         "Request has body",
         requestBody.hasBody() == true
      );
      
      ok &= testResult(
         "Request has json",
         requestBody.hasJSON() == true
      );
      
      return ok;
   }
   
   inline bool testFile(
      string label,
      path file,
      bee::fish::https::Request& request,
      optional<bool> result
   )
   {
      bool ok = true;
      
      // open the sample session file
      ifstream input(file);
      request.read(input);
      
      ok &= testResult(
         label,
         (request._result == result)
      );
      
      input.close();
      
      return ok;
   }
 


      
}

#endif
