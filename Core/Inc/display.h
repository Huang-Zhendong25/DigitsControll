/**
  ******************************************************************************
  * @file    display.h
  * @brief   5 位数码管“并行驱动”模块接口
  *
  * 设计要点：
  *   - 静态并行驱动：5 个数码管各占一个端口的连续 8 位段脚，无位选、无扫描，
  *     所有管的数据常驻输出，只在内容变化时更新。
  *   - 引脚分布（由 CubeMX 生成的 main.h 定义，勿改）：
  *       数码管1 = PD0~PD7   （PORTD 低字节）
  *       数码管2 = PD8~PD15  （PORTD 高字节）
  *       数码管3 = PE0~PE7   （PORTE 低字节）
  *       数码管4 = PE8~PE15  （PORTE 高字节）
  *       数码管5 = PB8~PB15  （PORTB 高字节）
  *   - 段位映射：bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g, bit7=dp
  *   - 热路径（Display_*）直接写寄存器 BSRR/ODR，不调用 HAL，避免逐脚写开销。
  ******************************************************************************
  */
#ifndef __DISPLAY_H
#define __DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ----------------------------------------------------------------------------
 * 段极性配置（按实际硬件二选一）：
 *   0 = 高电平点亮：共阴数码管，或“共阳 + 低边缓冲(NPN/N-MOS)”，GPIO 高 -> 段亮
 *   1 = 低电平点亮：共阳数码管直接灌流，GPIO 低 -> 段亮
 * 注意：本宏影响所有段的点亮逻辑，改后需重新编译。
 * -------------------------------------------------------------------------- */
#define SEG_ACTIVE_LOW  1

/* 计时步进（整数微秒）与计数模式的触发输入（1=PA6，2=PA7）。 */
#define DISPLAY_STEP_US        10u
#define DISPLAY_TRIGGER_INPUT  1u

/* 直接设置 8 个段脚的电平，bit0=a ... bit7=dp，bit=1 表示引脚输出高。
 * idx=0~4 对应硬件编号 DIG1~DIG5，即从右向左。 */
void Display_SetRaw(uint8_t idx, uint8_t seg);

/* 显示单个十六进制字符，取 value 的低 4 位，idx 与 Display_SetRaw 相同。 */
void Display_ShowDigit(uint8_t idx, uint8_t value);

/* 从左到右显示 5 位十进制数，DIG5 为万位，DIG1 为个位。
 * 不足 5 位时补前导零，超出时取末 5 位，小数点熄灭。 */
void Display_ShowNumber(uint32_t n);

/* 全灭：所有段脚输出“熄灭”电平 */
void Display_Blank(void);

/* GPIO 和定时器初始化后调用。显示 00000 并按控制引脚电平选择递增源。 */
void Display_Start(void);

/* 分别由 TIM2 更新、TIM3 触发和 SysTick 中断调用。 */
void Display_OnTimerUpdate(void);
void Display_OnTrigger(void);
void Display_CheckMode(void);

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H */
