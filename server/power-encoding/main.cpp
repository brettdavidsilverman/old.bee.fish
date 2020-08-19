#include "../version.h"
#include "power-encoding.h"

using namespace std;
using namespace bee::fish::power_encoding;

int main(int argc, char* argv[])
{
   cerr << "bee.fish.server.power-encoding "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Build date  : "
           << (unsigned long) &BEE_FISH_BUILD_DATE
           << endl
        << "Build number: "
           << (unsigned long) &BEE_FISH_BUILD_NUMBER
           << endl;

   PowerEncoding power(cout);
   for (int i = 0; i <= 10; ++i)
   {
      cout << i << '\t';
      power << i;
      cout << endl;
   }
   /*
   power << "Hello world";
   cout << "*****";
   */
   return 0;
}
