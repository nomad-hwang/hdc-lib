
#pragma once

#include "platform.h"
#include <functional>

/// @brief UART Transmitter base class
/// @tparam T derived class
template <typename T> class UartTransmitter {
public:
  UartTransmitter(UART_HandleTypeDef *huart) : m_huart(huart) { assert(m_huart != nullptr); }

  /// @brief Initialize UART
  /// @return 0 if success
  /// @note This is must be called before using UART since the value assigned at startup is cleared by HAL_MspInit()
  int init() { return static_cast<T *>(this)->init(); }

  /// @brief Write data to UART
  /// @param data: data to be written
  /// @param size: data size
  /// @param timeout: timeout in ms
  /// @return size of data written
  inline size_t write(const uint8_t *data, size_t size, uint32_t timeout = 1000) {
    assert(timeout > 0);
    assert(data != nullptr);
    assert(size > 0 && size <= UART_TX_BUFFER_SIZE);

    return static_cast<T *>(this)->write(data, size, timeout);
  }

protected:
  UART_HandleTypeDef *m_huart;
};

/// @brief UART Receiver base class
/// @tparam T derived class
template <typename T> class UartReceiver {
public:
  UartReceiver(UART_HandleTypeDef *huart) : m_huart(huart) { assert(m_huart != nullptr); }

  /// @brief Initialize UART
  /// @return 0 if success
  /// @note This is must be called before using UART since the value assigned at startup is cleared by HAL_MspInit()
  int init() { return static_cast<T *>(this)->init(); }

  /// @brief Read data from UART
  /// @param data: data buffer
  /// @param size: data size
  /// @param timeout: timeout in ms
  /// @return size of data read
  inline size_t read(uint8_t *data, size_t size, uint32_t timeout = 1000) {
    assert(timeout > 0);
    assert(data != nullptr);
    assert(size > 0 && size <= UART_RX_BUFFER_SIZE);

    return static_cast<T *>(this)->read(data, size, timeout);
  }

  /// @brief Length of data available in RX buffer
  /// @return length of data available
  inline size_t available() { return static_cast<T *>(this)->available(); }

  /// @brief Set callback function to be called when data is received(idle line detected)
  /// @param callback: callback function
  void set_callback(std::function<void(void *)> callback) { m_callback = callback; }

protected:
  UART_HandleTypeDef *m_huart;
  std::function<void(void *)> m_callback;
};
