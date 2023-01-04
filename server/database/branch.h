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
         out << '{'
             << "left: " << branch._left << ", "
             << "right: " << branch._right << ", "
             << "dataIndex: " <<  branch._dataIndex
             << '}';
          
         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif