/**
 * @file encoder.c
 * @brief 四路编码器驱动实现文件 - 智能车专用
 * @version 1.0
 * @date 2025-02-25
 */

#include "encoder.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

/* ==================== 私有变量 ==================== */
static Encoder_Data_t Encoder_Data[ENCODER_MAX] = {0};

/* ==================== 编码器类实例 ==================== */
Encoder_Class_t Encoder = {0};

/* ==================== 私有函数声明 ==================== */
static void Encoder_GPIO_Init(Encoder_Id_e encoder);
static void Encoder_TIM_Init(Encoder_Id_e encoder);
static TIM_TypeDef* Encoder_GetTIM(Encoder_Id_e encoder);

/* ==================== 私有函数实现 ==================== */

/**
 * @brief 获取编码器对应的定时器
 * @param encoder 编码器ID
 * @return 定时器指针
 */
static TIM_TypeDef* Encoder_GetTIM(Encoder_Id_e encoder)
{
    switch (encoder) {
        case ENCODER_FR: return ENCODER_FR_TIM;
        case ENCODER_FL: return ENCODER_FL_TIM;
        case ENCODER_BR: return ENCODER_BR_TIM;
        case ENCODER_BL: return ENCODER_BL_TIM;
        default: return NULL;
    }
}

/**
 * @brief 初始化编码器GPIO
 * @param encoder 编码器ID
 */
static void Encoder_GPIO_Init(Encoder_Id_e encoder)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    switch (encoder) {
        case ENCODER_FR:
            RCC_AHB1PeriphClockCmd(ENCODER_FR_GPIO_CLK, ENABLE);

            // 配置CH1 (PA0 - TIM2_CH1)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_FR_CH1_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(ENCODER_FR_CH1_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_FR_CH1_PORT, GPIO_PinSource0, ENCODER_FR_AF);

            // 配置CH2 (PA1 - TIM2_CH2)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_FR_CH2_PIN;
            GPIO_Init(ENCODER_FR_CH2_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_FR_CH2_PORT, GPIO_PinSource1, ENCODER_FR_AF);
            break;

        case ENCODER_FL:
            RCC_AHB1PeriphClockCmd(ENCODER_FL_GPIO_CLK, ENABLE);

            // 配置CH1 (PA6 - TIM3_CH1)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_FL_CH1_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(ENCODER_FL_CH1_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_FL_CH1_PORT, GPIO_PinSource6, ENCODER_FL_AF);

            // 配置CH2 (PA7 - TIM3_CH2)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_FL_CH2_PIN;
            GPIO_Init(ENCODER_FL_CH2_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_FL_CH2_PORT, GPIO_PinSource7, ENCODER_FL_AF);
            break;

        case ENCODER_BR:
            RCC_AHB1PeriphClockCmd(ENCODER_BR_GPIO_CLK, ENABLE);

            // 配置CH1 (PD12 - TIM4_CH1)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_BR_CH1_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(ENCODER_BR_CH1_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_BR_CH1_PORT, GPIO_PinSource12, ENCODER_BR_AF);

            // 配置CH2 (PD13 - TIM4_CH2)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_BR_CH2_PIN;
            GPIO_Init(ENCODER_BR_CH2_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_BR_CH2_PORT, GPIO_PinSource13, ENCODER_BR_AF);
            break;

        case ENCODER_BL:
            RCC_AHB1PeriphClockCmd(ENCODER_BL_GPIO_CLK, ENABLE);

            // 配置CH1 (PA0 - TIM5_CH1)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_BL_CH1_PIN;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(ENCODER_BL_CH1_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_BL_CH1_PORT, GPIO_PinSource0, ENCODER_BL_AF);

            // 配置CH2 (PA1 - TIM5_CH2)
            GPIO_InitStructure.GPIO_Pin   = ENCODER_BL_CH2_PIN;
            GPIO_Init(ENCODER_BL_CH2_PORT, &GPIO_InitStructure);
            GPIO_PinAFConfig(ENCODER_BL_CH2_PORT, GPIO_PinSource1, ENCODER_BL_AF);
            break;

        default:
            break;
    }
}

/**
 * @brief 初始化编码器定时器为正交解码模式
 * @param encoder 编码器ID
 */
static void Encoder_TIM_Init(Encoder_Id_e encoder)
{
    TIM_TypeDef* tim = Encoder_GetTIM(encoder);
    uint32_t tim_clk = 0;

    if (tim == NULL) return;

    // 获取时钟
    switch (encoder) {
        case ENCODER_FR: tim_clk = ENCODER_FR_TIM_CLK; break;
        case ENCODER_FL: tim_clk = ENCODER_FL_TIM_CLK; break;
        case ENCODER_BR: tim_clk = ENCODER_BR_TIM_CLK; break;
        case ENCODER_BL: tim_clk = ENCODER_BL_TIM_CLK; break;
        default: return;
    }

    // 使能定时器时钟
    RCC_APB1PeriphClockCmd(tim_clk, ENABLE);

    // 定时器时基配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period         = 65535;   // 16位最大值
    TIM_TimeBaseStructure.TIM_Prescaler      = 0;        // 不分频
    TIM_TimeBaseStructure.TIM_ClockDivision  = 0;
    TIM_TimeBaseStructure.TIM_CounterMode    = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tim, &TIM_TimeBaseStructure);

    // 配置为正交编码器模式 (TI1和TI2都计数，4倍频)
    TIM_EncoderInterfaceConfig(tim,
                               TIM_EncoderMode_TI12,              // TI1和TI2都计数
                               TIM_ICPolarity_Rising,             // TI1上升沿
                               TIM_ICPolarity_Rising);            // TI2上升沿

    // 配置输入滤波器
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter    = 6;   // 滤波值
    TIM_ICInit(tim, &TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_2;
    TIM_ICInit(tim, &TIM_ICInitStructure);

    // 使能预装载
    TIM_ARRPreloadConfig(tim, ENABLE);

    // 复位计数器
    TIM_SetCounter(tim, 0);

    // 启动定时器
    TIM_Cmd(tim, ENABLE);
}

/* ==================== 公有函数实现 ==================== */

/**
 * @brief 编码器驱动初始化
 * @param encoder 编码器ID
 */
void Encoder_Init(Encoder_Id_e encoder)
{
    if (encoder >= ENCODER_MAX) return;

    // 初始化GPIO和定时器
    Encoder_GPIO_Init(encoder);
    Encoder_TIM_Init(encoder);

    // 初始化数据结构
    Encoder_Data[encoder].count           = 0;
    Encoder_Data[encoder].last_count      = 0;
    Encoder_Data[encoder].delta_count     = 0;
    Encoder_Data[encoder].speed_rpm       = 0.0f;
    Encoder_Data[encoder].speed_rps       = 0.0f;
    Encoder_Data[encoder].speed_rad_s     = 0.0f;
    Encoder_Data[encoder].speed_m_s       = 0.0f;
    Encoder_Data[encoder].total_count     = 0;
    Encoder_Data[encoder].total_distance  = 0.0f;
    Encoder_Data[encoder].direction       = ENCODER_DIR_STOPPED;
    Encoder_Data[encoder].wheel_diameter  = 0.065f;  // 默认65mm直径
    Encoder_Data[encoder].gear_ratio      = 1.0f;
    Encoder_Data[encoder].last_update_time = 0;

    Encoder._initialized |= (1 << encoder);
}

/**
 * @brief 编码器驱动初始化（所有编码器）
 */
void Encoder_InitAll(void)
{
    Encoder_Init(ENCODER_FR);
    Encoder_Init(ENCODER_FL);
    Encoder_Init(ENCODER_BR);
    Encoder_Init(ENCODER_BL);

    // 初始化类函数指针
    Encoder.Init       = Encoder_Init;
    Encoder.DeInit     = Encoder_DeInit;
    Encoder.Reset      = Encoder_Reset;
    Encoder.GetCount   = Encoder_GetCount;
    Encoder.GetTotalCount = Encoder_GetTotalCount;
    Encoder.GetSpeedRPM = Encoder_GetSpeedRPM;
    Encoder.GetSpeedRPS = Encoder_GetSpeedRPS;
    Encoder.GetSpeedRadS = Encoder_GetSpeedRadS;
    Encoder.GetSpeedMS  = Encoder_GetSpeedMS;
    Encoder.GetDistance = Encoder_GetDistance;
    Encoder.GetDirection = Encoder_GetDirection;
    Encoder.Update     = Encoder_Update;
    Encoder.UpdateAll  = Encoder_UpdateAll;
}

/**
 * @brief 编码器驱动去初始化
 * @param encoder 编码器ID
 */
void Encoder_DeInit(Encoder_Id_e encoder)
{
    TIM_TypeDef* tim = Encoder_GetTIM(encoder);

    if (tim != NULL) {
        TIM_Cmd(tim, DISABLE);
    }

    Encoder._initialized &= ~(1 << encoder);
}

/**
 * @brief 复位编码器计数值
 * @param encoder 编码器ID
 */
void Encoder_Reset(Encoder_Id_e encoder)
{
    TIM_TypeDef* tim = Encoder_GetTIM(encoder);

    if (tim != NULL) {
        TIM_SetCounter(tim, 0);
    }

    Encoder_Data[encoder].count           = 0;
    Encoder_Data[encoder].last_count      = 0;
    Encoder_Data[encoder].delta_count     = 0;
    Encoder_Data[encoder].total_count     = 0;
    Encoder_Data[encoder].total_distance  = 0.0f;
    Encoder_Data[encoder].speed_rpm       = 0.0f;
    Encoder_Data[encoder].speed_rps       = 0.0f;
    Encoder_Data[encoder].speed_rad_s     = 0.0f;
    Encoder_Data[encoder].speed_m_s       = 0.0f;
}

/**
 * @brief 复位所有编码器计数值
 */
void Encoder_ResetAll(void)
{
    for (Encoder_Id_e i = 0; i < ENCODER_MAX; i++) {
        Encoder_Reset(i);
    }
}

/**
 * @brief 获取编码器当前计数值
 * @param encoder 编码器ID
 * @return 当前计数值
 */
int16_t Encoder_GetCount(Encoder_Id_e encoder)
{
    TIM_TypeDef* tim = Encoder_GetTIM(encoder);

    if (tim != NULL) {
        Encoder_Data[encoder].count = (int16_t)TIM_GetCounter(tim);
    }

    return Encoder_Data[encoder].count;
}

/**
 * @brief 获取编码器总计数值
 * @param encoder 编码器ID
 * @return 总计数值（累计，处理溢出）
 */
int32_t Encoder_GetTotalCount(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].total_count;
}

/**
 * @brief 获取编码器转速
 * @param encoder 编码器ID
 * @return 转速 (RPM)
 */
float Encoder_GetSpeedRPM(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].speed_rpm;
}

/**
 * @brief 获取编码器转速
 * @param encoder 编码器ID
 * @return 转速 (RPS)
 */
float Encoder_GetSpeedRPS(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].speed_rps;
}

/**
 * @brief 获取编码器角速度
 * @param encoder 编码器ID
 * @return 角速度 (rad/s)
 */
float Encoder_GetSpeedRadS(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].speed_rad_s;
}

/**
 * @brief 获取编码器线速度
 * @param encoder 编码器ID
 * @return 线速度 (m/s)
 */
float Encoder_GetSpeedMS(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].speed_m_s;
}

/**
 * @brief 获取编码器里程
 * @param encoder 编码器ID
 * @return 里程 (m)
 */
float Encoder_GetDistance(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].total_distance;
}

/**
 * @brief 获取编码器方向
 * @param encoder 编码器ID
 * @return 方向
 */
Encoder_Direction_e Encoder_GetDirection(Encoder_Id_e encoder)
{
    return Encoder_Data[encoder].direction;
}

/**
 * @brief 更新编码器数据（测速计算）
 * @param encoder 编码器ID
 * @note  需要定期调用（如10ms周期）
 */
void Encoder_Update(Encoder_Id_e encoder)
{
    if (encoder >= ENCODER_MAX) return;

    // 读取当前计数值
    int16_t current_count = (int16_t)TIM_GetCounter(Encoder_GetTIM(encoder));

    // 计算增量（处理溢出）
    int16_t delta = current_count - Encoder_Data[encoder].last_count;

    // 处理溢出情况
    if (delta > 32767) {
        delta -= 65536;  // 向后溢出
    } else if (delta < -32768) {
        delta += 65536;  // 向前溢出
    }

    Encoder_Data[encoder].delta_count = delta;
    Encoder_Data[encoder].last_count = current_count;

    // 更新总计数值
    Encoder_Data[encoder].total_count += delta;

    // 判断方向
    if (delta > 0) {
        Encoder_Data[encoder].direction = ENCODER_DIR_FORWARD;
    } else if (delta < 0) {
        Encoder_Data[encoder].direction = ENCODER_DIR_BACKWARD;
    } else {
        Encoder_Data[encoder].direction = ENCODER_DIR_STOPPED;
    }

    // 计算转速 (RPM) = (delta / PPR) * (60000 / sample_period_ms)
    // 假设采样周期为10ms
    float revolutions = (float)delta / (float)ENCODER_PPR;
    Encoder_Data[encoder].speed_rps = revolutions * (1000.0f / ENCODER_SAMPLE_PERIOD_MS);
    Encoder_Data[encoder].speed_rpm = Encoder_Data[encoder].speed_rps * 60.0f;

    // 计算角速度 (rad/s) = speed_rps * 2 * PI
    Encoder_Data[encoder].speed_rad_s = Encoder_Data[encoder].speed_rps * 6.28318f;

    // 计算线速度 (m/s) = speed_rad_s * wheel_radius
    float radius = Encoder_Data[encoder].wheel_diameter / 2.0f;
    Encoder_Data[encoder].speed_m_s = Encoder_Data[encoder].speed_rad_s * radius;

    // 更新里程
    Encoder_Data[encoder].total_distance += (float)delta * PULSE_TO_RAD * radius;
}

/**
 * @brief 更新所有编码器数据
 */
void Encoder_UpdateAll(void)
{
    for (Encoder_Id_e i = 0; i < ENCODER_MAX; i++) {
        Encoder_Update(i);
    }
}

/**
 * @brief 设置轮子直径
 * @param encoder 编码器ID
 * @param diameter 直径 (m)
 */
void Encoder_SetWheelDiameter(Encoder_Id_e encoder, float diameter)
{
    if (encoder < ENCODER_MAX && diameter > 0) {
        Encoder_Data[encoder].wheel_diameter = diameter;
    }
}

/**
 * @brief 设置齿轮比
 * @param encoder 编码器ID
 * @param ratio 齿轮比
 */
void Encoder_SetGearRatio(Encoder_Id_e encoder, float ratio)
{
    if (encoder < ENCODER_MAX && ratio > 0) {
        Encoder_Data[encoder].gear_ratio = ratio;
    }
}
