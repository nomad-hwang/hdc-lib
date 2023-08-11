#pragma once

#include "dma_circular.hpp"
#include "lut.hpp"
#include "lwrb.h"
#include "uart.hpp"

/// @brief UART Polling Transmitter implementation
class UartPollingTransmitter : public UartTransmitter<UartPollingTransmitter> {
public:
  UartPollingTransmitter(UART_HandleTypeDef *huart) : UartTransmitter(huart) {}

  int init() { return 0; }

  inline size_t write(const uint8_t *data, size_t size, uint32_t timeout = 1000) { //
    return HAL_UART_Transmit(m_huart, const_cast<uint8_t *>(data), size, timeout) == HAL_OK ? size : 0;
  }
};

/// @brief UART Polling Receiver implementation
class UartPollingReceiver : public UartReceiver<UartPollingReceiver> {
public:
  UartPollingReceiver(UART_HandleTypeDef *huart) : UartReceiver(huart) {}

  int init() { return 0; }

  inline size_t read(uint8_t *data, size_t size, uint32_t timeout = 1000) { //
    return HAL_UART_Receive(m_huart, data, size, timeout) == HAL_OK ? size : 0;
  }
};

/// @brief UART DMA Transmitter implementation
class UartDmaTransmitter : public UartTransmitter<UartDmaTransmitter> {
public:
  UartDmaTransmitter(UART_HandleTypeDef *huart) : UartTransmitter(huart), m_rb{0} { m_ref.insert(m_huart, this); }

  int init() {
    lwrb_init(&m_rb, m_rb_buffer, UART_RX_BUFFER_SIZE);

    HAL_UART_RegisterCallback(m_huart, HAL_UART_TX_COMPLETE_CB_ID, [](UART_HandleTypeDef *huart) { get(huart)->handle_tx_cplt(huart); });
    HAL_UART_RegisterCallback(m_huart, HAL_UART_ERROR_CB_ID, [](UART_HandleTypeDef *huart) { get(huart)->handle_error(huart); });

    m_initialized = true;

    return 0;
  }

  inline size_t write(const uint8_t *data, size_t size, uint32_t timeout = 1000) {
    UNUSED(timeout);
    assert(m_initialized);

    size_t written = 0;

    __disable_irq();

    if (m_dma_running) {
      written = lwrb_write(&m_rb, data, size);
    } else {
      m_dma_running = true;
      memcpy(m_tx_dma_buffer, data, size);
      written = HAL_UART_Transmit_DMA(m_huart, m_tx_dma_buffer, size) == HAL_OK ? size : 0;
    }

    __enable_irq();

    return written;
  }

private:
  inline static LookupTable<UART_HandleTypeDef *, UartDmaTransmitter *, UART_MAX_INSTANCES> m_ref;

  static UartDmaTransmitter *get(UART_HandleTypeDef *huart) {
    auto *obj = m_ref.get(huart);
    assert(obj);
    return obj;
  }

  lwrb_t m_rb;
  uint8_t m_rb_buffer[UART_TX_BUFFER_SIZE] = {0};

  uint8_t m_tx_dma_buffer[UART_TX_BUFFER_SIZE] = {0};

  bool m_dma_running = false;
  bool m_initialized = false;

  void handle_tx_cplt(UART_HandleTypeDef *huart) {
    if (lwrb_get_full(&m_rb) == 0) {
      m_dma_running = false;
    } else {
      if (HAL_UART_Transmit_DMA(huart, m_tx_dma_buffer, lwrb_read(&m_rb, m_tx_dma_buffer, UART_TX_BUFFER_SIZE)) != HAL_OK) {
        m_dma_running = false;
      }
    }
  }

  void handle_error(UART_HandleTypeDef *huart) {
    HAL_UART_MspDeInit(huart);
    HAL_UART_MspInit(huart);
  }
};

/// @brief UART DMA Receiver implementation
class UartDmaReceiver : public UartReceiver<UartDmaReceiver> {
public:
  UartDmaReceiver(UART_HandleTypeDef *huart) : UartReceiver(huart) { m_ref.insert(m_huart, this); }

  int init() {
    lwrb_init(&m_rb, m_rb_buffer, UART_RX_BUFFER_SIZE);

    HAL_UART_RegisterRxEventCallback(m_huart, [](UART_HandleTypeDef *huart, uint16_t Pos) { get(huart)->handle_rx_event(huart, Pos); });
    HAL_UART_RegisterCallback(m_huart, HAL_UART_ERROR_CB_ID, [](UART_HandleTypeDef *huart) { get(huart)->handle_error(huart); });

    HAL_UARTEx_ReceiveToIdle_DMA(m_huart, m_dma_buffer, UART_RX_BUFFER_SIZE);

    m_initialized = true;

    return 0;
  }

  inline size_t read(uint8_t *data, size_t size, uint32_t timeout = 1000) {
    UNUSED(timeout);
    assert(m_initialized);

    // TODO: another ring buffer needed? remove later maybe

    return lwrb_read(&m_rb, data, size);
  }

  inline size_t available() {
    assert(m_initialized);

    return lwrb_get_full(&m_rb);
  }

private:
  inline static LookupTable<UART_HandleTypeDef *, UartDmaReceiver *, UART_MAX_INSTANCES> m_ref;

  bool m_initialized = false;

  uint16_t m_old_pos = 0;
  uint8_t m_dma_buffer[UART_RX_BUFFER_SIZE] = {0};

  lwrb_t m_rb;
  uint8_t m_rb_buffer[UART_RX_BUFFER_SIZE] = {0};

  static UartDmaReceiver *get(UART_HandleTypeDef *huart) {
    auto *obj = m_ref.get(huart);
    assert(obj);
    return obj;
  }

  void handle_rx_event(UART_HandleTypeDef *huart, uint16_t _) {
    UNUSED(huart);

    uint16_t pos = UART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(m_huart->hdmarx);

    if (pos == m_old_pos) {
      return;
    }

    if (m_callback == nullptr) {
      goto END;
    }

    if (pos > m_old_pos) {
      lwrb_write(&m_rb, m_dma_buffer + m_old_pos, pos - m_old_pos);
    } else {
      lwrb_write(&m_rb, m_dma_buffer + m_old_pos, UART_RX_BUFFER_SIZE - m_old_pos);
      lwrb_write(&m_rb, m_dma_buffer, pos);
    }

  END:
    m_old_pos = pos;

    if (m_callback != nullptr) {
      m_callback(this);
    }
  }

  void handle_error(UART_HandleTypeDef *huart) {
    // TODO: handle various errors(maybe no reinit for some errors such as frame, noise, overrun)
    HAL_UART_MspDeInit(huart);
    HAL_UART_MspInit(huart);

    HAL_UARTEx_ReceiveToIdle_DMA(m_huart, m_dma_buffer, UART_RX_BUFFER_SIZE);
  }
};
