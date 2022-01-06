#ifndef BEE_FISH_MISC__DEBUG
#define BEE_FISH_MISC__DEBUG

#ifdef DEBUG
   #define DEBUG_OUT(x) \
         std::cout << x;
#else
   #define DEBUG_OUT(x)
#endif

#endif