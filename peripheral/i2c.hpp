#pragma once

#include "error.hpp"
#include "platform.h"

/// @brief I2C class
class I2C {
public:
  I2C(I2C_HandleTypeDef *hi2c) : m_hi2c(hi2c) {}

  /// @brief Initialize I2C
  Error init();

  /// @brief Read data from I2C
  /// @param addr I2C address
  /// @param data Pointer to data buffer
  /// @param size Size of data buffer
  /// @return Error code
  Error read(uint16_t addr, uint8_t *data, size_t size, uint32_t timeout_ms = 100) {
    return HAL_I2C_Master_Receive(m_hi2c, addr, data, size, timeout_ms) == HAL_OK ? Error::OK : Error::TIMEOUT;
  }

  /// @brief Write data to I2C
  /// @param addr I2C address
  /// @param data Pointer to data buffer
  /// @param size Size of data buffer
  /// @return Error code
  Error write(uint16_t addr, uint8_t *data, size_t size, uint32_t timeout_ms = 100) {
    return HAL_I2C_Master_Transmit(m_hi2c, addr, data, size, timeout_ms) == HAL_OK ? Error::OK : Error::TIMEOUT;
  }

private:
  I2C_HandleTypeDef *m_hi2c;
};

class I2CArduinoCompat {
public:
  I2CArduinoCompat(I2C_HandleTypeDef *hi2c, uint32_t timeout_ms = 100) : m_i2c(hi2c), m_timeout_ms(timeout_ms) {}

  void begin() {}

  // TX

  void beginTransmission(uint8_t address) {
    m_address = address << 1;
    m_tx_pos = 0;
  }

  int endTransmission() { //
    return m_i2c.write(m_address, m_tx_buffer, m_tx_pos, m_timeout_ms) == Error::OK ? 0 : 1;
  }

  void write(uint8_t data) {
    assert(m_tx_pos < 256);
    m_tx_buffer[m_tx_pos++] = data;
  }

  // RX

  uint8_t requestFrom(uint8_t address, uint8_t size) {
    m_address = address << 1;
    m_rx_pos = 0;
    m_rx_size = size;

    return m_i2c.read(m_address, m_rx_buffer, m_rx_size, m_timeout_ms) == Error::OK ? m_rx_size : 0;
  }

  uint8_t read() { return m_rx_buffer[m_rx_pos++]; }
  uint8_t available() { return m_rx_pos < m_rx_size; }

private:
  I2C m_i2c;

  uint8_t m_address = 0;
  uint8_t m_tx_buffer[256] = {0};
  uint8_t m_rx_buffer[256] = {0};
  uint8_t m_tx_pos = 0;
  uint8_t m_rx_pos = 0;
  uint8_t m_rx_size = 0;

  uint32_t m_timeout_ms = 100;
};
