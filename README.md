# Digits 五位数码管控制器

本项目使用 STM32F407VGT6 静态并行驱动五位七段数码管。每位数码管独占八个 GPIO 段脚，输出保持到下一次写入。显示范围为 `00000`～`99999`，递增到末值后回到 `00000`。

## 接线与控制

| 显示位置 | 硬件编号 | 段脚按 `a b c d e f g dp` 顺序排列 |
| --- | --- | --- |
| 最左，万位 | `DIG5` | `PB8–PB15` |
| 千位 | `DIG4` | `PE8–PE15` |
| 百位 | `DIG3` | `PE0–PE7` |
| 十位 | `DIG2` | `PD8–PD15` |
| 最右，个位 | `DIG1` | `PD0–PD7` |

段脚设为高速推挽输出。当前硬件逻辑为低电平点亮，GPIO 初始化时先将段脚置高。正常数字显示时小数点熄灭。完整引脚定义见 [`Core/Inc/main.h`](Core/Inc/main.h)。

| 信号 | 引脚 | 固件行为 |
| --- | --- | --- |
| `Controll_Mode` | `PC4` | 高电平选择计时模式，低电平选择外部计数模式。切换通过消抖后显示清零。 |
| `TRIG_SW` | `PA5` | 高电平运行，低电平暂停。单独暂停或恢复会保留显示值。 |
| `TRIG_SIG1` | `PA6` / `TIM3_CH1` | 外部计数输入 1，当前选用，检测上升沿。 |
| `TRIG_SIG2` | `PA7` / `TIM3_CH2` | 外部计数输入 2，可用宏选用，检测上升沿。 |

## 编译时配置

在 [`Core/Inc/display.h`](Core/Inc/display.h) 修改对应的宏，重新编译并烧录。

| 宏 | 当前值 | 用途 |
| --- | --- | --- |
| `DISPLAY_STEP_US` | `10u` | 计时模式的递增间隔，单位为整数微秒。当前时钟下可设 `1`～`51130563`。 |
| `DISPLAY_TRIGGER_INPUT` | `1u` | 外部计数的输入选择。`1u` 选 PA6，`2u` 选 PA7。 |
| `SEG_ACTIVE_LOW` | `1` | 段点亮极性。`1` 为低有效，`0` 为高有效。硬件极性变更时同步核对 CubeMX 的 GPIO 初始电平。 |

工程采用 25 MHz HSE，PLL 的 `M/N/P/Q` 为 `25/336/2/7`，系统时钟为 168 MHz。APB1 为 42 MHz，TIM2 和 TIM3 的定时器时钟为 84 MHz。计时步进由 `Display_Start()` 根据当前定时器时钟设置，`PSC = 0`，`ARR = 84 × DISPLAY_STEP_US − 1`。在当前 `10u` 下，`ARR` 为 `839`。

## 代码实现与运行流程

1. **上电初始化。** [`main.c`](Core/Src/main.c) 调用 CubeMX 生成的时钟、GPIO、TIM2 和 TIM3 初始化，再调用 [`Display_Start()`](Core/Src/display.c)。显示模块生成两位数的端口段码表和万位的 `BSRR` 掩码表，先输出 `00000`，再准备 `00001`。固件读取 PC4 和 PA5，按上电电平选模式并决定是否启动递增。主循环用 `__WFI()` 等待中断。
2. **计时模式。** TIM2 每隔一个设定步进产生更新中断。[`TIM2_IRQHandler()`](Core/Src/stm32f4xx_it.c) 清除 `UIF` 并调用 `Display_OnTimerUpdate()`。显示模块写出预先准备的端口值，再对低两位、中间两位和万位做十进制进位。计时路径用查表和进位准备下一次输出。
3. **外部计数模式。** 固件将 TIM3 设为外部时钟模式 1。宏选中的输入经 `TI1FP1` 或 `TI2FP2` 驱动 16 位 `CNT`，每个有效上升沿加一。[`TIM3_IRQHandler()`](Core/Src/stm32f4xx_it.c) 清除 `TIF`，`Display_OnTrigger()` 用两次 `CNT` 的差值累计触发。多次上升沿合并到一次中断时，显示直接跳到累计值。该差值能跨越一次计数器回绕，有效前提是两次读取间隔内少于 65,536 个脉冲。
4. **刷新输出。** 五位显示从左到右依次对应 `DIG5`～`DIG1`。一次刷新会写 `GPIOD->ODR`、`GPIOE->ODR` 和 `GPIOB->BSRR`，后一次写入仅更新 `PB8–PB15`。`Display_ShowNumber()` 可直接显示给定数值，短数补前导零，长数取末五位。`Display_SetRaw()` 和 `Display_ShowDigit()` 的索引 `0`～`4` 按硬件编号从 `DIG1` 到 `DIG5` 对应。
5. **暂停与模式切换。** PA5 双边沿中断按当前电平停止或重启递增源，两路定时器中断也在刷新前检查 PA5。恢复计时时从一个完整步进开始，暂停期间的外部脉冲丢弃。PC4 的边沿记录时间，SysTick 在约 10 ms 消抖后确认模式。实际切换模式时写出 `00000`、准备 `00001`，清除旧递增源的待处理事件。若 PA5 仍为低电平，`00000` 保持到恢复。

应用逻辑集中在 [`Core/Src/display.c`](Core/Src/display.c)。[`gpio.c`](Core/Src/gpio.c) 和 [`tim.c`](Core/Src/tim.c) 由 CubeMX 生成，`main.c` 和 `stm32f4xx_it.c` 通过 `USER CODE` 区接入应用模块。

## 构建、烧录与上板验证

1. 用 Keil MDK-ARM V5 打开 [`MDK-ARM/Digits.uvprojx`](MDK-ARM/Digits.uvprojx)，选择 `Digits` 目标。工程使用 ARM Compiler 5.06 update 5 和 Keil STM32F4 DFP 2.17.1。
2. 构建后，从 `MDK-ARM/Digits/Digits.hex` 烧录。需要重新配置外设时，在 STM32CubeMX 中打开 [`Digits.ioc`](Digits.ioc)，配套固件包为 STM32Cube FW_F4 V1.28.3。重新生成后确认 `Core/Src/display.c` 仍在 Keil 目标的源文件列表中。
3. 上板测计时步进时将 PC4 和 PA5 拉高，在 MCU 侧测 `PD2`（个位 C 段）。低有效配置下，个位仅在显示 `2` 时输出高电平。正脉宽为一个步进，两个相邻上升沿间隔为十个步进。当前设置下分别约为 10 µs 和 100 µs。
4. 测外部计数时将 PC4 拉低、PA5 拉高，在宏选中的输入上施加上升沿。验证逐次加一、PA5 拉低暂停、恢复继续以及切换模式清零。

`1 µs` 是可配置的定时器步进。实际显示的保持时间与抖动需要用示波器或逻辑分析仪验收。可同时测 `PD4`、`PE4`、`PB12` 在 `99999 → 00000` 时的下降沿，量出三个端口之间的切换偏差。
