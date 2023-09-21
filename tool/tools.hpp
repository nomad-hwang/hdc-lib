#pragma once

#include "platform.h"

static uint8_t get_bit_index(uint16_t pin) {
  uint8_t bitIndex = 0;

  for (uint8_t i = 0; i < 16; i++) {
    if (pin & (1 << i)) {
      bitIndex = i;
      break;
    }
  }
  return bitIndex;
}