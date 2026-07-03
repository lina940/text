/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "BSP_LED.h"
#include "BSP_UART.h"
#include <stdio.h>

/* 外部变量声明 */
extern volatile uint32_t ms_tick;
extern volatile uint8_t one_second_flag;
extern volatile uint32_t alarm_red_led_timer;

/* 时钟全局变量 */
extern volatile uint16_t g_year;
extern volatile uint8_t g_month, g_day;
extern volatile uint8_t g_hour, g_minute, g_second;
extern volatile uint8_t g_weekday;

/* 串口接收全局变量（定义在 main.c） */
extern volatile char     rx_buffer[30];
extern volatile uint8_t  rx_index;
extern volatile uint8_t  rx_complete;

/* 闹钟 */
typedef struct {
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Enable;
    uint8_t Triggered;
} Alarm_TypeDef;
extern volatile Alarm_TypeDef g_alarms[3];


/* Cortex-M3 异常处理 */
void NMI_Handler(void) {}
void HardFault_Handler(void) { while(1); }
void MemManage_Handler(void) { while(1); }
void BusFault_Handler(void) { while(1); }
void UsageFault_Handler(void) { while(1); }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) {}


void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if (EXTI_GetITStatus(EXTI_Line15) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}



