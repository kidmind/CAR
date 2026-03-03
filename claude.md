# 智能车项目核心指南 (CLAUDE.md)
## 1. 项目核心信息
### 1.1 硬件架构
- 主控芯片：STM32F407ZGT6 (ARM Cortex-M4, 168MHz, 1MB Flash, 192KB RAM)
- 电机驱动：L298N (扩展为两路L298N，共四路H桥，支持四路直流电机驱动)
- 寻路模块：8路TCRT5000红外循迹传感器 (模拟/数字输出)
- 姿态/里程检测：
  - MPU6050 (三轴加速度+三轴陀螺仪)：采用**卡尔曼滤波**实现姿态解算（偏航角/俯仰角/横滚角）
  - 四路电机编码器 (每路电机独立编码器，测速/里程计算)
- 通信：USART (波特率115200，8N1，用于调试/数据输出)

### 1.2 核心功能目标
- 基础功能：通过8路TCRT传感器实现黑线/白线循迹
- 运动控制：
  - 四路编码器独立反馈转速，闭环控制保证四轮转速同步，提升直线行驶精度
  - MPU6050卡尔曼滤波解算偏航角，实时修正行驶偏航，结合编码器做二次校准
  - 结合编码器+卡尔曼滤波后的姿态角实现精准转弯（差速转弯/原地转弯，四轮独立调速）
- 硬件约束：
  - 两路L298N需独立供电，避免单电源过载
  - 每路电机需单独做过流保护，PWM频率建议20kHz（避免电机啸叫）
  - MPU6050采样频率需与卡尔曼滤波迭代频率匹配（100Hz），避免数据不同步

## 2. 开发/调试核心规则
### 2.1 代码规范
- 开发环境：VSCode + ARM-GCC (arm-none-eabi-gcc) + STM32标准库 (STM32F4xx_StdPeriph_Driver)
- 编译工具链：ARM-GCC 10.3或以上版本，链接脚本基于STM32F407ZGT6_FLASH.ld
- 面向对象代码风格（C语言实现）：
  - 「类」命名：前缀+功能，如 Motor_Class、MPU6050_Class、KalmanFilter_Class
  - 「成员变量」：结构体封装，私有变量加下划线前缀（如 _speed），公有变量无前缀
  - 「成员方法」：函数指针封装，命名格式：类名_方法名（如 Motor_Class_SetSpeed）
  - 「构造/析构」：统一命名为 Class_Init() / Class_DeInit()，负责资源初始化/释放
- 代码风格：
  - 函数名使用下划线命名 (motor_ctrl_set_speed)，全局常量大写 (PWM_FREQ_20KHZ)
  - 标准库函数调用遵循官方规范 (如 TIM_SetCompare1()、USART_SendData())
  - 中断服务函数命名：USART1_IRQHandler、TIM2_IRQHandler（符合标准库中断向量表）
  - 四路电机命名：MOTOR_FR(前右)、MOTOR_FL(前左)、MOTOR_BR(后右)、MOTOR_BL(后左)
  - 卡尔曼滤波函数命名：kalman_filter_init()、kalman_filter_update()
- 中断优先级：
  - 分组：NVIC_PriorityGroup_2 (2位抢占优先级，2位响应优先级)
  - 优先级排序：编码器中断 (抢占1) > MPU6050数据读取中断 (抢占2) > 卡尔曼滤波计算 (抢占2) > TCRT采样中断 (抢占3) > 串口中断 (抢占3)
- 定时器分配（四路电机专属）：
  - PWM输出（20kHz，1000级分辨率）：
    - TIM1 (高级定时器)：CH1=MOTOR_FR_PWM, CH2=MOTOR_FL_PWM
    - TIM8 (高级定时器)：CH1=MOTOR_BR_PWM, CH2=MOTOR_BL_PWM
    - 方向控制：GPIO口独立控制每路电机正反转（TIM1/8仅输出PWM，方向由GPIO控制）
  - 编码器计数（正交解码模式）：
    - TIM2：MOTOR_FR编码器
    - TIM3：MOTOR_FL编码器
    - TIM4：MOTOR_BR编码器
    - TIM5：MOTOR_BL编码器
  - 辅助定时器：
    - TIM6：TCRT传感器采样 (10ms周期，TIM_IT_Update中断)
    - TIM7：MPU6050数据读取 (10ms周期，100Hz，TIM_IT_Update中断)，触发卡尔曼滤波计算

### 2.2 关键技术参数
- 串口配置：USART1 (PA9/PA10)，波特率115200，无奇偶校验，1停止位
  - 标准库配置：USART_InitStructure.USART_BaudRate = 115200;
  - 时钟源：APB2时钟 (168MHz)，USART1时钟分频系数1
- PWM参数：20kHz频率，1000级分辨率
  - 配置：ARR=8399，PSC=0 (TIM_PrescalerConfig)，计数模式向上计数
  - 标准库实现：TIM_OC1Init() 配置PWM模式1，TIM_Cmd() 使能定时器
- 编码器参数：每路电机编码器参数一致（示例：11线编码器，减速比1:30）
  - 标准库配置：TIM_EncoderInterfaceConfig(TIMx, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising)
  - 计数范围：0-65535（16位定时器，溢出后清零重新计数）
- MPU6050 + 卡尔曼滤波参数：
  - MPU6050：I2C1 (PB6/PB7)，采样率100Hz，低通滤波5Hz
    - 标准库I2C配置：I2C_StandardMode (100kHz)，I2C_AcknowledgeConfig(I2C1, ENABLE)
  - 卡尔曼滤波（一维，针对偏航角）：
    - 过程噪声协方差 Q = 0.001（陀螺仪噪声）
    - 测量噪声协方差 R = 0.01（加速度计+磁力计融合噪声）
    - 状态估计协方差 P 初始值 = 1.0
    - 迭代频率：100Hz（与MPU6050采样频率一致）
    - 解算维度：优先解算偏航角（yaw），用于智能车方向控制；俯仰/横滚角可选解算

### 2.3 编译与调试规则
- 编译脚本：使用Makefile（核心编译参数如下）
  - 核心CFLAGS：-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -Wall
  - 链接参数：-T STM32F407ZGT6_FLASH.ld -Wl,-Map=output.map,--cref
- 烧录工具：openocd + cmsis-dap，VSCode配置launch.json调试
- 调试输出：串口打印使用自定义printf重定向（重映射fputc到USART_SendData）
  - 调试内容：每100ms输出一次卡尔曼滤波后的偏航角、四路电机转速
- 标准库宏定义：需在编译时添加 -DSTM32F407_ZX -DUSE_STDPERIPH_DRIVER -DQUAD_MOTOR_DRIVE -DKALMAN_FILTER_MPU6050

### 2.4 核心算法逻辑
- 循迹算法：8路TCRT数据归一化后，采用加权平均法计算偏差值，PID调节四轮转速（外侧电机增速/内侧电机减速）
- 直线行驶：
  - 基础层：四路编码器反馈转速，计算每路与基准转速的差值，PID独立补偿
  - 补偿层：MPU6050卡尔曼滤波解算偏航角，实时调整左右侧电机整体转速差，修正行驶方向（偏航角偏差>1°时启动补偿）
- 转弯控制：
  - 小角度转弯（<30°）：基于卡尔曼滤波后的偏航角闭环控制，差速调节左右侧电机转速（前/后电机转速同步）
  - 大角度/原地转弯（≥30°）：编码器计数控制每路电机转动距离，卡尔曼滤波后的偏航角校准最终角度，实现四轮独立调速的原地转向

## 3. 进阶文档索引（按需调用）
以下文档包含细分功能细节，仅在处理对应任务时阅读：
- `docs/motor_control.md`：四路电机PWM驱动、正反转、刹车逻辑及双L298N保护机制（标准库实现）
- `docs/tcrt_sampling.md`：8路TCRT传感器校准、ADC采样（标准库）、数据滤波、偏差计算
- `docs/encoder_mpu6050.md`：四路编码器计数（标准库正交解码）、MPU6050 I2C读写、卡尔曼滤波实现（标准库C代码）、四轮转速同步算法
- `docs/kalman_filter.md`：卡尔曼滤波核心公式、参数调优方法、偏航角解算实战（适配智能车场景）
- `docs/build_config.md`：Makefile完整配置、VSCode launch.json/tasks.json示例
- `docs/debug_log.md`：串口printf重定向实现、四路电机状态+卡尔曼滤波姿态角输出规范
