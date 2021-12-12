#ifndef BEE_FISH_TEST__TEST
#define BEE_FISH_TEST__TEST

#include <fstream>
#include <filesystem>
#include "../parser/parser.h"
#include "../b-string/string.h"
#include "../json/json-parser.h"

namespace BeeFishTest
{

#ifdef SERVER
   using namespace std::filesystem;
#endif

   using namespace BeeFishParser;

   inline bool testResult(
      string label,
      bool ok
   )
   {
      cout << label << ":\t";
      
      if (ok)
         cout << "ok";
      else
         cout << "FAIL";

      cout << endl;
      
      return ok;
   }
   
   inline int hasArg(
      int argc,
      const char* argv[],
      const string& arg
   )
   {
      for (int i = 0; i < argc; i++)
      {
         if (arg == argv[i])
            return i;
      }
   
      return -1;
   }

#ifdef SERVER
   inline bool testFile(
      string label,
      path file,
      BeeFishParser::Match& match,
      BeeFishMisc::optional<bool> result
   )
   {
      bool ok = true;
      
      file = BString(FILE_SYSTEM_PATH) + BString("/") + BString(file);

      if (!exists(file)) {
         cout << "File not found: " << file << endl;
         return false;
      }

      // open the sample session file
      ifstream input(file);
      BeeFishJSON::JSONParser parser(match);
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
#endif

   inline bool testMatch(
      BString label,
      Match* match,
      string text,
      BeeFishMisc::optional<bool> result = false,
      BString expected = {}
   )
   {
      cout << label << ":\t";
      
      bool ok = true;

      BeeFishJSON::JSONParser parser(*match);
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
         cout << "FAIL. Got  ";
         if (match->result() == true)
            cout << "true";
         else if (match->result() == false)
            cout << "false";
         else
            cout << "?";
         cout << endl;
         cout << "\tTested   " << text << endl;
         cout << "\tExpected " << expected << endl;
         cout << "\tCaptured " << value << endl;
      }
      
      return ok;
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