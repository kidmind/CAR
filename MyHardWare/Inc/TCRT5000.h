#ifndef __TCRT5000_H
#define __TCRT5000_H

#include "stm32f4xx.h"
#include <stdint.h>
#define TCRT_AO_CLK RCC_AHB1Periph_GPIOA
#define TCRT_AO_Port GPIOA
#define TCRT_AO_Pin GPIO_Pin_2
#define TCRTDO_CLK RCC_AHB1Periph_GPIOA
#define TCRT_DO_Port GPIOA
#define TCRT_DO_Pin GPIO_Pin_8
#define TCRT_ADC_CLK RCC_APB2Periph_ADC1
#define TCRT_ADC ADC1
typedef struct TCRT {
  uint16_t AO;
  uint8_t DO;
} TCRT;

void TCRT_Init(void);

uint16_t TCRT_GetValue(void);
#endif
