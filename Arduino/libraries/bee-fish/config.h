#ifdef SERVER
#undef SERVER
#endif

//#define DEBUG

#ifndef DEBUG
#define LIVE
#endif

//#define DEBUG_PARSER

#ifdef DEBUG
   #define HOST_NAME "laptop"
   //#define HOST_NAME "192.168.95.162"
#else
   #define HOST_NAME "bee.fish"
#endif


#define HOST "https://" HOST_NAME


