#pragma once

#include "platform.h"

class Tick {
public:
  inline static uint32_t get() {
    return uwTick; // HAL_GetTick();
  }

  static uint32_t elapsed(uint32_t start) {
    return get() >= start ? get() - start : get() + (0xFFFFFFFF - start); // Overflow handling
  }
};
