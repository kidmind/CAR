/**
  ******************************************************************************
  * @file    tim4_delay.h
  * @brief   TIM4微秒级延时函数头文件
  ******************************************************************************
  */

#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"
#define RCC_APB1Periph_TIMx RCC_APB1Periph_TIM4
#define TIMx TIM4
/* 函数声明 */
void delay_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif /* __TIM4_DELAY_H */