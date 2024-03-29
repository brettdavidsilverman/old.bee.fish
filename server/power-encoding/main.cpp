#include <fstream>
#include <array>
#include "../b-string/b-string.h"
#include "version.h"
#include "power-encoding.h"
#include "test.h"

using namespace std;
using namespace BeeFishBString;
using namespace BeeFishPowerEncoding;

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
   
   BeeFishPowerEncoding::test();
   
   EncodeToStream encoding(cin, cout);
   
   while (!cin.eof())
   {
   
      BString line;
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
      
      Data data = Data::fromBase64(line);
      BitStream stream = BitStream::fromData(data);
      
      BString string;
      stream >> string;
      
      cout << string
           << endl;
      
   }
   
   return 0;
}
