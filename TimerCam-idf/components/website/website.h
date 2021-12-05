#ifndef FEEBEECAM_WEBSITE
#define FEEBEECAM_WEBSITE

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char setup_html_start[] asm("_binary_setup_html_start");
extern const unsigned char setup_html_end[]   asm("_binary_setup_html_end");

extern const unsigned char beehive_html_start[] asm("_binary_beehive_html_start");
extern const unsigned char beehive_html_end[]   asm("_binary_beehive_html_end");

#ifdef __cplusplus
}
#endif

#endif
