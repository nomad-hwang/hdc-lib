#pragma once

#include "platform.h"
#include "tick.hpp"
#include "uart_impl.hpp"

static UartDmaTransmitter *log_uart = nullptr;

void log_init(UartDmaTransmitter *_uart) {
  log_uart = _uart;
  log_uart->init();
}

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

const char *level_str[] = {"TRC", "DBG", "INF", "WRN", "ERR"};
const char *level_color[] = {COLOR_TRACE, COLOR_DEBUG, COLOR_INFO, COLOR_WARN, COLOR_ERROR};

#ifndef LOG_LEVEL
#define LOG_LEVEL (LEVEL_INFO)
#endif

#ifndef LOG_USE_COLOR
#define LOG_USE_COLOR (0)
#endif

// format: [timestamp] {color}[level]{color} [tag] message

static char __log_buf__[LOG_BUF_SIZE];

#if LOG_USE_COLOR
#define LOG(TAG, level, fmt, ...)                                                                                                                              \
  do {                                                                                                                                                         \
    sprintf(__log_buf__, "[%ld] [%s] [%s] %s" fmt "%s\r\n", Tick::get(), level_str[level], TAG, level_color[level], ##__VA_ARGS__, COLOR_END);                 \
    log_uart->write((uint8_t *)__log_buf__, strlen(__log_buf__));                                                                                              \
  } while (0)
#else
#define LOG(TAG, level, fmt, ...)                                                                                                                              \
  do {                                                                                                                                                         \
    char __log_buf__[LOG_BUF_SIZE];                                                                                                                            \
    sprintf(__log_buf__, "[%s] [%s] " fmt "\r\n", Tick::get(), TAG, ##__VA_ARGS__);                                                                            \
    log_uart->write((uint8_t *)__log_buf__, strlen(__log_buf__));                                                                                              \
  } while (0)
#endif

#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(fmt, ...) LOG(TAG, LEVEL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) LOG(TAG, LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(fmt, ...) LOG(TAG, LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_WARN
#define LOG_WARN(fmt, ...) LOG(TAG, LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(fmt, ...) LOG(TAG, LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif
