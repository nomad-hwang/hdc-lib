#pragma once

#include "gpio.hpp"
#include <functional>

#define BIT_LOCATION(x) __builtin_ffs(x) - 1

class GpioInputExti;

class GpioInputExtiInstanceMixin {
protected:
  inline static GpioInputExti *instances[16];

  static inline void set_instance(uint16_t pin, GpioInputExti *instance) {
    uint8_t index = BIT_LOCATION(pin);
    if (instances[index] == nullptr) {
      instances[index] = instance;
      return;
    }
  }

public:
  static inline GpioInputExti *get_instance(uint16_t pin) { return instances[BIT_LOCATION(pin)]; }
};

class GpioInputExti : public Gpio, public GpioInputExtiInstanceMixin {
public:
  GpioInputExti(GPIO_TypeDef *port, uint16_t pin, bool active_level = true) : Gpio(port, pin, active_level) { set_instance(pin, this); }

  void set_cb(std::function<void(GpioInputExti *)> callback) { m_cb = callback; }

  //! This should be called from HAL_GPIO_EXTI_Callback only.
  void callback() {
    if (m_cb) {
      m_cb(this);
    }
  }

private:
  std::function<void(GpioInputExti *)> m_cb;
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  auto inst = GpioInputExti::get_instance(GPIO_Pin);
  if (inst == nullptr) {
    return; // TODO: error("GPIO_Pin not found")
  }

  inst->callback();
}

////////////////////////////////////////////////////////////////
