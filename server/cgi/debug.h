#ifndef BEE_FISH_DEBUG
   #define BEE_FISH_DEBUG

   #define DEBUG

   #ifdef DEBUG
      #define DEBUG_OUT(x) \
          std::cout << x << std::endl;
   #else
      #define DEBUG_OUT(x)
   #endif

#endif