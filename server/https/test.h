#ifndef BEE_FISH_HTTPS__TEST_H
#define BEE_FISH_HTTPS__TEST_H
#include <fstream>
#include <filesystem>
#include "../test/test.h"

#include "request.h"

using namespace std::filesystem;
using namespace BeeFishTest;

namespace BeeFishHTTPS
{

   inline bool testURL();
   inline bool testRequest();
   inline bool testParts();
   
   inline bool testFile(
      string label,
      path file,
      BeeFishHTTPS::Request& request,
      BeeFishMisc::optional<bool> result
   );
   
   inline bool test()
   {
   
      bool ok = true;

      ok &= testURL();
      ok &= testRequest();
      ok &= testParts();

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testURL() {
      cout << "Test URL" << endl;

      bool ok = true;

      Request::URL::Hex hex;
      Parser hexParser(hex);
      hexParser.read("a");
      ok &= testResult("URL Hex is 'a'", 
         hexParser.result() == true && 
         hex.value() == Char('a'));
      
      Request::URL::HexCharacter hexCharacter;
      Parser hexCharacterParser(hexCharacter);
      hexCharacterParser.read("%38");

      ok &= testResult("URL hex value is '8'", 
         hexCharacter.result() == true && 
         hexCharacter.character() == Char('8'));
      
      Request::URL::Path path;
      Parser pathParser(path);
      pathParser.read("Hello%20World%25");
      ok &= testResult("Path with escaped space is \"Hello World%\"",
         path.result() == BeeFishMisc::nullopt &&
         path.value() == "Hello World%");

      cout << "Path: " << path.result() << ": " << path.value() << endl;

/*
      Request::URL::HexCharacterSequence hexCharacterSequence;
      Parser sequenceParser(hexCharacterSequence);
      sequenceParser.read("%F0%9F%90%9D");

      ok &= testResult("URL hex character sequence is '🐝'", 
         hexCharacterSequence.result() == true && 
         hexCharacterSequence.value() == Char(L'🐝'));
*/

      return ok;


   }
   inline bool testRequest()
   {
      
      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      BeeFishHTTPS::Request requestHeadersOnly;
      ok &= testFile(
         "Request with only headers",
         "../https/tests/request.txt",
         requestHeadersOnly,
         true
      );

      ok &= testResult(
         "Request get",
         requestHeadersOnly.method() == "GET"
      );
      
      ok &= testResult(
         "Request has no json",
         requestHeadersOnly.hasJSON() == false
      );
      
      BeeFishHTTPS::Request requestBody;
      ok &= testFile(
         "Request with body",
         "../https/tests/request-body.txt",
         requestBody,
         true
      );
      
      ok &= testResult(
         "Request has json",
         requestBody.hasJSON() == true
      );
      
      BString name;
      bool hit = false;

      BeeFishJSON::Object::Function invokeOnName = 
         [&name, &hit](const BString& key, JSON& json) 
         {
            name = json.value();
            hit = true;
         };

      BeeFishHTTPS::Request requestFull(
         {
            {"name", invokeOnName}
         }
      );

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
           
      ok &= testResult(
         "Request full has name",
         hit == true
      );
      
      ok &= testResult(
         "Request full name is Brett",
         name == "Brett"
      );
      
      
      BeeFishMisc::optional<BString> name2;

      BeeFishHTTPS::Request request2;

      request2.captureObjectField("name", name2);

      ok &= testFile(
         "Request with full json 2",
         "../https/tests/request-full.txt",
         request2,
         true
      );
      
      ok &= testResult(
         "Request full has name 2",
         name2.hasValue()
      );
      
      ok &= testResult(
         "Request full name is Brett 2",
         name2 == BString("Brett")
      );

      BeeFishHTTPS::Request urlRequest;
      ok &= testFile(
         "Request with path and query",
         "../https/tests/path.txt",
         urlRequest,
         BeeFishMisc::nullopt
      );
      
      ok &= testResult(
         "Request path is /path",
         urlRequest.path() == "/path"
      );
      
      ok &= testResult(
         "Request query is query",
         urlRequest.query() == "query"
      );
      
      BeeFishHTTPS::Request escapedUrlRequest;
      ok &= testFile(
         "Request with escaped path and query",
         "../https/tests/escaped-path.txt",
         escapedUrlRequest,
         BeeFishMisc::nullopt
      );
      
      ok &= testResult(
         "Request escaped path is /path",
         escapedUrlRequest.path() == "/path"
      );
      
      ok &= testResult(
         "Request escaped query is query<space>query",
         escapedUrlRequest.query() == "query query"
      );

      BeeFishHTTPS::Request postRequest;
      
      // Post with anything but JSON is not allowed....
      ok &= testFile(
         "Post with encoded name value pairs",
         "../https/tests/post.txt",
         postRequest,
         false
      );

      return ok;
   }
   
   inline bool testParts()
   {
      
      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      BeeFishHTTPS::Request request;
      
      ok &= testFile(
         "Request part 0",
         "../https/tests/request-part-0.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok &= testFile(
         "Request part 1",
         "../https/tests/request-part-1.txt",
         request,
         BeeFishMisc::nullopt
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
      BeeFishHTTPS::Request& request,
      BeeFishMisc::optional<bool> result
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

      if (!ok) {
         cout << "Expected: " << result << endl;
         cout << "Got: " << request.result() << endl;
      }
      
      input.close();
      
      return ok;
   }
 


      
}

#endif
