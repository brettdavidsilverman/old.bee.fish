#ifndef BEE_FISH_DATABASE__BRANCH_H
#define BEE_FISH_DATABASE__BRANCH_H
#include "index.h"
#include "../b-script/b-script.h"

using namespace std;

namespace BeeFishDatabase
{

   struct Branch
   {
      Index _parent;
      Index _left;
      Index _right;
      Index _dataIndex;
      
      bool isDeadEnd()
      {
         return _left  == 0 &&
                _right == 0;
      }
      
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         using namespace BeeFishBScript;

         Object object = {
            {"parent", (Number)branch._parent},
            {"left",   (Number)branch._left},
            {"right",  (Number)branch._right}
         };

         out << object;
          
         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif