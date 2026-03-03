/**
 * @file motor.c
 * @brief 四路电机PWM驱动实现文件 - 智能车专用
 * @version 1.0
 * @date 2025-02-25
 */

#include "motor.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

/* ==================== 私有变量 ==================== */
static Motor_Class_t Motor_Class = {0};

/* ==================== 私有函数声明 ==================== */
static void Motor_GPIO_Init(Motor_Id_e motor);
static void Motor_TIM_Init(TIM_TypeDef* tim, uint32_t tim_clk);
static void Motor_TIM_Channel_Init(TIM_TypeDef* tim, uint16_t channel);

/* ==================== 私有函数实现 ==================== */

/**
 * @brief 初始化单个电机的GPIO引脚
 * @param motor 电机ID
 */
static void Motor_GPIO_Init(Motor_Id_e motor)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    switch (motor) {
        case MOTOR_FR:
            // 使能时钟
            RCC_AHB1PeriphClockCmd(MOTOR_FR_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_FR_DIR_CLK, ENABLE);

            // 配置PWM引脚 (PE9 - TIM1_CH1)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_FR_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_FR_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_FR_PWM_PORT, GPIO_PinSource9, MOTOR_FR_PWM_AF);

            // 配置方向控制引脚 (PE10, PE11)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_FR_DIR_PORT, &GPIO_InitStructure);

            // 默认停止状态
            GPIO_ResetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN);
            break;

        case MOTOR_FL:
            // 使能时钟
            RCC_AHB1PeriphClockCmd(MOTOR_FL_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_FL_DIR_CLK, ENABLE);

            // 配置PWM引脚 (PE11 - TIM1_CH2)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_FL_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_FL_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_FL_PWM_PORT, GPIO_PinSource11, MOTOR_FL_PWM_AF);

            // 配置方向控制引脚 (PE12, PE13)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_FL_DIR_PORT, &GPIO_InitStructure);

            // 默认停止状态
            GPIO_ResetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN);
            break;

        case MOTOR_BR:
            // 使能时钟
            RCC_AHB1PeriphClockCmd(MOTOR_BR_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_BR_DIR_CLK, ENABLE);

            // 配置PWM引脚 (PC6 - TIM8_CH1)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_BR_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_BR_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_BR_PWM_PORT, GPIO_PinSource6, MOTOR_BR_PWM_AF);

            // 配置方向控制引脚 (PC7, PC8)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_BR_DIR1_PIN | MOTOR_BR_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_BR_DIR_PORT, &GPIO_InitStructure);

            // 默认停止状态
            GPIO_ResetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR1_PIN | MOTOR_BR_DIR2_PIN);
            break;

        case MOTOR_BL:
            // 使能时钟
            RCC_AHB1PeriphClockCmd(MOTOR_BL_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_BL_DIR_CLK, ENABLE);

            // 配置PWM引脚 (PC7 - TIM8_CH2)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_BL_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_BL_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_BL_PWM_PORT, GPIO_PinSource7, MOTOR_BL_PWM_AF);

            // 配置方向控制引脚 (PC9, PC10)
            GPIO_InitStructure.GPIO_Pin   = MOTOR_BL_DIR1_PIN | MOTOR_BL_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_BL_DIR_PORT, &GPIO_InitStructure);

            // 默认停止状态
            GPIO_ResetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR1_PIN | MOTOR_BL_DIR2_PIN);
            break;

        default:
            break;
    }
}

/**
 * @brief 初始化定时器时基单元
 * @param tim 定时器指针
 * @param tim_clk 定时器时钟RCC
 */
static void Motor_TIM_Init(TIM_TypeDef* tim, uint32_t tim_clk)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 使能定时器时钟
    RCC_APB2PeriphClockCmd(tim_clk, ENABLE);

    // 定时器时基配置 - 20kHz PWM
    // 168MHz / (0+1) / (8399+1) = 20kHz
    TIM_TimeBaseStructure.TIM_Period         = MOTOR_PWM_PERIOD;      // ARR = 8399
    TIM_TimeBaseStructure.TIM_Prescaler      = MOTOR_PWM_PRESCALER;   // PSC = 0
    TIM_TimeBaseStructure.TIM_ClockDivision  = 0;
    TIM_TimeBaseStructure.TIM_CounterMode    = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tim, &TIM_TimeBaseStructure);

    // 使能预装载寄存器
    TIM_ARRPreloadConfig(tim, ENABLE);
}

/**
 * @brief 初始化定时器PWM通道
 * @param tim 定时器指针
 * @param channel PWM通道
 */
static void Motor_TIM_Channel_Init(TIM_TypeDef* tim, uint16_t channel)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // PWM模式配置
    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  // 使能互补输出
    TIM_OCInitStructure.TIM_Pulse        = 0;                         // 初始占空比0
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

    switch (channel) {
        case TIM_Channel_1:
            TIM_OC1Init(tim, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        case TIM_Channel_2:
            TIM_OC2Init(tim, &TIM_OCInitStructure);
            TIM_OC2PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        case TIM_Channel_3:
            TIM_OC3Init(tim, &TIM_OCInitStructure);
            TIM_OC3PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        case TIM_Channel_4:
            TIM_OC4Init(tim, &TIM_OCInitStructure);
            TIM_OC4PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        default:
            break;
    }
}

/* ==================== 公有函数实现 ==================== */

/**
 * @brief 电机驱动初始化
 */
void Motor_Driver_Init(void)
{
    // 初始化TIM1 (前右、前左电机)
    Motor_TIM_Init(MOTOR_FR_PWM_TIM, MOTOR_FR_PWM_TIM_CLK);
    Motor_TIM_Channel_Init(MOTOR_FR_PWM_TIM, MOTOR_FR_PWM_CHANNEL);
    Motor_TIM_Channel_Init(MOTOR_FR_PWM_TIM, MOTOR_FL_PWM_CHANNEL);

    // 初始化TIM8 (后右、后左电机)
    Motor_TIM_Init(MOTOR_BR_PWM_TIM, MOTOR_BR_PWM_TIM_CLK);
    Motor_TIM_Channel_Init(MOTOR_BR_PWM_TIM, MOTOR_BR_PWM_CHANNEL);
    Motor_TIM_Channel_Init(MOTOR_BR_PWM_TIM, MOTOR_BL_PWM_CHANNEL);

    // 初始化所有电机的GPIO
    Motor_GPIO_Init(MOTOR_FR);
    Motor_GPIO_Init(MOTOR_FL);
    Motor_GPIO_Init(MOTOR_BR);
    Motor_GPIO_Init(MOTOR_BL);

    // 使能TIM1/TIM8主输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM8, ENABLE);

    // 启动定时器
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM8, ENABLE);

    // 初始化类结构体
    Motor_Class.Init = Motor_Driver_Init;
    Motor_Class.DeInit = Motor_Driver_DeInit;
    Motor_Class.SetSpeed = Motor_SetSpeed;
    Motor_Class.SetDirection = Motor_SetDirection;
    Motor_Class.SetDuty = Motor_SetDuty;
    Motor_Class.Stop = Motor_Stop;
    Motor_Class.Brake = Motor_Brake;
}

/**
 * @brief 电机驱动去初始化
 */
void Motor_Driver_DeInit(void)
{
    // 停止所有电机
    Motor_StopAll();

    // 停止定时器
    TIM_Cmd(TIM1, DISABLE);
    TIM_Cmd(TIM8, DISABLE);

    // 禁用主输出
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    TIM_CtrlPWMOutputs(TIM8, DISABLE);
}

/**
 * @brief 设置电机速度（带符号）
 * @param motor 电机ID
 * @param speed 速度值 (-MOTOR_PWM_MAX_DUTY ~ +MOTOR_PWM_MAX_DUTY)
 */
void Motor_SetSpeed(Motor_Id_e motor, int16_t speed)
{
    // 限制速度范围
    if (speed > MOTOR_PWM_MAX_DUTY) {
        speed = MOTOR_PWM_MAX_DUTY;
    } else if (speed < -MOTOR_PWM_MAX_DUTY) {
        speed = -MOTOR_PWM_MAX_DUTY;
    }

    // 根据速度正负设置方向和占空比
    if (speed > 0) {
        Motor_SetDirection(motor, MOTOR_DIR_FORWARD);
        Motor_SetDuty(motor, (uint16_t)speed);
    } else if (speed < 0) {
        Motor_SetDirection(motor, MOTOR_DIR_BACKWARD);
        Motor_SetDuty(motor, (uint16_t)(-speed));
    } else {
        Motor_Stop(motor);
    }
}

/**
 * @brief 设置电机方向
 * @param motor 电机ID
 * @param dir 方向
 */
void Motor_SetDirection(Motor_Id_e motor, Motor_Direction_e dir)
{
    switch (motor) {
        case MOTOR_FR:
            switch (dir) {
                case MOTOR_DIR_FORWARD:
                    GPIO_SetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN);
                    GPIO_ResetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR2_PIN);
                    break;
                case MOTOR_DIR_BACKWARD:
                    GPIO_ResetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN);
                    GPIO_SetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR2_PIN);
                    break;
                case MOTOR_DIR_STOP:
                    GPIO_ResetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN);
                    break;
                case MOTOR_DIR_BRAKE:
                    GPIO_SetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN);
                    break;
            }
            break;

        case MOTOR_FL:
            switch (dir) {
                case MOTOR_DIR_FORWARD:
                    GPIO_SetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN);
                    GPIO_ResetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR2_PIN);
                    break;
                case MOTOR_DIR_BACKWARD:
                    GPIO_ResetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN);
                    GPIO_SetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR2_PIN);
                    break;
                case MOTOR_DIR_STOP:
                    GPIO_ResetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN);
                    break;
                case MOTOR_DIR_BRAKE:
                    GPIO_SetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN);
                    break;
            }
            break;

        case MOTOR_BR:
            switch (dir) {
                case MOTOR_DIR_FORWARD:
                    GPIO_SetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR1_PIN);
                    GPIO_ResetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR2_PIN);
                    break;
                case MOTOR_DIR_BACKWARD:
                    GPIO_ResetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR1_PIN);
                    GPIO_SetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR2_PIN);
                    break;
                case MOTOR_DIR_STOP:
                    GPIO_ResetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR1_PIN | MOTOR_BR_DIR2_PIN);
                    break;
                case MOTOR_DIR_BRAKE:
                    GPIO_SetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR1_PIN | MOTOR_BR_DIR2_PIN);
                    break;
            }
            break;

        case MOTOR_BL:
            switch (dir) {
                case MOTOR_DIR_FORWARD:
                    GPIO_SetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR1_PIN);
                    GPIO_ResetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR2_PIN);
                    break;
                case MOTOR_DIR_BACKWARD:
                    GPIO_ResetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR1_PIN);
                    GPIO_SetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR2_PIN);
                    break;
                case MOTOR_DIR_STOP:
                    GPIO_ResetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR1_PIN | MOTOR_BL_DIR2_PIN);
                    break;
                case MOTOR_DIR_BRAKE:
                    GPIO_SetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR1_PIN | MOTOR_BL_DIR2_PIN);
                    break;
            }
            break;

        default:
            break;
    }
}

/**
 * @brief 设置电机PWM占空比
 * @param motor 电机ID
 * @param duty 占空比 (0 ~ MOTOR_PWM_MAX_DUTY)
 */
void Motor_SetDuty(Motor_Id_e motor, uint16_t duty)
{
    if (duty > MOTOR_PWM_MAX_DUTY) {
        duty = MOTOR_PWM_MAX_DUTY;
    }

    switch (motor) {
        case MOTOR_FR:
            TIM_SetCompare1(MOTOR_FR_PWM_TIM, duty);
            break;
        case MOTOR_FL:
            TIM_SetCompare2(MOTOR_FL_PWM_TIM, duty);
            break;
        case MOTOR_BR:
            TIM_SetCompare1(MOTOR_BR_PWM_TIM, duty);
            break;
        case MOTOR_BL:
            TIM_SetCompare2(MOTOR_BL_PWM_TIM, duty);
            break;
        default:
            break;
    }
}

/**
 * @brief 停止指定电机
 * @param motor 电机ID
 */
void Motor_Stop(Motor_Id_e motor)
{
    Motor_SetDuty(motor, 0);
    Motor_SetDirection(motor, MOTOR_DIR_STOP);
}

/**
 * @brief 刹车指定电机
 * @param motor 电机ID
 */
void Motor_Brake(Motor_Id_e motor)
{
    Motor_SetDuty(motor, 0);
    Motor_SetDirection(motor, MOTOR_DIR_BRAKE);
}

/**
 * @brief 停止所有电机
 */
void Motor_StopAll(void)
{
    Motor_Stop(MOTOR_FR);
    Motor_Stop(MOTOR_FL);
    Motor_Stop(MOTOR_BR);
    Motor_Stop(MOTOR_BL);
}

/**
 * @brief 刹车所有电机
 */
void Motor_BrakeAll(void)
{
    Motor_Brake(MOTOR_FR);
    Motor_Brake(MOTOR_FL);
    Motor_Brake(MOTOR_BR);
    Motor_Brake(MOTOR_BL);
}

/**
 * @brief 启动所有电机定时器
 */
void Motor_StartTimers(void)
{
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
}

/**
 * @brief 停止所有电机定时器
 */
void Motor_StopTimers(void)
{
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    TIM_CtrlPWMOutputs(TIM8, DISABLE);
    TIM_Cmd(TIM1, DISABLE);
    TIM_Cmd(TIM8, DISABLE);
}
