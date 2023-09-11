#pragma once

#include "platform.h"

#define ADC_MAX_CHANNEL (16)

class AdcDma {
public:
  AdcDma(ADC_HandleTypeDef *hadc, uint32_t channel_count) : m_hadc(hadc), m_channel_count(channel_count) {}

  int init() { return 0; }
  int start() { return HAL_ADC_Start_DMA(m_hadc, (uint32_t *)m_adc_value, m_channel_count) == HAL_OK ? 0 : -1; }
  int stop() { return HAL_ADC_Stop_DMA(m_hadc) == HAL_OK ? 0 : -1; }

  uint16_t get(uint32_t channel) { return m_adc_value[channel]; }
  float get_voltage(uint32_t channel, float vref = 3.3f) { return m_adc_value[channel] * vref / 4096.0f; }

  ADC_HandleTypeDef *m_hadc;

private:
  uint32_t m_channel_count;
  __IO uint16_t m_adc_value[ADC_MAX_CHANNEL] = {0};
};
