#ifndef BEE_FISH_TEST__TEST
#define BEE_FISH_TEST__TEST

#include <fstream>
#ifdef SERVER
#include <filesystem>
#endif

#include "../parser/parser.h"
#include "../b-string/string.h"
#include "../json/json-parser.h"

#include "test-result.h"

namespace BeeFishTest
{

#ifdef SERVER
   using namespace std::filesystem;
#endif

   using namespace BeeFishParser;


#ifdef SERVER
   inline bool testFile(
      BeeFishJSON::JSONParser& parser,
      string label,
      path file,
      BeeFishParser::Match& match,
      BeeFishMisc::optional<bool> result
   )
   {
      bool ok = true;
      
      file = string(FILE_SYSTEM_PATH) + "/" + string(file);

      if (!exists(file)) {
         cout << "File not found: " << file << endl;
         return false;
      }

      // open the sample session file
      ifstream input(file);
      parser.read(input);
      
      ok &= testResult(
         label,
         (match.result() == result)
      );

      if (!ok) {
         cout << "Expected: " << result << endl;
         cout << "Got: " << match.result() << endl;
      }
      
      input.close();
      
      return ok;
   }

   inline bool testFile(
      string label,
      path file,
      BeeFishParser::Match& match,
      BeeFishMisc::optional<bool> result
   )
   {
      BeeFishJSON::JSONParser parser(match);
      return testFile(parser, label, file, match, result);
   }
#endif

   inline bool testMatch(
      Parser& parser,
      BString label,
      Match* match,
      string text,
      BeeFishMisc::optional<bool> result = false,
      BString expected = {}
   )
   {
      cout << label << ":\t";
      
      bool ok = true;

      parser.read(text);
      
      BString value;

      if (match->matched())
         value = match->value();

      ok = (result == match->result());

      if (match->matched() && expected.size())
      {
         if (value != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL. Expected "
              << result
              << " Got  "
              << parser.result()
              << endl;

         cout << "\tTested   " << text << endl;
         cout << "\tExpected " << expected << endl;
         cout << "\tCaptured " << value << endl;
      }
      
      return ok;
   }
   
   inline bool testMatch(
      BString label,
      Match* match,
      string text,
      BeeFishMisc::optional<bool> result = false,
      BString expected = {}
   ) 
   {
      BeeFishJSON::JSONParser parser(*match);
      return testMatch(parser, label, match, text, result, expected);
   }

   inline bool testMatchDelete(
      BString label,
      Match* parser,
      string text,
      BeeFishMisc::optional<bool> result = false,
      BString expected = {}
   )
   {
      bool ok = testMatch(label, parser, text, result, expected);
      delete parser;
      return ok;
   }
   

}

#endif