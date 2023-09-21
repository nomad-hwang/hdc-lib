#pragma once

#include "platform.h"
#include "tools.hpp"

class Gpio {
public:
  Gpio(GPIO_TypeDef *port, uint16_t pin, bool active_level = true) : m_port(port), m_pin(pin), m_active(active_level) {}

  void write(bool level) { HAL_GPIO_WritePin(m_port, m_pin, (GPIO_PinState)level); }
  bool read() { return HAL_GPIO_ReadPin(m_port, m_pin); }

  void activate(bool active) { HAL_GPIO_WritePin(m_port, m_pin, (GPIO_PinState)(active ? m_active : !m_active)); }
  bool is_active() { return (bool)HAL_GPIO_ReadPin(m_port, m_pin) == m_active; }

protected:
  GPIO_TypeDef *m_port;
  uint16_t m_pin;
  bool m_active;
};

// class Debounce {
// public:
//   // Debounce(Gpio *gpio, uint32_t interval_ms = 10) : m_gpio(gpio), m_interval_ms(interval_ms) {}
//   Debounce(Gpio *gpio, uint32_t interval_ms = 10, uint32_t debounce_cnt = 3) : m_gpio(gpio), m_interval_ms(interval_ms) {
//     m_timer.xTimerPeriodInTicks = pdMS_TO_TICKS(interval_ms);
//     m_timer.xTimerPeriodInTicks = pdMS_TO_TICKS(interval_ms);
//   }

//   void init() {
//     if (m_timer_handle != nullptr) {
//       return;
//     }

//     m_timer_handle = xTimerCreateStatic(
//         "debounce", pdMS_TO_TICKS(m_interval_ms), pdFALSE, this, [](TimerHandle_t timer) { ((Debounce *)pvTimerGetTimerID(timer))->_loop(); }, &m_timer);
//   }

//   bool read() { return m_active; }

// private:
//   Gpio *m_gpio;
//   uint32_t m_interval_ms;

//   bool m_active = false;
//   bool m_active_last = false;

//   StaticTimer_t m_timer;
//   TimerHandle_t m_timer_handle;

//   void _loop() {
//     bool level = m_gpio->read();
//     if (level == m_active_last) {
//       return;
//     }

//     m_active_last = level;
//     m_active = level;
//   }
// };

// static void *exti_instances[16] = {nullptr};

class Exti : public Gpio {
public:
  enum class Edge {
    RISING = 0x01,
    FALLING = 0x02,
    BOTH = 0x03,
  };

  inline static void *m_instances[16] = {nullptr};
  inline static void *m_instances_arg[16] = {nullptr};

  static void call_callback(uint16_t pin) {
    uint8_t bitIndex = get_bit_index(pin);
    if (m_instances[bitIndex] == nullptr) {
      return;
    }
    auto callback = ((Exti *)Exti::m_instances[bitIndex])->m_callback;
    if (callback) {
      callback(Exti::m_instances_arg[bitIndex]);
    }
  }

  Exti(GPIO_TypeDef *port, uint16_t pin, Edge edge) : Gpio(port, pin), m_edge(edge) {
    uint8_t bitIndex = get_bit_index(pin);

    assert(Exti::m_instances[bitIndex] == nullptr); // EXTI Pin must be unique across all GPIO ports
    m_instances[bitIndex] = this;

    // TODO: Fix this
    // if (edge == Edge::RISING) {
    //   assert(port->MODER & (uint32_t)GPIO_MODE_IT_RISING << (pin * 2));
    // } else if (edge == Edge::FALLING) {
    //   assert(port->MODER & (uint32_t)GPIO_MODE_IT_FALLING << (pin * 2));
    // } else {
    //   assert(port->MODER & (uint32_t)GPIO_MODE_IT_RISING_FALLING << (pin * 2));
    // }
  }

  Edge get_edge() { return m_edge; }

  void set_callback(void (*callback)(void *arg), void *arg = nullptr) {
    m_callback = callback;
    m_instances_arg[get_bit_index(m_pin)] = arg;
  }

private:
  void (*m_callback)(void *arg);

  Edge m_edge;
};
