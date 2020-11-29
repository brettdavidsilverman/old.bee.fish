#include <iostream>
#include "atomic_flag.hpp"

using namespace std;

int main()
{

   cerr << "bee.fish.cgi.atomic-flag"
        << endl
        << "C++ run time: "
        << __cplusplus
        << endl;
   
   std::experimental::atomic_flag flag;
   if (!flag.test_and_set())
      flag.wait(false);
   
   return 0;
}
