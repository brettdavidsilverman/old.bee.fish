#include <fstream>
#include "version.h"
#include "power-encoding.h"

using namespace std;
using namespace bee::fish::power_encoding;

class Encoding : public PowerEncoding
{
protected:
   istream& _in;
   ostream& _out;
public:
   Encoding(istream& in, ostream& out) :
      _in(in),
      _out(out)
   {
   }
      
   virtual void writeBit(bool bit)
   {
      _out << ( bit ? '1' : '0' );
   }
   
   virtual bool readBit()
   {
      char bit;
      _in >> bit;
      return bit != '0';
   }
   
   virtual bool peekBit()
   {
      return _in.peek() != '0';
   }
   
   
};
   
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

   Encoding encoding(cin, cout);
   
   for (int i = 0; i < 256; i++) {
      encoding << (unsigned char)i;
      cout << endl;
   }

   
   
   cout << "String a" << endl;
   encoding << "a";
   cout << endl;
   
   cout << "WString a" << endl;
   encoding << L"a";
   cout << endl;
   
   cout << "4294967280L" << endl;
   encoding << 4294967280L;
   cout << endl;
   
   cout << "🍄" << endl;
   encoding << L"🍄";
   cout << endl;
   encoding << "🍄";
   cout << endl;
   
   stringstream stream;
   Encoding enc(stream, stream);
   
   enc << 8;
   int i;
   enc >> i;
   cout << i << endl;
   
   enc << "Hello World";
   string str;
   enc >> str;
   cout << str << endl;
   
   enc << L"🍄I love this planet";
   wstring wstr;
   enc >> wstr;
   wcout << wstr << endl;
   
   if (wstr == L"🍄")
      cout << "It works" << endl;
      
   return 0;
}
