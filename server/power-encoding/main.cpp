#include <fstream>
#include "version.h"
#include "power-encoding.h"

using namespace std;
using namespace bee::fish::power_encoding;

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
   
void writeCharBitsHeader();

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
#ifdef GEN_CHAR_BITS
   writeCharBitsHeader();
#else
   Encoding encoding(cout);
   
   for (int i = 0; i < 256; i++) {
      encoding << (unsigned char)i;
      cout << endl;
   }
#endif
   
   
   cout << "String a" << endl;
   encoding << "a";
   cout << endl;
   
   cout << "WString a" << endl;
   encoding << L"a";
   cout << endl;
   
   return 0;
}

void writeCharBitsHeader()
{

   
   ofstream header("char-bits.h", std::ofstream::trunc);
   Encoding encoding(header);
   
   header << "#include <vector>" << endl;
   header << "inline std::vector<std::string> _charBits = {" << endl;;
   for (int c = 0; c <= 255; ++c)
   {
      header << "\"1";
      encoding << c;
      header << "\"";
      if (c != 255)
         header << ",";
      header << endl;
   }
   header << "};";
}
