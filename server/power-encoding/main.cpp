#include <fstream>
#include <array>
#include "version.h"
#include "power-encoding.h"

using namespace std;
using namespace bee::fish::power_encoding;

class Encoding : public PowerEncoding
{
protected:
   istream& _in;
   ostream& _out;
   long _count;
   
public:
   Encoding(istream& in, ostream& out) :
      _in(in),
      _out(out)
   {
      _count = 0;
   }
      
   virtual void writeBit(bool bit)
   {
#ifdef DEBUG
      cerr << 'w' << (bit ? '1' : '0');
#endif
      if (bit)
         ++_count;
      else
         --_count;
         
      _out << ( bit ? '1' : '0' );
   }
   
   virtual bool readBit()
   {
      if (_in.eof())
         throw runtime_error("End of input stream");
         
      char bit;
      _in >> bit;
      
#ifdef DEBUG
      cerr << 'r' << bit;
#endif

      bool b = ( bit != '0' );
      if (b)
         ++_count;
      else
         --_count;
         
      
      return b;
   }
   
   virtual bool peekBit()
   {
      return _in.peek() != '0';
   }
   
   long count()
   {
      return _count;
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
   
   std::stringstream stream1;
   Encoding encoding1(stream1, stream1);
   encoding1.operator <<  < int[3], int > ({0, 1, 2});

   cerr << endl;
   vector<int> a;
   encoding1 >> a;
   cerr << endl;
   
   for (auto v : a)
      cerr << v << endl;
   

   Encoding encoding(cin, cout);
   
   for (int i = 0; i < 256; i++) {
      encoding << (unsigned char)i;
      cout << endl;
   }

   cout << "Count: " << encoding.count() << endl;
   
   cout << "String a" << endl;
   encoding << "a";
   cout << endl;
   cout << "Count: " << encoding.count() << endl;

   cout << "WString a" << endl;
   encoding << L"a";
   cout << endl;
   cout << "Count: " << encoding.count() << endl;

   cout << "4294967280L" << endl;
   encoding << 4294967280L;
   cout << endl;
   cout << "Count: " << encoding.count() << endl;

   wcout << L"🍄" << endl;
   encoding << L"🍄";
   wcout << endl;
   wcout << "Count: " << encoding.count() << endl;

   stringstream stream;
   Encoding enc(stream, stream);
   
   cerr << "8 stream" << endl;
   enc << 8;
   cerr << endl;
   int i;
   enc >> i;

   cout << i << endl;
   cout << "Count: " << enc.count() << endl;

   enc << "Hello World";
   string str;
   enc >> str;
   cout << str << endl;
   cout << "Count: " << enc.count() << endl;
   
   enc << L"🍄I love this planet";
   wstring wstr;
   enc >> wstr;
   wcout << wstr << endl;
   cout << "Count: " << enc.count() << endl;
   
   if (wstr == L"🍄I love this planet")
      cout << "It works" << endl;
      
   return 0;
}
