#ifndef BEE_FISH_WEB__TEST_RESPONSE_H
#define BEE_FISH_WEB__TEST_RESPONSE_H
#include "../test/test.h"

#include "../json/json-parser.h"
#include "web-response.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

using namespace BeeFishTest;

namespace BeeFishWeb
{

   inline bool testResponse()
   {
   
      bool ok = true;

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
/*   
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

      ok &= testFile(
         parser2,
         "WebRequest with full json 2",
         "server/web-request/tests/request-full.txt",
         request2,
         true
      );

      return ok;


   }

*/

}

#endif
