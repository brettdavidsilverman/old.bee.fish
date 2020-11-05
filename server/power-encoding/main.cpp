#include <fstream>
#include <array>
#include "version.h"
#include "power-encoding.h"
#include "../https/basic-authorization.h"

using namespace std;

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
   
   return 0;
}
