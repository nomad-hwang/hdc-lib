#pragma once

#include "error.hpp"
#include "platform.h"
#include <algorithm>

class Pwm {
public:
  // timer should be configured before using this class(CubeMX)
  Pwm(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t frequency, bool active_level)
      : m_htim(htim), m_channel(channel), m_frequency(frequency), m_active_level(active_level) {}

  Error init() {
    assert(m_htim != nullptr);
    assert(HAL_RCC_GetHCLKFreq() / (m_htim->Instance->PSC + 1) / (m_htim->Instance->ARR + 1) == m_frequency);

    // check polarity
    TIM_OC_InitTypeDef conf = {0};
    conf.OCMode = TIM_OCMODE_PWM1;
    conf.OCPolarity = m_active_level ? TIM_OCPOLARITY_HIGH : TIM_OCPOLARITY_LOW;
    conf.OCIdleState = m_active_level ? TIM_OCIDLESTATE_RESET : TIM_OCIDLESTATE_SET;

    assert(HAL_TIM_PWM_ConfigChannel(m_htim, &conf, m_channel) == HAL_OK);

    set_duty(0.0f);
    return Error::OK;
  }

  Error start() { return HAL_TIM_PWM_Start(m_htim, m_channel) == HAL_OK ? Error::OK : Error::INIT_FAILED; }
  Error stop() { return HAL_TIM_PWM_Stop(m_htim, m_channel) == HAL_OK ? Error::OK : Error::INIT_FAILED; }

  void set_duty(float duty) {
    uint16_t compare = (uint16_t)(std::clamp(std::abs(duty), 0.0f, 1.0f) * (__HAL_TIM_GET_AUTORELOAD(m_htim) + 1));
    __HAL_TIM_SET_COMPARE(m_htim, m_channel, compare);
  }

  float get_duty() { return (float)__HAL_TIM_GET_COMPARE(m_htim, m_channel) / (float)(__HAL_TIM_GET_AUTORELOAD(m_htim) + 1); }

private:
  TIM_HandleTypeDef *m_htim;
  uint32_t m_channel;
  uint32_t m_frequency;
  bool m_active_level;
};

class InputCapture {
public:
  InputCapture(TIM_HandleTypeDef *htim, uint32_t channel) : m_htim(htim), m_channel(channel) {
    assert(m_htim != nullptr);
    assert(HAL_RCC_GetHCLKFreq() / (m_htim->Instance->PSC + 1) / (m_htim->Instance->ARR + 1) == 1000000);
  }

  Error init() {
    if (HAL_TIM_IC_Start_IT(m_htim, m_channel) != HAL_OK) {
      return Error::INIT_FAILED;
    }

    return Error::OK;
  }

  Error start() { return HAL_TIM_IC_Start_IT(m_htim, m_channel) == HAL_OK ? Error::OK : Error::INIT_FAILED; }
  Error stop() { return HAL_TIM_IC_Stop_IT(m_htim, m_channel) == HAL_OK ? Error::OK : Error::INIT_FAILED; }

  uint32_t get() { return m_capture; }

private:
  TIM_HandleTypeDef *m_htim;
  uint32_t m_channel;
  uint32_t m_capture;

  void _capture() { m_capture = HAL_TIM_ReadCapturedValue(m_htim, m_channel); }

  friend void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
};
