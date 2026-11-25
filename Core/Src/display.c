/**
  ******************************************************************************
  * @file    display.c
  * @brief   5 位数码管并行驱动实现（直接寄存器访问，热路径零 HAL 开销）
  *
  * 关键实现思路：
  *   1) 一个数码管恰好占一个端口的连续 8 位，所以“写一个管 = 写一个字节”。
  *   2) 单管更新用 BSRR：一次写同时“置高/置低”指定 8 位，原子且不影响同端口
  *      另一个管（因为两个管共用 PORTD/PORTE）。
  *   3) 整组更新时，PD、PE 各用一次 ODR 写更新两管，PB 用一次 BSRR 写更新第 5 管。
  *      三个端口依次写入，PB0~PB7 的输出锁存位保持原值。
  ******************************************************************************
  */
#include "display.h"
#include "main.h"      /* 提供 GPIO_TypeDef / GPIOD/E/B / BSRR / ODR 定义 */

#if DISPLAY_TRIGGER_INPUT == 1u
#define DISPLAY_TIM3_TRIGGER TIM_TS_TI1FP1
#elif DISPLAY_TRIGGER_INPUT == 2u
#define DISPLAY_TIM3_TRIGGER TIM_TS_TI2FP2
#else
#error DISPLAY_TRIGGER_INPUT must be 1 or 2
#endif

#define DISPLAY_MODE_DEBOUNCE_MS 10u

/* 下一帧的十进制分组与端口值。低两位在 PD，中间两位在 PE，万位在 PB。 */
static uint16_t pair_segments[100];
static uint32_t highest_segment_bsrr[10];
static uint32_t next_low_pair;
static uint32_t next_middle_pair;
static uint32_t next_high_digit;
static struct {
    uint32_t pd_odr;
    uint32_t pe_odr;
    uint32_t pb_bsrr;
} next_frame;

static volatile uint16_t previous_trigger_count;
static volatile uint8_t display_ready;
static volatile uint8_t timing_mode;
static volatile uint8_t mode_pending;
static volatile uint32_t last_mode_edge_ms;

/* ----------------------------------------------------------------------------
 * 七段码表：bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g, bit7=dp。
 * 表中 bit=1 表示“点亮”该段（当 SEG_ACTIVE_LOW=0 时，即引脚输出高电平）。
 * 下标 0~15 依次对应字符 0~9、A、b、C、d、E、F。
 * -------------------------------------------------------------------------- */
static const uint8_t seg_code[16] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, /* 0 1 2 3 4 5 6 7 */
    0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71  /* 8 9 A b C d E F */
};

/* 查表得到段码；若为“低电平点亮”，整体取反（1<->0），得到最终引脚电平字节 */
static uint8_t seg_encode(uint8_t v)
{
    uint8_t s = seg_code[v & 0x0Fu];   /* 仅取低 4 位，越界字符自动回绕 */
#if SEG_ACTIVE_LOW
    s = (uint8_t)(~s);                 /* 共阳直灌：点亮位由 1 变 0 */
#endif
    return s;
}

/* ----------------------------------------------------------------------------
 * BSRR 原子写说明：
 *   GPIOx->BSRR 的 bit[15:0]  写 1 = 对应引脚“置高”
 *   GPIOx->BSRR 的 bit[31:16] 写 1 = 对应引脚“置低”
 *   一次寄存器写入即可同时置高和置低选中的引脚，无需读-改-写，
 *   且只作用于掩码选中的引脚，因此不会误伤同端口另一个数码管。
 * -------------------------------------------------------------------------- */

/* 写“低字节”里的一个管（管1: PD0-7 / 管3: PE0-7） */
__STATIC_FORCEINLINE void seg_write_low(GPIO_TypeDef *port, uint8_t seg)
{
    uint32_t inv = (uint8_t)(~seg);                    /* 需要置低的位 */
    port->BSRR = (uint32_t)seg | ((uint32_t)inv << 16);/* 低16位置高 + 高16位置低 */
}

/* 生成高字节段脚的 BSRR 掩码，低字节对应的置位和复位位均为 0。 */
__STATIC_FORCEINLINE uint32_t seg_high_bsrr(uint8_t seg)
{
    uint32_t inv = (uint8_t)(~seg);
    return ((uint32_t)seg << 8) | (inv << 24);
}

/* 写“高字节”里的一个管（管2: PD8-15 / 管4: PE8-15 / 管5: PB8-15） */
__STATIC_FORCEINLINE void seg_write_high(GPIO_TypeDef *port, uint8_t seg)
{
    port->BSRR = seg_high_bsrr(seg);
}

/* 按数码管编号 idx 选择端口和高/低字节 */
void Display_SetRaw(uint8_t idx, uint8_t seg)
{
    switch (idx) {
        case 0: seg_write_low (GPIOD, seg); break;  /* 管1: PD0-7  */
        case 1: seg_write_high(GPIOD, seg); break;  /* 管2: PD8-15 */
        case 2: seg_write_low (GPIOE, seg); break;  /* 管3: PE0-7  */
        case 3: seg_write_high(GPIOE, seg); break;  /* 管4: PE8-15 */
        case 4: seg_write_high(GPIOB, seg); break;  /* 管5: PB8-15 */
        default: break;                             /* 非法编号：忽略 */
    }
}

/* 单管显示字符：查表 + 极性处理，再写对应管 */
void Display_ShowDigit(uint8_t idx, uint8_t value)
{
    Display_SetRaw(idx, seg_encode(value));
}

/* 实物从左到右为 DIG5~DIG1，因此万位到个位按此顺序分配段码。 */
void Display_ShowNumber(uint32_t n)
{
    uint8_t ones = seg_encode((uint8_t)(n % 10u));
    uint8_t tens = seg_encode((uint8_t)((n / 10u) % 10u));
    uint8_t hundreds = seg_encode((uint8_t)((n / 100u) % 10u));
    uint8_t thousands = seg_encode((uint8_t)((n / 1000u) % 10u));
    uint8_t ten_thousands = seg_encode((uint8_t)((n / 10000u) % 10u));

    GPIOD->ODR = ((uint32_t)tens << 8) | ones;         /* DIG2 + DIG1 */
    GPIOE->ODR = ((uint32_t)thousands << 8) | hundreds; /* DIG4 + DIG3 */
    seg_write_high(GPIOB, ten_thousands);             /* DIG5 */
}

/* 全灭：根据极性把熄灭电平写到三个端口。
 * 高电平点亮 -> 熄灭=0x00；低电平点亮 -> 熄灭=0xFF */
void Display_Blank(void)
{
    uint8_t off = SEG_ACTIVE_LOW ? 0xFFu : 0x00u; /* 单个数码管的熄灭电平 */
    uint32_t off_pair = ((uint32_t)off << 8) | off; /* 两个数码管的熄灭电平 */

    GPIOD->ODR = off_pair;                        /* 管1+管2 */
    GPIOE->ODR = off_pair;                        /* 管3+管4 */
    seg_write_high(GPIOB, off);                  /* 管5 */
}

/* 输出已准备好的帧，然后用进位和查表准备下一帧。 */
__STATIC_FORCEINLINE void display_advance(void)
{
    GPIOD->ODR = next_frame.pd_odr;
    GPIOE->ODR = next_frame.pe_odr;
    GPIOB->BSRR = next_frame.pb_bsrr;

    if (++next_low_pair == 100u) {
        next_low_pair = 0;
        if (++next_middle_pair == 100u) {
            next_middle_pair = 0;
            if (++next_high_digit == 10u) {
                next_high_digit = 0;
            }
            next_frame.pb_bsrr = highest_segment_bsrr[next_high_digit];
        }
        next_frame.pe_odr = pair_segments[next_middle_pair];
    }
    next_frame.pd_odr = pair_segments[next_low_pair];
}

/* 立即显示 00000，并为下一次递增准备 00001。 */
static void display_reset_number(void)
{
    next_low_pair = 0;
    next_middle_pair = 0;
    next_high_digit = 0;
    next_frame.pd_odr = pair_segments[0];
    next_frame.pe_odr = pair_segments[0];
    next_frame.pb_bsrr = highest_segment_bsrr[0];
    display_advance();
}

/* 停止两个递增源。清除尚未处理的事件，保留屏幕值和下一帧。 */
static void display_stop_sources(void)
{
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM2->DIER = 0;
    TIM3->DIER = 0;
    TIM2->SR = 0;
    TIM3->SR = 0;
    __DSB();
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_ClearPendingIRQ(TIM3_IRQn);
}

/* PA5 高电平允许运行，低电平暂停。 */
__STATIC_FORCEINLINE uint32_t display_run_requested(void)
{
    return (TRIG_SW_GPIO_Port->IDR & TRIG_SW_Pin) != 0u;
}

/* 调用方屏蔽中断。恢复时重新计满一周期，暂停期间的外部脉冲不补计。 */
static void display_restart_source(void)
{
    display_stop_sources();

    if (display_run_requested()) {
        if (timing_mode != 0u) {
            TIM2->CNT = 0;
            TIM2->DIER = TIM_DIER_UIE;
            TIM2->CR1 |= TIM_CR1_CEN;
        } else {
            TIM3->CNT = 0;
            previous_trigger_count = 0;
            TIM3->DIER = TIM_DIER_TIE;
            TIM3->CR1 |= TIM_CR1_CEN;
        }
    }
}

void Display_Start(void)
{
    uint32_t timer_clock_hz = HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
        timer_clock_hz *= 2u;
    }
    uint64_t step_ticks = (uint64_t)(timer_clock_hz / 1000000u) * DISPLAY_STEP_US;
    if ((timer_clock_hz % 1000000u) != 0u || step_ticks < 2u || step_ticks > (uint64_t)UINT32_MAX + 1u) {
        Error_Handler();
    }

    /* 只在启动时生成段码，两位十进制数可通过一次查表得到端口值。 */
    for (uint32_t tens = 0; tens < 10u; ++tens) {
        uint8_t tens_segment = seg_encode((uint8_t)tens);
        highest_segment_bsrr[tens] = seg_high_bsrr(tens_segment);
        for (uint32_t ones = 0; ones < 10u; ++ones) {
            pair_segments[tens * 10u + ones] = (uint16_t)(((uint16_t)tens_segment << 8) | seg_encode((uint8_t)ones));
        }
    }

    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    display_stop_sources();
    TIM2->PSC = 0;
    TIM2->ARR = (uint32_t)(step_ticks - 1u);
    TIM2->EGR = TIM_EGR_UG;

    /* CubeMX 已设置 TI1/TI2 上升沿。选择一路外部时钟，硬件每沿对 CNT 加一。 */
    TIM3->SMCR = (TIM3->SMCR & ~(TIM_SMCR_SMS | TIM_SMCR_TS | TIM_SMCR_ECE)) |
                 TIM_SLAVEMODE_EXTERNAL1 | DISPLAY_TIM3_TRIGGER;

    display_reset_number();

    mode_pending = 0;
    timing_mode = (Controll_Mode_GPIO_Port->IDR & Controll_Mode_Pin) != 0u;
    display_ready = 1;
    display_restart_source();
    __set_PRIMASK(primask);
}

void Display_OnTimerUpdate(void)
{
    /* TIM2 优先级高于暂停 EXTI，先读引脚以避免继续输出一帧。 */
    if (!display_run_requested()) {
        display_stop_sources();
        return;
    }
    display_advance();
}

void Display_OnTrigger(void)
{
    if (!display_run_requested()) {
        display_stop_sources();
        return;
    }

    /* TIF 可能合并多个上升沿。读取 CNT 差值以累计所有未处理的脉冲。 */
    uint16_t count = (uint16_t)TIM3->CNT;
    uint16_t elapsed = (uint16_t)(count - previous_trigger_count);
    previous_trigger_count = count;
    if (elapsed == 0u) {
        return;
    }
    if (elapsed > 1u) {
        uint32_t number = next_high_digit * 10000u + next_middle_pair * 100u + next_low_pair + elapsed - 1u;
        if (number >= 100000u) {
            number -= 100000u;
        }
        next_high_digit = number / 10000u;
        next_middle_pair = (number / 100u) % 100u;
        next_low_pair = number % 100u;
        next_frame.pd_odr = pair_segments[next_low_pair];
        next_frame.pe_odr = pair_segments[next_middle_pair];
        next_frame.pb_bsrr = highest_segment_bsrr[next_high_digit];
    }
    display_advance();
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (display_ready == 0u) {
        return;
    }

    if (pin == TRIG_SW_Pin) {
        uint32_t primask = __get_PRIMASK();
        __disable_irq();
        display_restart_source();
        __set_PRIMASK(primask);
    } else if (pin == Controll_Mode_Pin) {
        last_mode_edge_ms = HAL_GetTick();
        mode_pending = 1;
    }
}

void Display_CheckMode(void)
{
    if (mode_pending == 0u || (HAL_GetTick() - last_mode_edge_ms) < DISPLAY_MODE_DEBOUNCE_MS) {
        return;
    }

    /* SysTick 优先级最低。屏蔽中断后重查边沿，避免提交机械抖动期间的模式。 */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    if ((HAL_GetTick() - last_mode_edge_ms) >= DISPLAY_MODE_DEBOUNCE_MS && (EXTI->PR & Controll_Mode_Pin) == 0u) {
        uint8_t requested_mode = (Controll_Mode_GPIO_Port->IDR & Controll_Mode_Pin) != 0u;
        mode_pending = 0;
        if (requested_mode != timing_mode) {
            timing_mode = requested_mode;
            display_reset_number();
            display_restart_source();
        }
    }
    __set_PRIMASK(primask);
}
