#ifndef BEE_FISH__CONFIG_H
#define BEE_FISH__CONFIG_H

#if __has_include("../config.h")
   #include "../config.h"
#endif

#define LIVE
//#define LOCAL

#ifdef LOCAL
#define DISABLE_CACHE
#endif


#endif
