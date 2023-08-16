#pragma once

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include <cassert>

#define LOG_LEVEL (LEVEL_TRC)
#define LOG_USE_COLOR (1)

#define UART_MAX_INSTANCES (8)
#define UART_RX_BUFFER_SIZE (512)
#define UART_TX_BUFFER_SIZE (512)

// LWRB
#define LWRB_DISABLE_ATOMIC (1)

// ROS
#define ROS_MAX_SUBSCRIBERS (25)
#define ROS_MAX_PUBLISHERS (25)
#define ROS_INPUT_BUFFER_SIZE (UART_RX_BUFFER_SIZE)
#define ROS_OUTPUT_BUFFER_SIZE (UART_TX_BUFFER_SIZE)
