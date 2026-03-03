// /**
//  ******************************************************************************
//  * @file    main.c
//  * @brief   主程序 - 23kHz PWM生成器
//  *          K1: 控制PWM开关
//  *          K2: 生成持续1ms的PWM脉冲
//  ******************************************************************************
//  */

// #include "../MyHardWare/Inc/key.h"
// #include "../MyHardWare/Inc/pwm.h"
// #include "../MyHardWare/Inc/timer.h"
// #include "Delay.h"
// #include "adc1.h"
// #include "led.h"
// #include "stm32f4xx.h"
// #include "stm32f4xx_gpio.h"
// #include "uart.h"
// #include <stdint.h>
// /* 全局变量 */
// volatile uint8_t pwm_enabled = 0;        // PWM使能标志
// volatile uint8_t pwm_pulse_active = 0;   // 1ms脉冲标志
// volatile uint16_t pulse_countdown = 0;   // 脉冲计数器
// volatile uint8_t uart_transmit_flag = 0; // UART发送标志

// extern uint8_t AWD_Flag;
// extern uint8_t DMA2_Flag;
// /* 私有函数声明 */
// static void SystemClock_Config(void);
// void ADC_Read_Buffer_Example(void); // ADC缓冲区读取示例函数

// /**
//  * @brief  主函数
//  * @param  None
//  * @retval None
//  */
// int main1213(void) {
//   /* 系统时钟配置 */
//   SystemClock_Config();

//   extern uint8_t ADC_Flag;
//   /* 初始化硬件模块 */
//   PWM_Init();       // PWM初始化
//   KEY_Init();       // 按键初始化
//   Timer_1ms_Init(); // 定时器初始化
//   // UART1_Init(); // UART1初始化
//   // led_init();
//   // GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//   // Delay_Init();
//   // UART1_SendString("The stm32 is ready\n");
//   // delay_us(1);
//   // ADC1_Init();
//   // uint16_t num;

//   while (1) {
//     // if (DMA2_Flag == 1 && AWD_Flag == 3) {
//     //   DMA2_Flag = 0;
//     //   AWD_Flag = 0;
//     //   Print_DataPoint();
//     //   UART1_SendString("This is ");
//     //   UART1_SendData(&num, 1);
//     //   UART1_SendString("\n");
//     //   num++;
//     // }
//     /* 主循环中处理1ms脉冲定时停止 */
//     if (pwm_pulse_active && pulse_countdown == 0)
//     {
//         PWM_Stop();
//         pwm_pulse_active = 0;
//         Timer_1ms_Stop();  // 停止定时器
//     }
//   }
// }

// /**
//  * @brief  系统时钟配置函数
//  * @param  None
//  * @retval None
//  */
// static void SystemClock_Config(void) {
//   /* 使能HSE */
//   RCC_HSEConfig(RCC_HSE_ON);

//   /* 等待HSE就绪 */
//   if (RCC_WaitForHSEStartUp() == SUCCESS) {
//     /* 配置Flash预取指、指令缓存、数据缓存和等待周期 */
//     FLASH_SetLatency(FLASH_Latency_5);
//     FLASH_PrefetchBufferCmd(ENABLE);
//     FLASH_InstructionCacheCmd(ENABLE);
//     FLASH_DataCacheCmd(ENABLE);

//     /* 配置AHB、APB1、APB2分频器 */
//     RCC_HCLKConfig(RCC_SYSCLK_Div1);
//     RCC_PCLK1Config(RCC_HCLK_Div4);
//     RCC_PCLK2Config(RCC_HCLK_Div2);

//     /* 配置PLL */
//     RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);

//     /* 使能PLL */
//     RCC_PLLCmd(ENABLE);

//     /* 等待PLL就绪 */
//     while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//       ;

//     /* 选择PLL作为系统时钟源 */
//     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

//     /* 等待PLL成为系统时钟源 */
//     while (RCC_GetSYSCLKSource() != 0x08)
//       ;
//   } else {
//     /* HSE启动失败，使用HSI */
//     RCC_HSICmd(ENABLE);
//     while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
//       ;

//     FLASH_SetLatency(FLASH_Latency_5);
//     FLASH_PrefetchBufferCmd(ENABLE);

//     RCC_HCLKConfig(RCC_SYSCLK_Div1);
//     RCC_PCLK1Config(RCC_HCLK_Div4);
//     RCC_PCLK2Config(RCC_HCLK_Div2);

//     RCC_PLLConfig(RCC_PLLSource_HSI, 16, 336, 2, 7);
//     RCC_PLLCmd(ENABLE);
//     while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//       ;

//     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//     while (RCC_GetSYSCLKSource() != 0x08)
//       ;
//   }
// }

// /**
//  * @brief  K1按键按下处理函数
//  * @param  None
//  * @retval None
//  */
// void KEY1_Pressed_Handler(void) {
//   /* 切换PWM使能状态 */
//   if (pwm_enabled) {
//     PWM_Stop();
//     pwm_enabled = 0;
//     pwm_pulse_active = 0; // 取消脉冲模式
//   } else {
//     PWM_Start();
//     pwm_enabled = 1;
//     pwm_pulse_active = 0; // 取消脉冲模式
//   }
// }

// /**
//  * @brief  K2按键按下处理函数
//  * @param  None
//  * @retval None
//  */
// void KEY2_Pressed_Handler(void) {
//   /* 启动1msPWM脉冲 */
//   if (!pwm_pulse_active) {
//     PWM_Start();
//     pwm_pulse_active = 1;
//     pulse_countdown = 1; // 1ms后停止

//     /* 启动1ms定时器 */
//     Timer_1ms_Start();
//   }
// }

// /**
//  * @brief  定时器回调函数 - 1ms调用一次
//  * @param  None
//  * @retval None
//  */
// void Timer_1ms_Callback(void) {
//   /* 更新脉冲计数器 */
//   if (pwm_pulse_active && pulse_countdown > 0) {
//     pulse_countdown--;
//   }
// }

// /**
//  * @brief  ADC转换完成回调函数
//  * @param  None
//  * @retval None
//  */
// void ADC_ConversionComplete_Callback(void) {

//   /* 通过UART1发送ADC数据 */
//   if (!uart_transmit_flag) {
//     uart_transmit_flag = 1;
//     UART1_Send_ADC_Data();
//     uart_transmit_flag = 0;
//   }
// }

// /**
//  * @brief  ADC缓冲区半满回调函数
//  * @param  None
//  * @retval None
//  */
// void ADC_Buffer_HalfComplete_Callback(void) {
//   /* 半缓冲区满（1150个采样点） */
//   /* 可以在这里进行实时数据处理 */
//   /* 例如：实时显示、基本统计分析等 */
// }

// /**
//  * @brief  ADC模拟看门狗中断回调函数
//  * @param  value: 触发看门狗时的ADC值
//  * @retval None
//  *
//  * @details 当ADC采集的数据达到或超过3000时触发此函数
//  *          用户可以在这里添加阈值告警的处理逻辑
//  */
// void ADC_AnalogWatchdog_Callback(uint16_t value) {
//   /* 模拟看门狗触发 - ADC值达到或超过3000 */
// }
