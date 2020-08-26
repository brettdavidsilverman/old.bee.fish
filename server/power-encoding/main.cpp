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


   
 
   class Encoding : public PowerEncoding
   {
   protected:
      ostream& _out;
   public:
      Encoding(ostream& out) :
         _out(out)
      {
      }
      
      virtual void writeBit(bool bit)
      {
         _out << ( bit ? '1' : '0' );
      }
   };
   
   Encoding encoding(cout);
   
   for (int c = 0; c <= 255; ++c)
   {
      cout << c << '\t';
      encoding << c;
      cout << endl;
   }
   
   cout << "String a" << endl;
   encoding << "a";
   cout << endl;
   
   cout << "WString a" << endl;
   encoding << L"a";
   cout << endl;
  
   return 0;
}
