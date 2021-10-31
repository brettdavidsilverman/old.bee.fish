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
   inline bool testParts();
   
   inline bool testFile(
      string label,
      path file,
      bee::fish::https::Request& request,
      bee::fish::misc::optional<bool> result
   );
   
   inline bool test()
   {
   
      bool ok = true;

      ok &= testRequest();
      ok &= testParts();
      
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
         bee::fish::misc::nullopt
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
      
      ok &= testResult(
         "Request full name is Brett",
         object["name"]->value() == "Brett"
      );
      
      
      
      bee::fish::https::Request urlRequest;
      ok &= testFile(
         "Request with path and query",
         "../https/tests/path.txt",
         urlRequest,
         bee::fish::misc::nullopt
      );
      
      ok &= testResult(
         "Request path is /path",
         urlRequest.path() == "/path"
      );
      
      ok &= testResult(
         "Request query is ?query",
         urlRequest.query() == "?query"
      );
      
      bee::fish::https::Request postRequest;

      ok &= testFile(
         "Post with encoded name value pairs",
         "../https/tests/post.txt",
         postRequest,
         true
      );
      
      return ok;
   }
   
   inline bool testParts()
   {
      
      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      bee::fish::https::Request request;
      
      ok &= testFile(
         "Request part 0",
         "../https/tests/request-part-0.txt",
         request,
         bee::fish::misc::nullopt
      );
      
      ok &= testFile(
         "Request part 1",
         "../https/tests/request-part-1.txt",
         request,
         bee::fish::misc::nullopt
      );
      
      ok &= testFile(
         "Request part 2",
         "../https/tests/request-part-2.txt",
         request,
         true
      );
      
      ok &= testResult(
         "Request object is valid",
         request._json->_object->matched()
      );
      
      return ok;
      
   }

   inline bool testFile(
      string label,
      path file,
      bee::fish::https::Request& request,
      bee::fish::misc::optional<bool> result
   )
   {
      bool ok = true;
      
      // open the sample session file
      ifstream input(file);
      Parser parser(request);
      parser.read(input);
      
      ok &= testResult(
         label,
         (request.result() == result)
      );
      
      input.close();
      
      return ok;
   }
 


      
}

#endif
