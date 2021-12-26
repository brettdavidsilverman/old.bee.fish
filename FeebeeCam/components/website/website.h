#ifndef FEEBEECAM_WEBSITE
#define FEEBEECAM_WEBSITE

#include <bee-fish.h>
#include "files.h"

namespace FeebeeCam 
{
    DECLARE_FILE("error.js", error_js);
    DECLARE_FILE("setup.html", setup_html);
    DECLARE_FILE("beehive.html", beehive_html);
    DECLARE_FILE("sha512.js", sha512_js);
    DECLARE_FILE("full-screen.js", full_screen_js);
    DECLARE_FILE("winnie-black.jpg", winnie_black_jpg);
    DECLARE_FILE("winnie.jpg", winnie_jpg);
    DECLARE_FILE("loading-brown.gif", loading_brown_gif)
    DECLARE_FILE("test.txt", test_txt);
    DECLARE_FILE("fetch.js", fetch_js);
    DECLARE_FILE("red-small.jpg", red_small_jpg);
    DECLARE_FILE("green-small.jpg", green_small_jpg);
//    DECLARE_FILE("feebeecam.local.crt", feebeecam_local_crt);
//    DECLARE_FILE("feebeecam.local.key", feebeecam_local_key);

    inline static Files _files = {
        FILE("error.js", error_js, "text/javascript"),
        FILE("setup.html", setup_html, "text/html; charset=UTF-8"),
        FILE("beehive.html", beehive_html, "text/html; charset=UTF-8"),
        FILE("sha512.js", sha512_js, "text/javascript"),
        FILE("fetch.js", fetch_js, "text/javascript"),
        FILE("full-screen.js", full_screen_js, "text/javascript"),
        FILE("winnie-black.jpg", winnie_black_jpg, "image/jpeg"),
        FILE("loading-brown.gif", loading_brown_gif, "image/gif"),
        FILE("winnie.jpg", winnie_jpg, "image/jpeg"),
        FILE("green-small.jpg", green_small_jpg, "image/jpeg"),
        FILE("red-small.jpg", red_small_jpg, "image/jpeg"),
        FILE("test.txt", test_txt, "text/plain; charset=UTF-8")//,
 //       FILE("feebeecam.local.crt", feebeecam_local_crt, "certificate"),
 //       FILE("feebeecam.local.key", feebeecam_local_key, "certificate")
    };

}

#endif

