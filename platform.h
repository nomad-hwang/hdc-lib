#pragma once

#include "stm32f7xx_hal.h"
#include <cassert>

#define LOG_LEVEL (LEVEL_TRC)
#define LOG_USE_COLOR (1)

#define UART_MAX_INSTANCES (8)
#define UART_RX_BUFFER_SIZE (256)
#define UART_TX_BUFFER_SIZE (256)

// LWRB
#define LWRB_DISABLE_ATOMIC (1)