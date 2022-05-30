#ifndef BEE_FISH_MISC__PAGESIZE
#define BEE_FISH_MISC__PAGESIZE

#ifdef SERVER
#include <unistd.h>
#else
extern "C" {
inline int getpagesize() {
    return 1024;
}
}
#endif

#endif