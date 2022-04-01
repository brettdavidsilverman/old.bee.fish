#ifndef __CONFIG_H
#define __CONFIG_H

#define DEBUG

#ifndef DEBUG
#define LIVE
#endif


//#define DEBUG_PARSER

#ifdef DEBUG
   #define HOST_NAME "laptop"
#else
   #define HOST_NAME "bee.fish"
#endif

#define HOST "https://" HOST_NAME


#endif
