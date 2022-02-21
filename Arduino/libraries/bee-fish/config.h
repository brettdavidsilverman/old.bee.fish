#ifndef __CONFIG_H
#define __CONFIG_H

#define LIVE
//#define DEBUG

#ifdef DEBUG
   #define HOST_NAME "laptop"
#else
   #define HOST_NAME "bee.fish"
#endif

#define HOST "https://" HOST_NAME


#endif
