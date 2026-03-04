/**
 * @file motor.c
 * @brief 电机PWM驱动实现文件 - 智能车专用（面向对象风格）
 * @version 2.0
 * @date 2026-03-04
 */

#include "motor.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

/* ==================== 全局电机实例定义 ==================== */
// 四驱模式电机实例
#ifdef QUAD_MOTOR_DRIVE
Motor_Class_t Motor_FR = {0};
Motor_Class_t Motor_FL = {0};
Motor_Class_t Motor_BR = {0};
Motor_Class_t Motor_BL = {0};
#else
// 双驱模式电机实例
Motor_Class_t Motor_Right = {0};
Motor_Class_t Motor_Left = {0};
#endif

/* ==================== 私有函数声明 ==================== */
static void Motor_GPIO_Init(Motor_Id_e motor);
static void Motor_TIM_Init(TIM_TypeDef* tim, uint32_t tim_clk);
static void Motor_TIM_Channel_Init(TIM_TypeDef* tim, uint16_t channel);
static void Motor_Instance_SetSpeed(Motor_Class_t* inst, int16_t speed);

/* ==================== 私有函数：辅助函数 ==================== */

/**
 * @brief 设置单个实例的速度
 * @param inst 电机实例指针
 * @param speed 速度值 (-MOTOR_PWM_MAX_DUTY ~ +MOTOR_PWM_MAX_DUTY)
 */
static void Motor_Instance_SetSpeed(Motor_Class_t* inst, int16_t speed)
{
    // 限制速度范围
    if (speed > MOTOR_PWM_MAX_DUTY) {
        speed = MOTOR_PWM_MAX_DUTY;
    } else if (speed < -MOTOR_PWM_MAX_DUTY) {
        speed = -MOTOR_PWM_MAX_DUTY;
    }

    // 根据速度正负设置方向和占空比
    if (speed > 0) {
        inst->SetDirection(MOTOR_DIR_FORWARD);
        inst->SetDuty((uint16_t)speed);
    } else if (speed < 0) {
        inst->SetDirection(MOTOR_DIR_BACKWARD);
        inst->SetDuty((uint16_t)(-speed));
    } else {
        inst->Stop();
    }
}

/* ==================== 私有函数：单个实例操作 ==================== */

#ifdef QUAD_MOTOR_DRIVE

/**
 * @brief 前右电机实例操作
 */
static void Motor_FR_Init(void) {
    Motor_GPIO_Init(MOTOR_FR);
    Motor_FR.initialized = 1;
}
static void Motor_FR_DeInit(void) {
    Motor_FR.initialized = 0;
}
static void Motor_FR_SetSpeed(int16_t speed) {
    Motor_Instance_SetSpeed(&Motor_FR, speed);
}
static void Motor_FR_SetDirection(Motor_Direction_e dir) {
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
}
static void Motor_FR_SetDuty(uint16_t duty) {
    if (duty > MOTOR_PWM_MAX_DUTY) duty = MOTOR_PWM_MAX_DUTY;
    TIM_SetCompare1(MOTOR_FR_PWM_TIM, duty);
}
static void Motor_FR_Stop(void) {
    Motor_FR_SetDuty(0);
    Motor_FR_SetDirection(MOTOR_DIR_STOP);
}
static void Motor_FR_Brake(void) {
    Motor_FR_SetDuty(0);
    Motor_FR_SetDirection(MOTOR_DIR_BRAKE);
}

/**
 * @brief 前左电机实例操作
 */
static void Motor_FL_Init(void) {
    Motor_GPIO_Init(MOTOR_FL);
    Motor_FL.initialized = 1;
}
static void Motor_FL_DeInit(void) {
    Motor_FL.initialized = 0;
}
static void Motor_FL_SetSpeed(int16_t speed) {
    Motor_Instance_SetSpeed(&Motor_FL, speed);
}
static void Motor_FL_SetDirection(Motor_Direction_e dir) {
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
}
static void Motor_FL_SetDuty(uint16_t duty) {
    if (duty > MOTOR_PWM_MAX_DUTY) duty = MOTOR_PWM_MAX_DUTY;
    TIM_SetCompare2(MOTOR_FL_PWM_TIM, duty);
}
static void Motor_FL_Stop(void) {
    Motor_FL_SetDuty(0);
    Motor_FL_SetDirection(MOTOR_DIR_STOP);
}
static void Motor_FL_Brake(void) {
    Motor_FL_SetDuty(0);
    Motor_FL_SetDirection(MOTOR_DIR_BRAKE);
}

/**
 * @brief 后右电机实例操作
 */
static void Motor_BR_Init(void) {
    Motor_GPIO_Init(MOTOR_BR);
    Motor_BR.initialized = 1;
}
static void Motor_BR_DeInit(void) {
    Motor_BR.initialized = 0;
}
static void Motor_BR_SetSpeed(int16_t speed) {
    Motor_Instance_SetSpeed(&Motor_BR, speed);
}
static void Motor_BR_SetDirection(Motor_Direction_e dir) {
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
}
static void Motor_BR_SetDuty(uint16_t duty) {
    if (duty > MOTOR_PWM_MAX_DUTY) duty = MOTOR_PWM_MAX_DUTY;
    TIM_SetCompare1(MOTOR_BR_PWM_TIM, duty);
}
static void Motor_BR_Stop(void) {
    Motor_BR_SetDuty(0);
    Motor_BR_SetDirection(MOTOR_DIR_STOP);
}
static void Motor_BR_Brake(void) {
    Motor_BR_SetDuty(0);
    Motor_BR_SetDirection(MOTOR_DIR_BRAKE);
}

/**
 * @brief 后左电机实例操作
 */
static void Motor_BL_Init(void) {
    Motor_GPIO_Init(MOTOR_BL);
    Motor_BL.initialized = 1;
}
static void Motor_BL_DeInit(void) {
    Motor_BL.initialized = 0;
}
static void Motor_BL_SetSpeed(int16_t speed) {
    Motor_Instance_SetSpeed(&Motor_BL, speed);
}
static void Motor_BL_SetDirection(Motor_Direction_e dir) {
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
}
static void Motor_BL_SetDuty(uint16_t duty) {
    if (duty > MOTOR_PWM_MAX_DUTY) duty = MOTOR_PWM_MAX_DUTY;
    TIM_SetCompare2(MOTOR_BL_PWM_TIM, duty);
}
static void Motor_BL_Stop(void) {
    Motor_BL_SetDuty(0);
    Motor_BL_SetDirection(MOTOR_DIR_STOP);
}
static void Motor_BL_Brake(void) {
    Motor_BL_SetDuty(0);
    Motor_BL_SetDirection(MOTOR_DIR_BRAKE);
}

#else

/**
 * @brief 双驱模式：右电机实例操作（使用前右硬件）
 */
static void Motor_Right_Init(void) {
    Motor_GPIO_Init(MOTOR_RIGHT);
    Motor_Right.initialized = 1;
}
static void Motor_Right_DeInit(void) {
    Motor_Right.initialized = 0;
}
static void Motor_Right_SetSpeed(int16_t speed) {
    Motor_Instance_SetSpeed(&Motor_Right, speed);
}
static void Motor_Right_SetDirection(Motor_Direction_e dir) {
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
}
static void Motor_Right_SetDuty(uint16_t duty) {
    if (duty > MOTOR_PWM_MAX_DUTY) duty = MOTOR_PWM_MAX_DUTY;
    TIM_SetCompare1(MOTOR_FR_PWM_TIM, duty);
}
static void Motor_Right_Stop(void) {
    Motor_Right_SetDuty(0);
    Motor_Right_SetDirection(MOTOR_DIR_STOP);
}
static void Motor_Right_Brake(void) {
    Motor_Right_SetDuty(0);
    Motor_Right_SetDirection(MOTOR_DIR_BRAKE);
}

/**
 * @brief 双驱模式：左电机实例操作（使用前左硬件）
 */
static void Motor_Left_Init(void) {
    Motor_GPIO_Init(MOTOR_LEFT);
    Motor_Left.initialized = 1;
}
static void Motor_Left_DeInit(void) {
    Motor_Left.initialized = 0;
}
static void Motor_Left_SetSpeed(int16_t speed) {
    Motor_Instance_SetSpeed(&Motor_Left, speed);
}
static void Motor_Left_SetDirection(Motor_Direction_e dir) {
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
}
static void Motor_Left_SetDuty(uint16_t duty) {
    if (duty > MOTOR_PWM_MAX_DUTY) duty = MOTOR_PWM_MAX_DUTY;
    TIM_SetCompare2(MOTOR_FL_PWM_TIM, duty);
}
static void Motor_Left_Stop(void) {
    Motor_Left_SetDuty(0);
    Motor_Left_SetDirection(MOTOR_DIR_STOP);
}
static void Motor_Left_Brake(void) {
    Motor_Left_SetDuty(0);
    Motor_Left_SetDirection(MOTOR_DIR_BRAKE);
}

#endif

/* ==================== 私有函数：硬件初始化 ==================== */

/**
 * @brief 初始化单个电机的GPIO引脚
 * @param motor 电机ID
 */
static void Motor_GPIO_Init(Motor_Id_e motor)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#ifdef QUAD_MOTOR_DRIVE
    // 四驱模式：使用所有电机硬件
    switch (motor) {
        case MOTOR_FR:
            RCC_AHB1PeriphClockCmd(MOTOR_FR_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_FR_DIR_CLK, ENABLE);
            GPIO_InitStructure.GPIO_Pin = MOTOR_FR_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_FR_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_FR_PWM_PORT, GPIO_PinSource9, MOTOR_FR_PWM_AF);
            GPIO_InitStructure.GPIO_Pin = MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_FR_DIR_PORT, &GPIO_InitStructure);
            GPIO_ResetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN);
            break;

        case MOTOR_FL:
            RCC_AHB1PeriphClockCmd(MOTOR_FL_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_FL_DIR_CLK, ENABLE);
            GPIO_InitStructure.GPIO_Pin = MOTOR_FL_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_FL_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_FL_PWM_PORT, GPIO_PinSource11, MOTOR_FL_PWM_AF);
            GPIO_InitStructure.GPIO_Pin = MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_FL_DIR_PORT, &GPIO_InitStructure);
            GPIO_ResetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN);
            break;

        case MOTOR_BR:
            RCC_AHB1PeriphClockCmd(MOTOR_BR_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_BR_DIR_CLK, ENABLE);
            GPIO_InitStructure.GPIO_Pin = MOTOR_BR_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_BR_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_BR_PWM_PORT, GPIO_PinSource6, MOTOR_BR_PWM_AF);
            GPIO_InitStructure.GPIO_Pin = MOTOR_BR_DIR1_PIN | MOTOR_BR_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_BR_DIR_PORT, &GPIO_InitStructure);
            GPIO_ResetBits(MOTOR_BR_DIR_PORT, MOTOR_BR_DIR1_PIN | MOTOR_BR_DIR2_PIN);
            break;

        case MOTOR_BL:
            RCC_AHB1PeriphClockCmd(MOTOR_BL_PWM_CLK, ENABLE);
            RCC_AHB1PeriphClockCmd(MOTOR_BL_DIR_CLK, ENABLE);
            GPIO_InitStructure.GPIO_Pin = MOTOR_BL_PWM_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_Init(MOTOR_BL_PWM_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(MOTOR_BL_PWM_PORT, GPIO_PinSource7, MOTOR_BL_PWM_AF);
            GPIO_InitStructure.GPIO_Pin = MOTOR_BL_DIR1_PIN | MOTOR_BL_DIR2_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(MOTOR_BL_DIR_PORT, &GPIO_InitStructure);
            GPIO_ResetBits(MOTOR_BL_DIR_PORT, MOTOR_BL_DIR1_PIN | MOTOR_BL_DIR2_PIN);
            break;
    }
#else
    // 双驱模式：仅使用前轮硬件
    if (motor == MOTOR_RIGHT) {
        RCC_AHB1PeriphClockCmd(MOTOR_FR_PWM_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(MOTOR_FR_DIR_CLK, ENABLE);
        GPIO_InitStructure.GPIO_Pin = MOTOR_FR_PWM_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(MOTOR_FR_PWM_PORT, &GPIO_InitStructure);
        GPIO_PinAFConfig(MOTOR_FR_PWM_PORT, GPIO_PinSource9, MOTOR_FR_PWM_AF);
        GPIO_InitStructure.GPIO_Pin = MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(MOTOR_FR_DIR_PORT, &GPIO_InitStructure);
        GPIO_ResetBits(MOTOR_FR_DIR_PORT, MOTOR_FR_DIR1_PIN | MOTOR_FR_DIR2_PIN);
    } else if (motor == MOTOR_LEFT) {
        RCC_AHB1PeriphClockCmd(MOTOR_FL_PWM_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(MOTOR_FL_DIR_CLK, ENABLE);
        GPIO_InitStructure.GPIO_Pin = MOTOR_FL_PWM_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(MOTOR_FL_PWM_PORT, &GPIO_InitStructure);
        GPIO_PinAFConfig(MOTOR_FL_PWM_PORT, GPIO_PinSource11, MOTOR_FL_PWM_AF);
        GPIO_InitStructure.GPIO_Pin = MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(MOTOR_FL_DIR_PORT, &GPIO_InitStructure);
        GPIO_ResetBits(MOTOR_FL_DIR_PORT, MOTOR_FL_DIR1_PIN | MOTOR_FL_DIR2_PIN);
    }
#endif
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
    TIM_TimeBaseStructure.TIM_Period = MOTOR_PWM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = MOTOR_PWM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tim, &TIM_TimeBaseStructure);
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
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
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
    }
}

/* ==================== 公有函数实现 ==================== */

/**
 * @brief 电机驱动初始化
 */
void Motor_Driver_Init(void)
{
    // 初始化TIM1 (前右、前左电机 / 双驱模式下的右、左电机)
    Motor_TIM_Init(MOTOR_FR_PWM_TIM, MOTOR_FR_PWM_TIM_CLK);
    Motor_TIM_Channel_Init(MOTOR_FR_PWM_TIM, MOTOR_FR_PWM_CHANNEL);
    Motor_TIM_Channel_Init(MOTOR_FR_PWM_TIM, MOTOR_FL_PWM_CHANNEL);

#ifdef QUAD_MOTOR_DRIVE
    // 初始化TIM8 (后右、后左电机)
    Motor_TIM_Init(MOTOR_BR_PWM_TIM, MOTOR_BR_PWM_TIM_CLK);
    Motor_TIM_Channel_Init(MOTOR_BR_PWM_TIM, MOTOR_BR_PWM_CHANNEL);
    Motor_TIM_Channel_Init(MOTOR_BR_PWM_TIM, MOTOR_BL_PWM_CHANNEL);
#endif

    // 使能PWM输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
#ifdef QUAD_MOTOR_DRIVE
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
#endif

#ifdef QUAD_MOTOR_DRIVE
    // 初始化四驱电机实例的函数指针
    Motor_FR.id = MOTOR_FR; Motor_FR.tim = MOTOR_FR_PWM_TIM; Motor_FR.channel = MOTOR_FR_PWM_CHANNEL;
    Motor_FR.Init = Motor_FR_Init; Motor_FR.DeInit = Motor_FR_DeInit;
    Motor_FR.SetSpeed = Motor_FR_SetSpeed; Motor_FR.SetDirection = Motor_FR_SetDirection;
    Motor_FR.SetDuty = Motor_FR_SetDuty; Motor_FR.Stop = Motor_FR_Stop; Motor_FR.Brake = Motor_FR_Brake;

    Motor_FL.id = MOTOR_FL; Motor_FL.tim = MOTOR_FL_PWM_TIM; Motor_FL.channel = MOTOR_FL_PWM_CHANNEL;
    Motor_FL.Init = Motor_FL_Init; Motor_FL.DeInit = Motor_FL_DeInit;
    Motor_FL.SetSpeed = Motor_FL_SetSpeed; Motor_FL.SetDirection = Motor_FL_SetDirection;
    Motor_FL.SetDuty = Motor_FL_SetDuty; Motor_FL.Stop = Motor_FL_Stop; Motor_FL.Brake = Motor_FL_Brake;

    Motor_BR.id = MOTOR_BR; Motor_BR.tim = MOTOR_BR_PWM_TIM; Motor_BR.channel = MOTOR_BR_PWM_CHANNEL;
    Motor_BR.Init = Motor_BR_Init; Motor_BR.DeInit = Motor_BR_DeInit;
    Motor_BR.SetSpeed = Motor_BR_SetSpeed; Motor_BR.SetDirection = Motor_BR_SetDirection;
    Motor_BR.SetDuty = Motor_BR_SetDuty; Motor_BR.Stop = Motor_BR_Stop; Motor_BR.Brake = Motor_BR_Brake;

    Motor_BL.id = MOTOR_BL; Motor_BL.tim = MOTOR_BL_PWM_TIM; Motor_BL.channel = MOTOR_BL_PWM_CHANNEL;
    Motor_BL.Init = Motor_BL_Init; Motor_BL.DeInit = Motor_BL_DeInit;
    Motor_BL.SetSpeed = Motor_BL_SetSpeed; Motor_BL.SetDirection = Motor_BL_SetDirection;
    Motor_BL.SetDuty = Motor_BL_SetDuty; Motor_BL.Stop = Motor_BL_Stop; Motor_BL.Brake = Motor_BL_Brake;

    // 初始化所有电机实例
    Motor_FR.Init();
    Motor_FL.Init();
    Motor_BR.Init();
    Motor_BL.Init();
#else
    // 初始化双驱电机实例的函数指针
    Motor_Right.id = MOTOR_RIGHT; Motor_Right.tim = MOTOR_FR_PWM_TIM; Motor_Right.channel = MOTOR_FR_PWM_CHANNEL;
    Motor_Right.Init = Motor_Right_Init; Motor_Right.DeInit = Motor_Right_DeInit;
    Motor_Right.SetSpeed = Motor_Right_SetSpeed; Motor_Right.SetDirection = Motor_Right_SetDirection;
    Motor_Right.SetDuty = Motor_Right_SetDuty; Motor_Right.Stop = Motor_Right_Stop; Motor_Right.Brake = Motor_Right_Brake;

    Motor_Left.id = MOTOR_LEFT; Motor_Left.tim = MOTOR_FL_PWM_TIM; Motor_Left.channel = MOTOR_FL_PWM_CHANNEL;
    Motor_Left.Init = Motor_Left_Init; Motor_Left.DeInit = Motor_Left_DeInit;
    Motor_Left.SetSpeed = Motor_Left_SetSpeed; Motor_Left.SetDirection = Motor_Left_SetDirection;
    Motor_Left.SetDuty = Motor_Left_SetDuty; Motor_Left.Stop = Motor_Left_Stop; Motor_Left.Brake = Motor_Left_Brake;

    // 初始化所有电机实例
    Motor_Right.Init();
    Motor_Left.Init();
#endif
}

/**
 * @brief 电机驱动去初始化
 */
void Motor_Driver_DeInit(void)
{
    Motor_StopAll();
    TIM_Cmd(TIM1, DISABLE);
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
#ifdef QUAD_MOTOR_DRIVE
    TIM_Cmd(TIM8, DISABLE);
    TIM_CtrlPWMOutputs(TIM8, DISABLE);
#endif
}

/**
 * @brief 停止所有电机
 */
void Motor_StopAll(void)
{
#ifdef QUAD_MOTOR_DRIVE
    Motor_FR.Stop();
    Motor_FL.Stop();
    Motor_BR.Stop();
    Motor_BL.Stop();
#else
    Motor_Right.Stop();
    Motor_Left.Stop();
#endif
}

/**
 * @brief 刹车所有电机
 */
void Motor_BrakeAll(void)
{
#ifdef QUAD_MOTOR_DRIVE
    Motor_FR.Brake();
    Motor_FL.Brake();
    Motor_BR.Brake();
    Motor_BL.Brake();
#else
    Motor_Right.Brake();
    Motor_Left.Brake();
#endif
}

/**
 * @brief 启动所有电机定时器
 */
void Motor_StartTimers(void)
{
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
#ifdef QUAD_MOTOR_DRIVE
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
#endif
}

/**
 * @brief 停止所有电机定时器
 */
void Motor_StopTimers(void)
{
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    TIM_Cmd(TIM1, DISABLE);
#ifdef QUAD_MOTOR_DRIVE
    TIM_CtrlPWMOutputs(TIM8, DISABLE);
    TIM_Cmd(TIM8, DISABLE);
#endif
}
