#ifndef BEE_FISH_MISC__PAGESIZE
#define BEE_FISH_MISC__PAGESIZE

#ifndef SERVER
inline int getpagesize() {
    return 1024;
}
#endif

#endif