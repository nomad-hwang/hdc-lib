#pragma once

#include "platform.h"

class Gpio {
public:
  Gpio(GPIO_TypeDef *port, uint16_t pin, bool active_level = true) : m_port(port), m_pin(pin), m_active(active_level) {}

  void write(bool level) { HAL_GPIO_WritePin(m_port, m_pin, (GPIO_PinState)level); }
  bool read() { return HAL_GPIO_ReadPin(m_port, m_pin); }

  void set_active(bool active) { HAL_GPIO_WritePin(m_port, m_pin, (GPIO_PinState)(active ? m_active : !m_active)); }
  bool is_active() { return (bool)HAL_GPIO_ReadPin(m_port, m_pin) == m_active; }

protected:
  GPIO_TypeDef *m_port;
  uint16_t m_pin;
  bool m_active;
};
