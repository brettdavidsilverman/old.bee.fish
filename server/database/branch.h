#ifndef BEE_FISH_DATABASE__BRANCH_H
#define BEE_FISH_DATABASE__BRANCH_H
#include "index.h"

using namespace std;

namespace bee::fish::database
{

   struct Branch
   {
      Index _left;
      Index _right;
      Index _data;
      
      bool isDeadEnd()
      {
         return _left  == 0 &&
                _right == 0;
      }
      
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         out << '{'
             << branch._left
             << ','
             << branch._right
             << '}';
          
         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif