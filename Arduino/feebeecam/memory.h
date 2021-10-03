#ifndef FEEBEECAM_MEMORY
#define FEEBEECAM_MEMORY
#include <stdlib.h>

#include "light.h"

#define malloc(x) my_malloc(x)
#define realloc(x, n) my_realloc(x, n)
#define calloc(x, n) my_calloc(x, n)

#ifdef WEB_SERVER2
extern bool gettingImage;
#endif

bool psramInitialized = false;

void initializeMemory() {

  if (!psramInitialized) {

    psramInit();

    psramInitialized = true;

  }

  
}

#ifdef __cplusplus
extern "C" {
#endif

void* my_malloc(size_t size) {

  initializeMemory();
/*
#ifdef WEB_SERVER2
  if (gettingImage)
    return heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
#endif
*/
  void* allocated = ps_malloc(size);
/*
  Serial.print("M{");
  Serial.print(size);
  Serial.print(", 0x");
  Serial.print((uint32_t)allocated, HEX);
  Serial.println("}");
*/
  return allocated; 



}


void *my_realloc(void *ptr, size_t size) {
  initializeMemory();
/*
#ifdef WEB_SERVER2
  if (gettingImage)
    return heap_caps_realloc(ptr, size, MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
#endif
*/
  return ps_realloc(ptr, size);  
}

void *my_calloc(size_t n, size_t size) {
  initializeMemory();
/*
#ifdef WEB_SERVER2
  if (gettingImage)
    return heap_caps_calloc(n, size, MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
#endif
*/
  return ps_calloc(n, size);
}


#ifdef __cplusplus
}
#endif

void *operator new(size_t size)
{
//  return ::operator new(size);
  return my_malloc(size);
}

void operator delete(void *pointer)
{
//  return ::operator delete(pointer);
  free(pointer);
}

#endif
