#ifndef __CONFIG_H
#define __CONFIG_H

//#define DEBUG

#ifndef DEBUG
#define LIVE
#endif


//#define DEBUG_PARSER

#ifdef DEBUG
//   #define HOST_NAME "laptop"
   #define HOST_NAME "192.168.52.162"
#else
   #define HOST_NAME "bee.fish"
//   #define HOST_NAME "34.116.73.229"
#endif

#define HOST "https://" HOST_NAME


#endif
