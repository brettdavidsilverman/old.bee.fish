#ifndef BEE_FISH_DATABASE__PAGE_H
#define BEE_FISH_DATABASE__PAGE_H

#include "branch.h"
#include "config.h"

using namespace std;

namespace bee::fish::database
{
   
   struct HeaderPage
   {
      char   _version[256];
      unsigned short   _pageSize;
      Index  _nextIndex;
   };
   
   union Page
   {
      HeaderPage _headerPage;
   };
   
}

#endif