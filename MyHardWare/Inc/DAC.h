#ifndef __DAC_H
#define __DAC_H
#include <stm32f4xx.h>
#define TIM6_PSC 8399;
#define TIM6_ARR 9
#define DAC_CHANNEL DAC_Channel_1
void DAC_Config(void);

void DAC_SetValue(uint16_t dac_value);
#endif // !__DAC_H
