#include "DAC.h"
void DAC_GPIO_Config(void) {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructre;
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructre.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
}
void DAC_Time_Config(void) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructre;
  TIM_TimeBaseInitStructre.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructre.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructre.TIM_Period = TIM6_ARR;
  TIM_TimeBaseInitStructre.TIM_Prescaler = TIM6_PSC;
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructre);
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_Cmd(TIM6, ENABLE);
}
void DAC_TIM6_Trigger_Config(void) {
  DAC_InitTypeDef DAC_InitStructre;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  DAC_InitStructre.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
  DAC_InitStructre.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_InitStructre.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructre.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_Init(DAC_CHANNEL, &DAC_InitStructre);
  DAC_Cmd(DAC_CHANNEL, ENABLE);
}
void DAC_Config(void) {
  DAC_GPIO_Config();
  DAC_Time_Config();
  DAC_TIM6_Trigger_Config();
}
void DAC_SetValue(uint16_t dac_value) {
  // 设置DAC输出值，范围0-4095 (12位精度)
  DAC_SetChannel1Data(DAC_Align_12b_R, dac_value);
}
