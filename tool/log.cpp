#include "log.hpp"

// #define __LOG__(TAG, level, fmt, ...)                                                                                                                          \
//   do {                                                                                                                                                         \
//     sprintf(__log_buf__ "[%ld] [%s] [%s] %s" fmt "%s\r\n", Tick::get(), level_str[level], TAG, level_color[level], ##__VA_ARGS__, COLOR_END);                 \
//     __log_uart__->write((uint8_t *)__log_buf__ strlen(__log_buf__);                                                                                          \
//   } while (0)

inline static UartTransmitter *__log_uart__;
static char __log_buf__[LOG_BUF_SIZE];
static const char *level_str[] = {"TRC", "DBG", "INF", "WRN", "ERR"};
static const char *level_color[] = {COLOR_TRACE, COLOR_DEBUG, COLOR_INFO, COLOR_WARN, COLOR_ERROR};

void log_init(UartTransmitter *uart) {
  __log_uart__ = uart;
  __log_uart__->init();
}

void log_write(const char *tag, int level, const char *fmt, ...) {
  int len = 0;

  len = sprintf(__log_buf__, "[%ld] [%s] [%s] %s", Tick::get(), level_str[level], tag, level_color[level]);
  __log_uart__->write((uint8_t *)__log_buf__, len);

  va_list args;
  va_start(args, fmt);
  len = vsprintf(__log_buf__, fmt, args);
  va_end(args);
  __log_uart__->write((uint8_t *)__log_buf__, len);

  __log_uart__->write((uint8_t *)COLOR_END, strlen(COLOR_END));
  __log_uart__->write((uint8_t *)"\r\n", 2);
}