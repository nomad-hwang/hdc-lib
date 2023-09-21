#include "gpio.hpp"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { Exti::call_callback(GPIO_Pin); }