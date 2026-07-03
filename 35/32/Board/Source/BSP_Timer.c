/**********************************************************
*    模块：BSP_Timer
*    功能：板级 Timer 驱动模块
*          使用 TIM2 产生 1ms 中断
**********************************************************/
#include "BSP_Timer.h"

static void BSP_Timer_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void BSP_Timer_TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 72MHz/72 = 1MHz -> 1us计数, 1000us = 1ms中断 */
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void BSP_Timer_Init(void)
{
    BSP_Timer_NVIC_Init();
    BSP_Timer_TIM2_Init();
}

/* 声明外部全局变量 */
extern volatile uint32_t ms_tick;
extern volatile uint8_t  one_second_flag;
extern volatile uint32_t alarm_red_led_timer;
extern volatile uint16_t g_year;
extern volatile uint8_t  g_month, g_day;
extern volatile uint8_t  g_hour, g_minute, g_second;
extern volatile uint8_t  g_weekday;

/**********************************************************
* 函数名：TIM2_IRQHandler
* 功能  ：TIM2 中断服务函数，每 1ms 自动触发一次
*         - 闹钟红灯30秒倒计时熄灭
*         - 时钟走时更新（1秒为单位）
**********************************************************/
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        /* 闹钟红灯计时：每1ms减1，减到0熄灭 */
        if (alarm_red_led_timer > 0)
        {
            alarm_red_led_timer--;
            if (alarm_red_led_timer == 0)
            {
                GPIO_SetBits(GPIOE, GPIO_Pin_2);
            }
        }

        /* 1ms累计，满1000ms = 1秒 */
        ms_tick++;
        if (ms_tick >= 1000)
        {
            ms_tick = 0;
            one_second_flag = 1;

            /* 走时更新 */
            g_second++;
            if (g_second >= 60) {
                g_second = 0;
                g_minute++;
                if (g_minute >= 60) {
                    g_minute = 0;
                    g_hour++;
                    if (g_hour >= 24) {
                        g_hour = 0;
                        g_day++;
                        g_weekday = (g_weekday % 7) + 1;
                    }
                }
            }
        }
    }
}
