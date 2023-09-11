#pragma once

#include "platform.h"
#include <functional>

#include "error.hpp"
#include "lut.hpp"
#include "lwrb.h"

/// @brief UART Transmitter class using DMA or IT
class UartTransmitter {
public:
  enum Mode { DMA, IT };

  UartTransmitter(UART_HandleTypeDef *huart, enum Mode mode) : m_huart(huart), m_mode(mode) { m_ref.insert(m_huart, this); }

  int init();
  size_t write(const uint8_t data) { return write(&data, 1); }
  size_t write(const uint8_t *data, size_t size);

private:
  void handle_tx_cplt(UART_HandleTypeDef *huart);
  void handle_error(UART_HandleTypeDef *huart);

  inline static LookupTable<UART_HandleTypeDef *, UartTransmitter *, UART_MAX_INSTANCES> m_ref;

  HAL_StatusTypeDef (*m_transmit)(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
  UART_HandleTypeDef *m_huart;

  lwrb_t m_rb;
  uint8_t m_rb_buffer[UART_TX_BUFFER_SIZE] = {0};
  uint8_t m_tx_buffer[UART_TX_BUFFER_SIZE] = {0};

  enum Mode m_mode;
  bool m_tx_running = false;
  bool m_initialized = false;
};

/// @brief UART Receiver class using DMA or IT
class UartReceiver {
public:
  enum Mode { DMA, IT };

  UartReceiver(UART_HandleTypeDef *huart, enum Mode mode) : m_huart(huart), m_mode(mode) { m_ref.insert(m_huart, this); }

  /// @brief Initialize UART receiver
  Error init();

  /// @brief Read a single byte
  /// @return -1 if no data is available, otherwise the byte
  int read();

  /// @brief Read data
  /// @param data Pointer to data buffer
  /// @param size Size of data buffer
  /// @return Number of bytes read
  int read(uint8_t *data, size_t size);

  /// @brief Get number of bytes available
  /// @return Number of bytes available in buffer
  int available() { return lwrb_get_full(&m_rb); }

  /// @brief Flush buffer: clear all data in buffer
  void flush() {
    lwrb_reset(&m_rb);

    HAL_UART_AbortReceive(m_huart);
    m_old_pos = 0;

    init();
  }

  /// @brief Wait for data to be available
  /// @param timeout_ms Timeout in milliseconds
  /// @return 0 if data is available, -1 if timeout
  int wait(uint32_t timeout_ms) {
    m_task_handle = xTaskGetCurrentTaskHandle();
    return ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(timeout_ms)) == pdTRUE ? 0 : -1;
  }

  /// @brief Set callback function
  /// @param callback Callback function
  void set_callback(std::function<void(void *)> callback) { m_callback = callback; }

private:
  inline static LookupTable<UART_HandleTypeDef *, UartReceiver *, UART_MAX_INSTANCES> m_ref;

  UART_HandleTypeDef *m_huart;
  std::function<void(void *)> m_callback;

  TaskHandle_t m_task_handle = nullptr;

  enum Mode m_mode;
  bool m_initialized = false;

  uint16_t m_old_pos = 0;
  uint8_t m_rx_buffer[UART_RX_BUFFER_SIZE] = {0};

  lwrb_t m_rb;
  uint8_t m_rb_buffer[UART_RX_BUFFER_SIZE] = {0};

  void handle_rx_event_dma(UART_HandleTypeDef *, uint16_t);
  void handle_rx_event_it(UART_HandleTypeDef *, uint16_t);
  void handle_error(UART_HandleTypeDef *huart);
};
