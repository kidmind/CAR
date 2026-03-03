
#include "TCRT5000.h"
#include <stdint.h>
volatile TCRT Tcrt;
void TCRT_GPIOInit(void) {
  RCC_AHB1PeriphClockCmd(TCRT_AO_CLK | TCRTDO_CLK, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructrue;
  GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructrue.GPIO_Pin = TCRT_AO_Pin;
  GPIO_InitStructrue.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(TCRT_AO_Port, &GPIO_InitStructrue);
  GPIO_InitStructrue.GPIO_Pin = TCRT_DO_Pin;
  GPIO_Init(TCRT_DO_Port, &GPIO_InitStructrue);
}
// 可选TIM3作为ADC的触发源
void TCRT_Time_Init(void) {

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  // 配置100kHz触发频率：
  // TIM3时钟 = 84MHz (APB1 42MHz × 2)
  // 目标频率 = 100kHz
  // 计算：84MHz / (PSC+1) / (ARR+1) = 100kHz
  // 设置：PSC = 83, ARR = 9
  // 实际频率：84MHz / 84 / 10 = 100kHz

  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟不分频
  TIM_TimeBaseInitStructure.TIM_CounterMode =
      TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter =
      0; // 重复计数器（基本定时器不使用）
  TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;    // 自动重装载值 ARR = 9
  TIM_TimeBaseInitStructure.TIM_Prescaler = 84 - 1; // 预分频器值 PSC = 83

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

  // 使能TIM3的主模式输出，触发ADC
  TIM_SelectOutputTrigger(TIM3,
                          TIM_TRGOSource_Update); // 使用更新事件作为触发输出

  TIM_Cmd(TIM3, ENABLE); // 使能TIM3
}
void TCRT_ADC_Init(void) {

  RCC_APB2PeriphClockCmd(TCRT_ADC_CLK, ENABLE);
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInit(&ADC_CommonInitStructure);
  ADC_InitTypeDef ADC_InitStructure;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;       // 单通道模式
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 非连续转换，由外部触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  // 使用Time的更新事件进行触发
  /*
  ADC_InitStructure.ADC_ExternalTrigConv =
      ADC_ExternalTrigConv_T3_TRGO; // TIM3触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge =
      ADC_ExternalTrigConvEdge_Rising;                   // 上升沿触发
  */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 12位分辨率
  ADC_InitStructure.ADC_NbrOfConversion = 1;             // 1个转换通道
  ADC_Init(TCRT_ADC, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_28Cycles);
  ADC_Cmd(TCRT_ADC, ENABLE);
}
void TCRT_Init(void) {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  TCRT_GPIOInit();
  TCRT_ADC_Init();
}
uint16_t TCRT_GetValue(void) {
  ADC_SoftwareStartConv(TCRT_ADC);
  while (ADC_GetFlagStatus(TCRT_ADC, ADC_FLAG_EOC) == RESET)
    ;
  return ADC_GetConversionValue(TCRT_ADC);
}
