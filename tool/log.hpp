#ifndef __LOG_H__
#define __LOG_H__

#include <cstring>
#include <stdarg.h>
#include <stdio.h>

#include "platform.h"
#include "tick.hpp"
#include "uart.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_BUF_SIZE (256)

#define LEVEL_TRACE (0)
#define LEVEL_DEBUG (1)
#define LEVEL_INFO (2)
#define LEVEL_WARN (3)
#define LEVEL_ERROR (4)

#define COLOR_END "\033[0m"      // reset
#define COLOR_TRACE "\033[0;37m" // white
#define COLOR_DEBUG "\033[0;36m" // cyan
#define COLOR_INFO "\033[0;32m"  // green
#define COLOR_WARN "\033[0;33m"  // yellow
#define COLOR_ERROR "\033[0;31m" // red

#ifndef LOG_LEVEL
#define LOG_LEVEL (LEVEL_INFO)
#endif

#ifndef LOG_USE_COLOR
#define LOG_USE_COLOR (0)
#endif

// format: [timestamp] {color}[level]{color} [tag] message

extern void log_init(UartTransmitter *uart);
extern void log_write(const char *tag, int level, const char *fmt, ...);

#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(fmt, ...) log_write(TAG, LEVEL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) log_write(TAG, LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(fmt, ...) log_write(TAG, LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_WARN
#define LOG_WARN(fmt, ...) log_write(TAG, LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(fmt, ...) log_write(TAG, LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif
#endif // __LOG_H__
