#ifndef BEE_FISH_DATABASE__PAGE_H
#define BEE_FISH_DATABASE__PAGE_H

#include "branch.h"
#include "config.h"

using namespace std;

namespace bee::fish::database
{


   const Size
      branchesPerPage =
         floor(PAGE_SIZE / sizeof(Branch));
     
   struct BranchPage
   {
      Branch _branches[
         branchesPerPage
      ];
   };
   
   struct FreePage
   {
      Index _nextFreePage;
   };
   
   union Page
   {
      FreePage   _freePage;
      BranchPage _branchPage;
   };
   
}

#endif