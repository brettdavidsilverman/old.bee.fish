#ifndef FEEBEECAM_MEMORY
#define FEEBEECAM_MEMORY
#include <stdlib.h>
#include "esp_system.h" //This inclusion configures the peripherals in the ESP system.
#include "esp32-hal-psram.h"

#undef malloc
#undef realloc
#undef calloc

#define malloc(x) my_malloc(x)
#define realloc(x, n) my_realloc(x, n)
#define calloc(x, n) my_calloc(x, n)

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
  return ps_malloc(size);
}


void *my_realloc(void *ptr, size_t size) {
  initializeMemory();
  return ps_realloc(ptr, size);  
}

void *my_calloc(size_t n, size_t size) {
  initializeMemory();
  return ps_calloc(n, size);
}


#ifdef __cplusplus
}
#endif

void *operator new(size_t size)
{
  return my_malloc(size);
}

void *operator new[] (size_t size)
{
  return my_malloc(size);
}

void operator delete(void *pointer) noexcept
{
  free(pointer);
}

void operator delete[](void *pointer) noexcept
{
  free(pointer);
}

#endif
