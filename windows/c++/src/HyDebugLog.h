#ifndef __HY_DEBUG_LOG_H
#define __HY_DEBUG_LOG_H

#include <string>

#define TR_BUILDING        0x0001    // Creating unit, unit is destroyed.
#define TR_UNITS           0x0002    // unit is created, destroyed and upgraded.
#define TR_UNIT_ACTIONS    0x0004    // unit is working, attacking, searching and in more actions.
#define TR_GAME            0x0008    // Game related event, such as game start, game over and so on.
#define TR_SCOUTING        0x0010    // Scouting
#define TR_RESOURCE        0x0020

#define TR_TRACER          0x0040
#define TR_TRACER_ASCII    0x0080
#define TR_TRACER_DEBUGGER 0x0100
#define TR_TRACER_BOOT     0x0200
#define TR_API_ANALYZE     0x0400

#define LOG_ZONE 0xFFFF

#define TR_ERROR   0x0001
#define TR_WARNING 0x0002
#define TR_INFO    0x0003
#define TR_DEBUG   0x0004

#define LOG_LEVEL TR_INFO

extern int g_nFrames;

#ifdef TRACER_NO_LOG
#define TRACER_LOG(level, fmt, ...) {}
#else
#define TRACER_LOG(level, fmt, ...) TR_LOG(level, TR_TRACER, fmt,##__VA_ARGS__)
#endif

#if defined(WIN32) || defined(__APPLE__) || defined(LINUX)

#include <stdio.h>
#define TR_LOG(level, zone, fmt, ...)                      \
    do                                                     \
    {                                                      \
        if (zone & LOG_ZONE)                               \
        {                                                  \
            if (level == TR_ERROR && level <= LOG_LEVEL)   \
            {                                              \
                printf("%s", __FUNCTION__);                \
                printf("#%06d ", g_nFrames);               \
                printf(#zone);                             \
                printf(":ERROR: ");                        \
                printf(fmt,##__VA_ARGS__);                 \
            }                                              \
            if (level == TR_WARNING && level <= LOG_LEVEL) \
            {                                              \
                printf("%s", __FUNCTION__);                \
                printf("#%06d ", g_nFrames);               \
                printf(#zone);                             \
                printf(":WARN: ");                         \
                printf(fmt,##__VA_ARGS__);                 \
            }                                              \
            if (level == TR_INFO && level <= LOG_LEVEL)    \
            {                                              \
                printf("%s", __FUNCTION__);                \
                printf("#%06d ", g_nFrames);               \
                printf(#zone);                             \
                printf(":INFO: ");                         \
                printf(fmt,##__VA_ARGS__);                 \
            }                                              \
            if (level == TR_DEBUG && level <= LOG_LEVEL)   \
            {                                              \
                printf("%s", __FUNCTION__);                \
                printf("#%06d ", g_nFrames);               \
                printf(#zone);                             \
                printf(":DEBUG: ");                        \
                printf(fmt,##__VA_ARGS__);                 \
            }                                              \
        }                                                  \
    }                                                      \
    while (0)
#else
#include <android/log.h>

#define TR_LOG(level, zone, fmt, ...)                                             \
    do                                                                            \
    {                                                                             \
        if (zone & LOG_ZONE)                                                      \
        {                                                                         \
            if (level == TR_ERROR && level <= LOG_LEVEL)                          \
            {                                                                     \
                __android_log_print(ANDROID_LOG_ERROR, #zone, fmt,##__VA_ARGS__); \
            }                                                                     \
            if (level == TR_WARNING && level <= LOG_LEVEL)                        \
            {                                                                     \
                __android_log_print(ANDROID_LOG_WARN, #zone, fmt,##__VA_ARGS__);  \
            }                                                                     \
            if (level == TR_INFO && level <= LOG_LEVEL)                           \
            {                                                                     \
                __android_log_print(ANDROID_LOG_INFO, #zone, fmt,##__VA_ARGS__);  \
            }                                                                     \
            if (level == TR_DEBUG && level <= LOG_LEVEL)                          \
            {                                                                     \
                __android_log_print(ANDROID_LOG_DEBUG, #zone, fmt,##__VA_ARGS__); \
            }                                                                     \
        }                                                                         \
    }                                                                             \
    while (0)
#endif // WIN32

#endif // __HY_DEBUG_LOG_H