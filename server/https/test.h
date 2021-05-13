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
      
      bee::fish::https::Request requestFull;
      ok &= testFile(
         "Request with full json",
         "../https/tests/request-full.txt",
         requestFull,
         true
      );
      
      ok &= testResult(
         "Request full has json",
         requestFull.hasJSON() == true
      );
      
      bee::fish::json::_Object& object =
         *(requestFull.json()._object);
         
      ok &= testResult(
         "Request full object matched",
         object.matched() == true
      );
      
      ok &= testResult(
         "Request full has name",
         object.contains("name") == true
      );
      
      cerr << object["name"]->_capture << endl;
      Label* json = object["name"];
      cerr << *json << endl;
      /*
      ok &= testResult(
         "Request full name is Brett",
         object["name"]->value() == "Brett"
      );
      */
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
