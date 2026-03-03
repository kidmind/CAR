
#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"
#define ADC_DMA_BufferSize 1024
void ADC1_Init(void);
void Print_DataPoint(void);
void ADC_DMA_Start(void);  // 启动DMA传输
#endif
