#include "delay.h"
#include "led.h"
#include "mpu6050.h"
#include "oled.h"
#include "task.h"
#include "usart.h"
#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>

/**
 * @brief 系统初始化函数
 */
void System_Init(void) {

  // 初始化延时函数

  delay_init();
  // 初始化串口
  Usart_Config();
  printf("Start MPU Init\r\n");

  MPU_Init();
  Task_Init();
}
static void USART1_Pro(void) {
  // printf("%6.2f,%6.2f,%6.2f\r\n", MPU_Attitude.pitch, MPU_Attitude.roll,
  //        MPU_Attitude.yaw);
  Get_RawData();
}
/**
 * @brief 主函数
 */

int main(void) {
  System_Init();
  printf("Stm32 is ready\r\n");
  u8 usart_id = add_task(USART1_Pro, 10);
  u8 MPU_id = add_task(MPU_Proc, 5);
  MPU_Proc();
  while (1) {
  }
}
