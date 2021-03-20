#include <fstream>
#include <array>
#include "version.h"
#include "power-encoding.h"
#include "encoding.h"
#include "../https/basic-authorization.h"

using namespace std;
using namespace bee::fish::b_string;
namespace bee::fish::power_encoding
{
   bool test();
}

using namespace bee::fish::power_encoding;

int main(int argc, char* argv[])
{
   cerr << "bee.fish.server.power-encoding "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_POWER_ENCODING_VERSION
           << endl;
   
   bee::fish::power_encoding::test();
   
   Encoding encoding(cin, cout);
   
   while (!cin.eof())
   {
   
      BString line;
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
      
      encoding << line;
      
      cout << endl;
      
   }
   
   return 0;
}
