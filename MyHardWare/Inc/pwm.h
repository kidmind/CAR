/**
 * @file pwm.h
 * @brief PWM输出驱动头文件 - 23kHz PWM信号生成
 * @version 1.0
 * @date 2025-12-13
 */

#ifndef __PWM_H
#define __PWM_H

#include "stm32f4xx.h"

/* PWM配置宏定义 - 方便用户修改 */
// PWM输出引脚配置
#define PWM_GPIO_PORT              GPIOA
#define PWM_GPIO_PIN               GPIO_Pin_1
#define PWM_GPIO_CLK               RCC_AHB1Periph_GPIOA

// 定时器配置 - 使用TIM2
#define PWM_TIM                    TIM2
#define PWM_TIM_CLK                RCC_APB1Periph_TIM2

// PWM通道配置 - 使用通道2
#define PWM_TIM_CHANNEL            TIM_Channel_2

// PWM频率和占空比配置
// 系统时钟168MHz, APB1=42MHz, TIM2时钟=84MHz(APB1x2)
#define PWM_FREQUENCY_HZ           23000    // 23kHz
#define PWM_PRESCALER              83    // 预分频器，实际分频84 (84MHz/84 = 1MHz)
#define PWM_PERIOD                 43       // 1MHz/43 = 23.26kHz (接近23kHz)
#define PWM_PULSE_DEFAULT          (PWM_PERIOD / 2)  // 默认50%占空比

/* 函数声明 */
void PWM_Init(void);
void PWM_Start(void);
void PWM_Stop(void);
void PWM_Set_Duty(uint16_t duty);
void PWM_Set_Frequency(uint32_t frequency);

#endif /* __PWM_H */