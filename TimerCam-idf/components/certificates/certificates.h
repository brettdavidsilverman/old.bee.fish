#ifndef FEEBEECAM_CERTS
#define FEEBEECAM_CERTS

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");

extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");

#ifdef __cplusplus
}
#endif

#endif
