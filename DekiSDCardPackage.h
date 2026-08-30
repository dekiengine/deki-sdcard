#pragma once

#ifdef _WIN32
    #ifdef DEKI_SDCARD_EXPORTS
        #define DEKI_SDCARD_API __declspec(dllexport)
    #else
        #define DEKI_SDCARD_API __declspec(dllimport)
    #endif
#else
    #define DEKI_SDCARD_API __attribute__((visibility("default")))
#endif
