#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f4xx_gpio.h"
#include <stm32f4xx.h>
#define Buzzer_ON GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define Buzzer_OFF GPIO_ResetBits(GPIOC, GPIO_Pin_6);
void Buzzer_Init(void);

#endif // !__BUZZER_H
