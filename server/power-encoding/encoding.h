#ifndef BEE_FISH_POWER_ENCODING__ENCODING
#define BEE_FISH_POWER_ENCODING__ENCODING

#include <iostream>
#include "power-encoding.h"

using namespace std;

namespace bee::fish::power_encoding
{

   class EncodeToStream : public PowerEncoding
   {
   protected:
      wistream& _in;
      wostream& _out;
      long _count;
   
   public:
      EncodeToStream(wistream& in, wostream& out) :
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
         
         wchar_t bit;
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
   
   class EncodeToBits : public PowerEncoding
   {
   protected:
      vector<bool> _bits;
      size_t _position = -1;
      long _count = 0;
      
   public:
      EncodeToBits()
      {

      }
      
      EncodeToBits(vector<bool> bits) :
         _bits(bits)
      {
      }
      
      virtual void writeBit(bool bit)
      {
         if (bit)
            ++_count;
         else
            --_count;
            
         _bits.push_back(bit);
      }
   
      virtual bool readBit()
      {
         if ( _position != (size_t)(-1) &&
              _position >= _bits.size() )
            throw runtime_error("End of bits");
         
         bool bit = _bits[++_position];
         
         if (bit)
            ++_count;
         else
            --_count;

         return bit;
      }
   
      virtual bool peekBit()
      {
         return _bits[_position + 1];
      }
  
      const vector<bool>& bits() const
      {
         return _bits;
      }
      
      long count()
      {
         return _count;
      }
   
   };



}

#endif