/**
 * @file encoder.h
 * @brief 四路编码器驱动头文件 - 智能车专用
 * @version 1.0
 * @date 2025-02-25
 *
 * @details 本驱动实现四路正交编码器接口功能
 *          - 编码器模式：TIM2/TIM3/TIM4/TIM5 正交解码模式
 *          - 编码器类型：11线编码器，减速比1:30
 *          - 计数范围：0-65535（16位定时器）
 *          - 功能：测速、里程计算、方向检测
 *
 * @note  正交解码模式配置：
 *        - TI1FP1和TI2FP2连接到TIMx_CH1和TIMx_CH2
 *        - 上升沿和下降沿都计数（4倍频）
 *        - 自动方向检测
 */

#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f4xx.h"

/* ==================== 编码器枚举定义 ==================== */
/**
 * @brief 编码器编号枚举（对应四路电机）
 */
typedef enum {
  ENCODER_FR = 0, // 前右电机编码器 (Front Right) - TIM2
  ENCODER_FL = 1, // 前左电机编码器 (Front Left) - TIM3
  ENCODER_BR = 2, // 后右电机编码器 (Back Right) - TIM4
  ENCODER_BL = 3, // 后左电机编码器 (Back Left) - TIM5
  ENCODER_MAX = 4 // 编码器总数
} Encoder_Id_e;

/**
 * @brief 编码器方向枚举
 */
typedef enum {
  ENCODER_DIR_STOPPED = 0, // 停止
  ENCODER_DIR_FORWARD = 1, // 正转（计数增加）
  ENCODER_DIR_BACKWARD = 2 // 反转（计数减少）
} Encoder_Direction_e;

/* ==================== 编码器参数配置 ==================== */
// 编码器硬件参数
#define ENCODER_LINES 11           // 编码器线数
#define ENCODER_REDUCTION_RATIO 30 // 减速比
#define ENCODER_PPR                                                            \
  (ENCODER_LINES * 4 * ENCODER_REDUCTION_RATIO) // 每转脉冲数 (11*4*30=1320)

// 测速相关参数
#define ENCODER_SAMPLE_PERIOD_MS 10 // 采样周期 (ms)
#define ENCODER_SAMPLE_FREQ_HZ 100  // 采样频率 (Hz)

// 速度计算相关
#define RPM_TO_RPS (1.0f / 60.0f)             // RPM转RPS
#define PULSE_TO_RAD (6.28318f / ENCODER_PPR) // 脉冲转弧度

/* ==================== 前右编码器 (ENCODER_FR) - TIM2 ==================== */
#define ENCODER_FR_TIM TIM2
#define ENCODER_FR_TIM_CLK RCC_APB1Periph_TIM2
#define ENCODER_FR_CH1_PORT GPIOA
#define ENCODER_FR_CH1_PIN GPIO_Pin_0 // TIM2_CH1
#define ENCODER_FR_CH2_PORT GPIOA
#define ENCODER_FR_CH2_PIN GPIO_Pin_1 // TIM2_CH2
#define ENCODER_FR_GPIO_CLK RCC_AHB1Periph_GPIOA
#define ENCODER_FR_AF GPIO_AF_TIM2

/* ==================== 前左编码器 (ENCODER_FL) - TIM3 ==================== */
#define ENCODER_FL_TIM TIM3
#define ENCODER_FL_TIM_CLK RCC_APB1Periph_TIM3
#define ENCODER_FL_CH1_PORT GPIOA
#define ENCODER_FL_CH1_PIN GPIO_Pin_6 // TIM3_CH1
#define ENCODER_FL_CH2_PORT GPIOA
#define ENCODER_FL_CH2_PIN GPIO_Pin_7 // TIM3_CH2
#define ENCODER_FL_GPIO_CLK RCC_AHB1Periph_GPIOA
#define ENCODER_FL_AF GPIO_AF_TIM3

/* ==================== 后右编码器 (ENCODER_BR) - TIM4 ==================== */
#define ENCODER_BR_TIM TIM4
#define ENCODER_BR_TIM_CLK RCC_APB1Periph_TIM4
#define ENCODER_BR_CH1_PORT GPIOD
#define ENCODER_BR_CH1_PIN GPIO_Pin_12 // TIM4_CH1
#define ENCODER_BR_CH2_PORT GPIOD
#define ENCODER_BR_CH2_PIN GPIO_Pin_13 // TIM4_CH2
#define ENCODER_BR_GPIO_CLK RCC_AHB1Periph_GPIOD
#define ENCODER_BR_AF GPIO_AF_TIM4

/* ==================== 后左编码器 (ENCODER_BL) - TIM5 ==================== */
#define ENCODER_BL_TIM TIM5
#define ENCODER_BL_TIM_CLK RCC_APB1Periph_TIM5
#define ENCODER_BL_CH1_PORT GPIOA
#define ENCODER_BL_CH1_PIN GPIO_Pin_0 // TIM5_CH1
#define ENCODER_BL_CH2_PORT GPIOA
#define ENCODER_BL_CH2_PIN GPIO_Pin_1 // TIM5_CH2
#define ENCODER_BL_GPIO_CLK RCC_AHB1Periph_GPIOA
#define ENCODER_BL_AF GPIO_AF_TIM5

/* ==================== 编码器数据结构体 ==================== */
/**
 * @brief 编码器数据结构体
 */
typedef struct {
  // 当前状态
  int16_t count;       // 当前计数值
  int16_t last_count;  // 上次计数值
  int16_t delta_count; // 计数增量

  // 速度相关
  float speed_rpm;   // 转速 (RPM)
  float speed_rps;   // 转速 (RPS)
  float speed_rad_s; // 角速度 (rad/s)
  float speed_m_s;   // 线速度 (m/s)

  // 里程相关
  int32_t total_count;  // 总计数值（累计）
  float total_distance; // 总里程 (m)

  // 方向
  Encoder_Direction_e direction; // 当前方向

  // 参数
  float wheel_diameter; // 轮子直径 (m)
  float gear_ratio;     // 齿轮比

  // 时间戳
  uint32_t last_update_time; // 上次更新时间
} Encoder_Data_t;

/* ==================== 编码器类结构体 ==================== */
/**
 * @brief 编码器控制类结构体（面向对象风格）
 */
typedef struct {
  // 私有变量（带下划线前缀）
  Encoder_Data_t _data[ENCODER_MAX]; // 四路编码器数据
  uint8_t _initialized;              // 初始化标志

  // 公有变量

  // 构造/析构函数
  void (*Init)(Encoder_Id_e encoder);
  void (*DeInit)(Encoder_Id_e encoder);

  // 成员方法
  void (*Reset)(Encoder_Id_e encoder);            // 复位计数值
  int16_t (*GetCount)(Encoder_Id_e encoder);      // 获取计数值
  int32_t (*GetTotalCount)(Encoder_Id_e encoder); // 获取总计数值
  float (*GetSpeedRPM)(Encoder_Id_e encoder);     // 获取转速(RPM)
  float (*GetSpeedRPS)(Encoder_Id_e encoder);     // 获取转速(RPS)
  float (*GetSpeedRadS)(Encoder_Id_e encoder);    // 获取角速度(rad/s)
  float (*GetSpeedMS)(Encoder_Id_e encoder);      // 获取线速度(m/s)
  float (*GetDistance)(Encoder_Id_e encoder);     // 获取里程(m)
  Encoder_Direction_e (*GetDirection)(Encoder_Id_e encoder); // 获取方向
  void (*Update)(Encoder_Id_e encoder);                      // 更新数据（测速）
  void (*UpdateAll)(void);                                   // 更新所有编码器
} Encoder_Class_t;

/* ==================== 全局变量声明 ==================== */
extern Encoder_Class_t Encoder;

/* ==================== 全局函数声明 ==================== */

/**
 * @brief 编码器驱动初始化
 * @param encoder 编码器ID
 * @note  初始化指定编码器的GPIO和定时器为正交解码模式
 */
void Encoder_Init(Encoder_Id_e encoder);

/**
 * @brief 编码器驱动初始化（所有编码器）
 */
void Encoder_InitAll(void);

/**
 * @brief 编码器驱动去初始化
 * @param encoder 编码器ID
 */
void Encoder_DeInit(Encoder_Id_e encoder);

/**
 * @brief 复位编码器计数值
 * @param encoder 编码器ID
 */
void Encoder_Reset(Encoder_Id_e encoder);

/**
 * @brief 复位所有编码器计数值
 */
void Encoder_ResetAll(void);

/**
 * @brief 获取编码器当前计数值
 * @param encoder 编码器ID
 * @return 当前计数值
 */
int16_t Encoder_GetCount(Encoder_Id_e encoder);

/**
 * @brief 获取编码器总计数值
 * @param encoder 编码器ID
 * @return 总计数值（累计，处理溢出）
 */
int32_t Encoder_GetTotalCount(Encoder_Id_e encoder);

/**
 * @brief 获取编码器转速
 * @param encoder 编码器ID
 * @return 转速 (RPM)
 */
float Encoder_GetSpeedRPM(Encoder_Id_e encoder);

/**
 * @brief 获取编码器转速
 * @param encoder 编码器ID
 * @return 转速 (RPS - Revolutions Per Second)
 */
float Encoder_GetSpeedRPS(Encoder_Id_e encoder);

/**
 * @brief 获取编码器角速度
 * @param encoder 编码器ID
 * @return 角速度 (rad/s)
 */
float Encoder_GetSpeedRadS(Encoder_Id_e encoder);

/**
 * @brief 获取编码器线速度
 * @param encoder 编码器ID
 * @return 线速度 (m/s)
 */
float Encoder_GetSpeedMS(Encoder_Id_e encoder);

/**
 * @brief 获取编码器里程
 * @param encoder 编码器ID
 * @return 里程 (m)
 */
float Encoder_GetDistance(Encoder_Id_e encoder);

/**
 * @brief 获取编码器方向
 * @param encoder 编码器ID
 * @return 方向 (FORWARD/BACKWARD/STOPPED)
 */
Encoder_Direction_e Encoder_GetDirection(Encoder_Id_e encoder);

/**
 * @brief 更新编码器数据（测速计算）
 * @param encoder 编码器ID
 * @note  需要定期调用（如10ms周期）
 */
void Encoder_Update(Encoder_Id_e encoder);

/**
 * @brief 更新所有编码器数据
 */
void Encoder_UpdateAll(void);

/**
 * @brief 设置轮子直径
 * @param encoder 编码器ID
 * @param diameter 直径 (m)
 */
void Encoder_SetWheelDiameter(Encoder_Id_e encoder, float diameter);

/**
 * @brief 设置齿轮比
 * @param encoder 编码器ID
 * @param ratio 齿轮比
 */
void Encoder_SetGearRatio(Encoder_Id_e encoder, float ratio);

/* ==================== 宏定义快捷函数 ==================== */

// 前右编码器
#define Encoder_FR_GetCount() Encoder_GetCount(ENCODER_FR)
#define Encoder_FR_GetSpeedRPM() Encoder_GetSpeedRPM(ENCODER_FR)
#define Encoder_FR_GetSpeedMS() Encoder_GetSpeedMS(ENCODER_FR)

// 前左编码器
#define Encoder_FL_GetCount() Encoder_GetCount(ENCODER_FL)
#define Encoder_FL_GetSpeedRPM() Encoder_GetSpeedRPM(ENCODER_FL)
#define Encoder_FL_GetSpeedMS() Encoder_GetSpeedMS(ENCODER_FL)

// 后右编码器
#define Encoder_BR_GetCount() Encoder_GetCount(ENCODER_BR)
#define Encoder_BR_GetSpeedRPM() Encoder_GetSpeedRPM(ENCODER_BR)
#define Encoder_BR_GetSpeedMS() Encoder_GetSpeedMS(ENCODER_BR)

// 后左编码器
#define Encoder_BL_GetCount() Encoder_GetCount(ENCODER_BL)
#define Encoder_BL_GetSpeedRPM() Encoder_GetSpeedRPM(ENCODER_BL)
#define Encoder_BL_GetSpeedMS() Encoder_GetSpeedMS(ENCODER_BL)

#endif /* __ENCODER_H */
