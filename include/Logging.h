/**
 * Logging.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_LOGGING_H__
#define __OVLSNAP_LOGGING_H__

#include <time.h>
#include <iostream>


// TODO: Add file/syslog logging here.
#ifdef _DEBUG_
    #define LOG_ERROR(message) do { \
        std::cerr << "[" << timenow() << "] [ERROR] " << message << std::endl; \
    } while (0)
#else
    #define LOG_ERROR(message)
#endif


// TODO: Add file/syslog logging here.
#ifdef _DEBUG_
    #define LOG_WARN(message) do { \
        std::cerr << "[" << timenow() << "] [WARNING] " << message << std::endl; \
    } while (0)
#else
    #define LOG_WARN(message)
#endif


// TODO: Add file/syslog logging here.
#ifdef _DEBUG_
    #define LOG_INFO(message) do { \
        std::cout << "[" << timenow() << "] [INFO] " << message << std::endl; \
    } while (0)
#else
    #define LOG_INFO(message)
#endif


// TODO: Add file/syslog logging here.
#ifdef _DEBUG_
    #define LOG_DEBUG(message) do { \
        std::cout << "[" << timenow() << "] [DEBUG] " << message << std::endl; \
    } while (0)
#else
    #define LOG_DEBUG(message)
#endif


static inline char* timenow() {
    static char buffer[64];
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);

    return buffer;
}

#endif
