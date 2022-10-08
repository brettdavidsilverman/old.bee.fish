#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H
#include "../test/test.h"

#include "../json/json-parser.h"
#include "web-request.h"
#include "../b-script/b-script-parser.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

using namespace BeeFishTest;

namespace BeeFishWeb
{

   inline bool testURL();
#ifdef SERVER
   inline bool testWebRequest();
   inline bool testParts();
   inline bool testJSON();
   inline bool testPostImage();
#endif

   inline bool testRequest()
   {
   
      bool ok = true;

      //ok &= testJSON();
      //return ok;

      ok &= testURL();

#ifdef SERVER
      ok &= testWebRequest();
      ok &= testParts();
      ok &= testJSON();
      ok &= testPostImage();
#endif

      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testURL() {
      cout << "Test URL" << endl;

      bool ok = true;

      WebRequest::URL::Hex hex;
      Parser hexParser(hex);
      hexParser.read("a");
      ok &= testResult("URL Hex is 'a'", 
         hexParser.result() == true && 
         hex.value() == Char('a'));
      
      WebRequest::URL::HexCharacter hexCharacter;
      Parser hexCharacterParser(hexCharacter);
      hexCharacterParser.read("%38");
      
      ok &= testResult("URL hex value is '8'", 
         hexCharacter.result() == true && 
         hexCharacter.character() == Char('8'));
            
      WebRequest::URL::Path path;
      Parser pathParser(path);
      pathParser.read("Hello%20World%25");
      ok &= testResult("Path with escaped space is \"Hello World%\"",
         path.result() == BeeFishMisc::nullopt &&
         path.value() == "Hello World%");

      cout << "Path: " << path.result() << ": " << path.value() << endl;

      WebRequest::URL url;
      Parser urlWithQueryParser(url);
      urlWithQueryParser.read("/beehive/settings/?key=settings HTTP/1.1");

      cout << "Path With Query: " << url.result() << " " << url.query().value() << endl;
      
      ok &- testResult(
         "Path with query",
         urlWithQueryParser.result() == true && 
         url.query()["key"] == "settings"
      );



      WebRequest::URL::Path hexCharacterSequence;
      Parser sequenceParser(hexCharacterSequence);
      sequenceParser.read("%F0%9F%90%9D");
      ok &= testResult("URL hex character sequence is '🐝'", 
         hexCharacterSequence.result() == BeeFishMisc::nullopt && 
         hexCharacterSequence.value() == L'🐝');

      return ok;


   }

#ifdef SERVER
   inline bool testWebRequest()
   {
      using namespace BeeFishJSON;

      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      BeeFishWeb::WebRequest requestHeadersOnly;
      ok &= testFile(
         "WebRequest with only headers",
         "server/web-request/tests/request.txt",
         requestHeadersOnly,
         true
      );

      ok &= testResult(
         "WebRequest get",
         requestHeadersOnly.method() == "GET"
      );
      
      ok &= testResult(
         "WebRequest has no json",
         requestHeadersOnly.hasJSON() == false
      );
      
      BeeFishWeb::WebRequest requestBody;
      ok &= testFile(
         "WebRequest with body",
         "server/web-request/tests/request-body.txt",
         requestBody,
         true
      );
      
      ok &= testResult(
         "WebRequest has json",
         requestBody.hasJSON() == true
      );
      
      BString name;
      bool hit = false;

      BeeFishJSON::JSONParser::OnValue invokeOnName = 
         [&name, &hit](const BString& key, JSON& json) 
         {
            name = json.value();
            hit = true;
         };

      BeeFishWeb::WebRequest requestFull;
      JSONParser parser(requestFull);
      parser.invokeValue("name", invokeOnName);

      ok &= testFile(
         parser,
         "WebRequest with full json",
         "server/web-request/tests/request-full.txt",
         requestFull,
         true
      );
      
      ok &= testResult(
         "WebRequest full has json",
         requestFull.hasJSON() == true
      );

      ok &= testResult(
         "WebRequest full has name",
         hit == true
      );
      
      ok &= testResult(
         "WebRequest full name is Brett",
         name == "Brett"
      );
      
      
      BeeFishMisc::optional<BString> name2;

      BeeFishWeb::WebRequest request2;
      JSONParser parser2(request2);
      parser2.captureValue("name", name2);

      ok &= testFile(
         parser2,
         "WebRequest with full json 2",
         "server/web-request/tests/request-full.txt",
         request2,
         true
      );
      
      ok &= testResult(
         "WebRequest full has name 2",
         name2.hasValue()
      );
      
      ok &= testResult(
         "WebRequest full name is Brett 2",
         name2 == BString("Brett")
      );

      BeeFishWeb::WebRequest urlWebRequest;
      ok &= testFile(
         "WebRequest with path and query",
         "server/web-request/tests/path.txt",
         urlWebRequest,
         BeeFishMisc::nullopt
      );
      
      ok &= testResult(
         "WebRequest path is /path",
         urlWebRequest.path() == "/path"
      );
      
      ok &= testResult(
         "WebRequest query is query",
         urlWebRequest.query() == "query"
      );
      
      BeeFishWeb::WebRequest escapedUrlWebRequest;
      ok &= testFile(
         "WebRequest with escaped path and query",
         "server/web-request/tests/escaped-path.txt",
         escapedUrlWebRequest,
         BeeFishMisc::nullopt
      );
      
      ok &= testResult(
         "WebRequest escaped path is /path",
         escapedUrlWebRequest.path() == "/path"
      );
      
      ok &= testResult(
         "WebRequest escaped query is query<space>query",
         escapedUrlWebRequest.query() == "query query"
      );

      BeeFishWeb::WebRequest postWebRequest;
      
      ok &= testFile(
         "Post with encoded name value pairs",
         "server/web-request/tests/post.txt",
         postWebRequest,
         true
      );

      BeeFishWeb::WebRequest postWebRequest2;
      BString data;
      postWebRequest2.setOnData(
         [&data](const Data& buffer) {
            std::string string((const char*)buffer._data, buffer.size());
            data = string;
         }
      );

      // Post with anything
      ok &= testFile(
         "Post with content length text",
         "server/web-request/tests/request-text.txt",
         postWebRequest2,
         true
      );

      ok &= testResult(
         "Post with hello world body",
         data == "<h1>Hello world</h1>"
      );

      return ok;
   }
   
   inline bool testParts()
   {
      
      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      BeeFishWeb::WebRequest request;
      
      ok &= testFile(
         "WebRequest part 0",
         "server/web-request/tests/request-part-0.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok &= testFile(
         "WebRequest part 1",
         "server/web-request/tests/request-part-1.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok &= testFile(
         "WebRequest part 2",
         "server/web-request/tests/request-part-2.txt",
         request,
         true
      );
      
      ok &= testResult(
         "WebRequest object is valid",
         request.hasJSON()
      );

      cout << endl;
      
      return ok;
      
   }


   inline bool testJSON()
   {
      
      cout << "Test JSON" << endl;

      using namespace BeeFishJSON;
      
      bool ok = true;
        
      BeeFishWeb::WebRequest request;
      BeeFishBScript::BScriptParser parser(request);
      //BeeFishBScript::BScriptParser parser(request);

      ok &= testFile(
         parser,
         "POST with hello world sample",
         "server/web-request/tests/post-json.txt",
         request,
         true
      );

      ok &= testResult(
         "Post json value = World",
         parser.json()["Hello"] == "World"
      );

      BeeFishWeb::WebRequest request2;
      BeeFishBScript::BScriptParser parser2(request2);
      //BeeFishBScript::BScriptParser parser(request);

      ok &= testFile(
         parser2,
         "POST to settings",
         "server/web-request/tests/post-settings.txt",
         request2,
         true
      );

      ok &- testResult(
         "Post json value = World",
         parser2.json()["sleeping"] == false
      );

      cout << endl;


      return ok;
   }


   inline bool testPostImage()
   {
      
      cout << "Test Post Image" << endl;

      using namespace BeeFishJSON;
      
      bool ok = true;
        
      BeeFishWeb::WebRequest request;
      BeeFishBScript::BScriptParser parser(request);
      size_t count = 0;
      request.setOnData(
         [&count](const Data& data) {
            count += data.size();
         }
      );
      //BeeFishBScript::BScriptParser parser(request);

      ok &= testFile(
         parser,
         "POST Image",
         "server/web-request/tests/post-image.txt",
         request,
         true
      );
      
      cerr << "WEB-REQUEST TEST COUNT " << count << endl;

      ok &= testResult(
         "Content Type",
         request.headers()["content-type"] == "image/jpeg"
      );

      ok &= testResult(
         "Content Length",
         request.headers()["content-length"] == "235813"
      );

      cout << endl;


      return ok;
   }

#endif


}

#endif
