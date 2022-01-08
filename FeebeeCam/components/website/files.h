#ifndef FEEBEECAM_FILES
#define FEEBEECAM_FILES

#include <map>

#include <bee-fish.h>

namespace FeebeeCam 
{

    struct File {
        BString _fileName;
        const unsigned char* _data;
        size_t _length;
        BString _contentType;
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
                (size_t)(ESCAPED_NAME ## _end) - (size_t)(ESCAPED_NAME ## _start), \
                CONTENT_TYPE \
             } \
        )
}

#endif
