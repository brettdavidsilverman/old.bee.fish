#ifndef BEE_FISH_DATABASE__BINARY_ENCODING_H
#define BEE_FISH_DATABASE__BINARY_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <math.h>


using namespace std;

namespace bee::fish::database
{


   //vector<void(BinaryEncoding& encoding)> chars;
   
   class BinaryEncoding
   {
   protected:
  
      virtual void writeBit(bool bit)
      {
         cout << (bit ? '1' : '0');
      }
      
      virtual bool readBit()
      {
         char c;
         cin >> c;
         return (c != '0');
      }
      
      virtual bool peekBit()
      {
         char c = (char)(cin.peek());
         return (c != '0');
      }
      
   public:
      BinaryEncoding()
      {
      }
     
      BinaryEncoding& 
      operator << (const string& binary)
      {
         
         for (const char c : binary)
         {
            bool bit = (c != '0');
            writeBit(bit);
         }
         
         return *this;
      }
     
      BinaryEncoding& operator >> (string& value)
      {
      
         throw runtime_error("Not implemented yet");
      }
      
   };
   
}
#endif