#ifndef BEE_FISH_DATABASE__PAGE_H
#define BEE_FISH_DATABASE__PAGE_H

#include "branch.h"
#include "config.h"

using namespace std;

namespace bee::fish::database
{


   inline const Size
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
   
   struct Header
   {
      char   _version[256];
      unsigned short   _pageSize;
      Index  _next;
   };
   
   union Page
   {
      FreePage   _freePage;
      BranchPage _branchPage;
      Header     _headerPage;
   };
   
}

#endif