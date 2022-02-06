#include <iostream>
#include <filesystem>

#include "../config.h"
#include "../parser/parser.h"
#include "../parser/test.h"
#include "version.h"
#include "web-response.h"
#include "test.h"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;

int main(int argc, const char* argv[]) {

   cout << "bee.fish.web-response"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_WEB_RESPONSE_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing web-response..." << endl << endl;
      if (!BeeFishWeb::testResponse())
         return 1;
   }

   return 0;
}
