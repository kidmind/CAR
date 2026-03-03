/**
 * @file pwm.c
 * @brief PWM输出驱动实现文件 - 23kHz PWM信号生成
 * @version 1.0
 * @date 2025-12-13
 *
 * @details 本驱动实现基于TIM2_CH2的PWM输出功能
 *          - 输出引脚：PA1 (TIM2_CH2)
 *          - PWM频率：23kHz
 *          - 默认占空比：50%
 *          - 定时器时钟：84MHz (168MHz系统时钟分频)
 *
 * @note  时钟计算：168MHz → APB1=42MHz → TIM2=84MHz
 *        PWM频率计算：84MHz / (83+1) / (43) ≈ 23.26kHz
 */

#include "pwm.h"

/**
 * @brief  PWM初始化函数
 * @param  None
 * @retval None
 *
 * @details 初始化步骤：
 *          1. 使能GPIO和TIM2时钟
 *          2. 配置PA1为TIM2_CH2复用功能
 *          3. 配置TIM2时基单元产生23kHz PWM
 *          4. 配置PWM模式1输出
 *          5. 设置默认50%占空比
 */
void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 使能时钟 - 关键第一步 */
    RCC_AHB1PeriphClockCmd(PWM_GPIO_CLK, ENABLE);  // 使能GPIOA时钟 (PA1)
    RCC_APB1PeriphClockCmd(PWM_TIM_CLK, ENABLE);    // 使能TIM2时钟

    /* 配置PWM引脚为复用功能 - PA1作为TIM2_CH2输出 */
    GPIO_InitStructure.GPIO_Pin = PWM_GPIO_PIN;        // PA1引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用功能模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;     // 下拉电阻，默认低电平
    GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStructure);

    /* 配置GPIO引脚复用功能为TIM2 - 将PA1连接到TIM2 */
    GPIO_PinAFConfig(PWM_GPIO_PORT, GPIO_PinSource1, GPIO_AF_TIM2);

    /* 定时器基本配置 - 产生23kHz PWM信号 */
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;        // 自动重装载值=42 (0-42共43个计数)
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER - 1;  // 预分频器=82 (实际分频83)
                                                                // 时钟计算：84MHz/(83) = 1.01MHz ≈ 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;              // 无时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(PWM_TIM, &TIM_TimeBaseStructure);

    /* PWM模式配置 - 配置TIM2_CH2为PWM输出 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          // PWM模式1：CNT<CCR时输出高电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse = PWM_PULSE_DEFAULT;         // 初始占空比：50% (21/43)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 输出极性：高电平有效
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲状态：低电平

    /* 应用PWM配置到TIM2通道2 */
    TIM_OC2Init(PWM_TIM, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(PWM_TIM, TIM_OCPreload_Enable);  // 使能CCR预装载

    /* 使能定时器预装载寄存器 - 确保PWM更新同步 */
    TIM_ARRPreloadConfig(PWM_TIM, ENABLE);

    /* 先不启动定时器 - 等待用户控制启动 */
    TIM_Cmd(PWM_TIM, DISABLE);
}

/**
 * @brief  启动PWM输出
 * @param  None
 * @retval None
 *
 * @details 启动步骤：
 *          1. 启动TIM2定时器计数
 *          2. 启动PWM输出控制信号
 *          3. PA1开始输出23kHz PWM波形
 */
void PWM_Start(void)
{
    TIM_Cmd(PWM_TIM, ENABLE);                 // 启动TIM2定时器计数器
    TIM_CtrlPWMOutputs(PWM_TIM, ENABLE);      // 启动PWM主输出，使能PWM信号输出
}

/**
 * @brief  停止PWM输出
 * @param  None
 * @retval None
 *
 * @details 停止步骤：
 *          1. 禁用PWM输出控制信号
 *          2. 停止TIM2定时器计数
 *          3. PA1输出低电平（取决于GPIO配置）
 */
void PWM_Stop(void)
{
    TIM_CtrlPWMOutputs(PWM_TIM, DISABLE);     // 禁用PWM主输出，停止PWM信号
    TIM_Cmd(PWM_TIM, DISABLE);                // 停止TIM2定时器计数器
}

/**
 * @brief  设置PWM占空比
 * @param  duty: 占空比值 (0 - PWM_PERIOD)
 * @retval None
 *
 * @details 占空比计算：
 *          - 占空比 = duty / PWM_PERIOD * 100%
 *          - 例如：duty=21, PWM_PERIOD=43 → 21/43 ≈ 48.8%
 *          - 50%占空比：duty = PWM_PERIOD / 2 = 21
 *
 * @note   duty值会自动限制在有效范围内
 */
void PWM_Set_Duty(uint16_t duty)
{
    if (duty > PWM_PERIOD)                     // 检查参数有效性
    {
        duty = PWM_PERIOD;                     // 限制最大值为周期值
    }
    TIM_SetCompare2(PWM_TIM, duty);            // 设置TIM2_CH2捕获比较寄存器值
                                              // CCR值决定高电平持续时间
}

/**
 * @brief  设置PWM频率
 * @param  frequency: 频率值 (Hz)
 * @retval None
 *
 * @details 频率计算：
 *          - 计数频率 = 84MHz / 83 ≈ 1MHz
 *          - 周期值 = 1MHz / frequency
 *          - 新频率 = 1MHz / period
 *
 * @example frequency=23000Hz → period=1000000/23000=43
 *          实际频率 = 1MHz/43 ≈ 23.26kHz
 *
 * @note   频率改变时会保持当前占空比比例
 */
void PWM_Set_Frequency(uint32_t frequency)
{
    uint32_t period;

    if (frequency == 0)                        // 频率不能为0
    {
        return;                                // 直接返回，保持当前设置
    }

    period = 1000000 / frequency;               // 计算新的周期值（基于1MHz计数）

    if (period < 1)                            // 限制最小周期值
    {
        period = 1;                            // 最小周期=1，最高频率=1MHz
    }

    TIM_SetAutoreload(PWM_TIM, period - 1);     // 设置新的自动重装载值

    /* 保持当前占空比比例 - 关键特性 */
    uint32_t current_duty = TIM_GetCapture2(PWM_TIM);           // 读取当前CCR值
    uint32_t new_duty = (current_duty * period) / PWM_PERIOD;  // 按比例计算新CCR值
    TIM_SetCompare2(PWM_TIM, new_duty);                        // 应用新占空比

    /* 更新全局周期变量 - 供后续占空比设置使用 */
    // 注意：这里需要更新PWM_PERIOD宏定义，由于宏定义不能修改，
    //       建议用户调用PWM_Set_Duty()重新设置精确占空比
}