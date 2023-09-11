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

#define LOG_BUF_SIZE (512)

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

inline static UartTransmitter *__log_uart__;
static char __log_buf__[LOG_BUF_SIZE];
static const char *level_str[] = {"TRC", "DBG", "INF", "WRN", "ERR"};
static const char *level_color[] = {COLOR_TRACE, COLOR_DEBUG, COLOR_INFO, COLOR_WARN, COLOR_ERROR};

#define log_init(_uart)                                                                                                                                        \
  do {                                                                                                                                                         \
    __log_uart__ = _uart;                                                                                                                                      \
    __log_uart__->init();                                                                                                                                      \
  } while (0)

#ifndef LOG_LEVEL
#define LOG_LEVEL (LEVEL_INFO)
#endif

#ifndef LOG_USE_COLOR
#define LOG_USE_COLOR (0)
#endif

// format: [timestamp] {color}[level]{color} [tag] message

#if LOG_USE_COLOR
#define __LOG__(TAG, level, fmt, ...)                                                                                                                          \
  do {                                                                                                                                                         \
    sprintf(__log_buf__, "[%ld] [%s] [%s] %s" fmt "%s\r\n", Tick::get(), level_str[level], TAG, level_color[level], ##__VA_ARGS__, COLOR_END);                 \
    __log_uart__->write((uint8_t *)__log_buf__, strlen(__log_buf__));                                                                                          \
  } while (0)
#else
#define __LOG__(TAG, level, fmt, ...)                                                                                                                          \
  do {                                                                                                                                                         \
    char __log_buf__[LOG_BUF_SIZE];                                                                                                                            \
    sprintf(__log_buf__, "[%s] [%s] " fmt "\r\n", Tick::get(), TAG, ##__VA_ARGS__);                                                                            \
    __log_uart__->write((uint8_t *)__log_buf__, strlen(__log_buf__));                                                                                          \
  } while (0)
#endif

#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(fmt, ...) __LOG__(TAG, LEVEL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) __LOG__(TAG, LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(fmt, ...) __LOG__(TAG, LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_WARN
#define LOG_WARN(fmt, ...) __LOG__(TAG, LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(fmt, ...) __LOG__(TAG, LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif
#endif // __LOG_H__
