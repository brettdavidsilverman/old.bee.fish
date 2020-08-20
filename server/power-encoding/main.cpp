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


   
   
   Counter counter(cout);
   
   PowerEncoding power(counter);
   for (int c = 0; c <= 255; ++c)
   {
      cout << c << '\t';
      counter << '1';
      power << c;
      cout << '\t' << counter.counter();
      counter << Counter::endl;
   }
   
   cout << "String a" << endl;
   counter << '1';
   power << "a";
   cout << endl;
   
   cout << "WString a" << endl;
   counter << '1';
   power << L"a";
   cout << endl;
   /*
   power << "Hello world";
   cout << "*****";
   */
   
   cout << "Tally:\t" << counter.counter() << endl;
   return 0;
}
