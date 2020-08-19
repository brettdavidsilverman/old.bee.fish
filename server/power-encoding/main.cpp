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
   for (int c = 0; c <= 255; ++c)
   {
      cout << c << '\t';
      cout << '1';
      power << c;
      cout << endl;
   }
   /*
   power << "Hello world";
   cout << "*****";
   */
   return 0;
}
