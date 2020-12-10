#ifndef BEE_FISH_POWER_ENCODING__ENCODING
#define BEE_FISH_POWER_ENCODING__ENCODING

#include <iostream>
#include "power-encoding.h"

using namespace std;

namespace bee::fish::power_encoding
{

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



}

#endif