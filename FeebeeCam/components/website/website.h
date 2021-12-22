#ifndef FEEBEECAM_WEBSITE
#define FEEBEECAM_WEBSITE

#include <bee-fish.h>
#include <map>

namespace FeebeeCam 
{

    struct File {
        const BString _fileName;
        const unsigned char* _data;
        size_t _length;
        const BString _contentType;
    };
    
    typedef std::map<BeeFishBString::BString, File> Files;

    #define DECLARE_FILE(NAME, ESCAPED_NAME) \
        extern const unsigned char ESCAPED_NAME ## _start[] asm("_binary_" #ESCAPED_NAME "_start"); \
        extern const unsigned char ESCAPED_NAME ## _end[] asm("_binary_" #ESCAPED_NAME "_end");

    #define FILE(NAME, ESCAPED_NAME, CONTENT_TYPE) \
        std::make_pair(NAME, \ 
            FeebeeCam::File{ \
                NAME, \
                ESCAPED_NAME ## _start, \
                ESCAPED_NAME ## _end - ESCAPED_NAME ## _start, \
                CONTENT_TYPE \
             } \
        )

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

    inline static Files _files = {
        FILE("error.js", error_js, "application/javascript"),
        FILE("setup.html", setup_html, "text/html; charset=UTF-8"),
        FILE("beehive.html", beehive_html, "text/html; charset=UTF-8"),
        FILE("sha512.js", sha512_js, "application/javascript"),
        FILE("fetch.js", fetch_js, "application/javascript"),
        FILE("full-screen.js", full_screen_js, "application/javascript"),
        FILE("winnie-black.jpg", winnie_black_jpg, "image/jpeg"),
        FILE("loading-brown.gif", loading_brown_gif, "image/gif"),
        FILE("winnie.jpg", winnie_jpg, "image/jpeg"),
        FILE("green-small.jpg", green_small_jpg, "image/jpeg"),
        FILE("red-small.jpg", red_small_jpg, "image/jpeg"),
        FILE("test.txt", test_txt, "text/plain; charset=UTF-8")
    };

/*
    #define FILE(NAME, ESCAPED_NAME) \
        extern const unsigned char ESCAPED_NAME ## _start[] asm("_binary_" #ESCAPED_NAME "_start"); \
        _files[NAME] = File{ESCAPED_NAME ## _start, (ESCAPED_NAME ## _end - ESCAPED_NAME ## _start)};

    FILE("setup.html", setup_html);
    //FILE("beehive.html", beehive_html);
*/
    //extern const unsigned char setup_html_start[] asm("_binary_setup_html_start");
    //extern const unsigned char setup_html_end[]   asm("_binary_setup_html_end");

    //extern const unsigned char beehive_html_start[] asm("_binary_beehive_html_start");
    //extern const unsigned char beehive_html_end[]   asm("_binary_beehive_html_end");

}

#endif

