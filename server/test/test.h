#ifndef BEE_FISH_TEST__TEST
#define BEE_FISH_TEST__TEST

#include <fstream>
#include <filesystem>
#include "../parser/parser.h"
#include "../b-string/string.h"


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
      
      // open the sample session file
      ifstream input(file);
      Parser parser(match);
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

}

#endif