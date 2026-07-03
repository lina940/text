/**********************************************************
* 模块：SmartClock
* 功能：智能电子时钟
* (1) TIM2 秒定时，实现年-月-日 时:分:秒 星期走时
* (2) OLED 显示时钟
* (3) 按键设置年-月-日 时:分
* (4) 增加串口接收与指令解析功能
**********************************************************/
#include "stm32f10x.h"
#include "BSP_Delay.h"
#include "BSP_LED.h"
#include "BSP_OLED.h"
#include "BSP_Timer.h"
#include "BSP_Key.h"
#include "BSP_UART.h"
#include "BSP_ADC.h"
#include <stdio.h>
#include <string.h>

/*==================================================================
 * 时间及报警结构体定义
 *================================================================*/
typedef struct {
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Day;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
    uint8_t  Weekday;
} Clock_TypeDef;

typedef struct {
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Enable;
    uint8_t Triggered;
} Alarm_TypeDef;

/*==================================================================
 * 全局变量
 *================================================================*/
volatile uint32_t ms_tick = 0;
volatile uint8_t  one_second_flag = 0;
volatile uint32_t alarm_red_led_timer = 0;

/* 温度相关 (任务6/7: 电位器模拟温度传感器) */
#define TEMP_THRESHOLD   3000   /* ADC超阈值报警值 (0-4095) */
volatile uint16_t g_temp_adc = 0;       /* 当前ADC原始值 */
volatile uint8_t  g_temp_over_threshold = 0;  /* 超阈值标志 */

volatile uint16_t g_year    = 2026;
volatile uint8_t  g_month   = 5;
volatile uint8_t  g_day     = 18;
volatile uint8_t  g_hour    = 14;
volatile uint8_t  g_minute  = 54;
volatile uint8_t  g_second  = 0;
volatile uint8_t  g_weekday = 1;

volatile Alarm_TypeDef g_alarms[3] = {0};

/* 按键设置状态 */
static uint8_t key_set_mode = 0;  /* 0:正常, 1:年, 2:月, 3:日, 4:时, 5:分 */
static uint8_t g_disp_update = 0; /* 需要刷新显示 */

/* 串口接收缓存全局变量 (注意：rx_buffer去掉了volatile以防止sscanf报错) */
char rx_buffer[30];
volatile uint8_t rx_index = 0;
volatile uint8_t rx_complete = 0;

/*==================================================================
 * 串口1中断服务函数 (支持 \r\n 连续发送，防止缓冲区错乱)
 * 注：时钟走时在 BSP_Timer.c 的 TIM2_IRQHandler 中完成
 *================================================================*/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        char data = (char)USART_ReceiveData(USART1);

        /* 上条指令还没处理完，丢弃新数据（DR已读取，RXNE已清除） */
        if (rx_complete) {
            return;
        }

        /* 遇到回车(\r)或换行(\n)认为一条指令结束 */
        if(data == '\n' || data == '\r') {
            if(rx_index > 0) {
                rx_buffer[rx_index] = '\0'; /* 添加字符串结束符 */
                rx_complete = 1;            /* 标记接收完成 */
                /* 注意：不重置 rx_index，留待 Process_Serial_Command 中拷贝完再重置 */
            }
        } else {
            /* 过滤空字符 */
            if (data != '\0') {
                rx_buffer[rx_index++] = data;
                if(rx_index >= 30) rx_index = 0; /* 防止数组越界 */
            }
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/*==================================================================
 * 解析串口指令
 *================================================================*/
static void Process_Serial_Command(void)
{
    if(rx_complete) {
        int alarm_id = 0, h = 0, m = 0;
        char local_buf[30];

        /* 关中断安全拷贝缓冲区，拷贝完立刻清空状态，允许继续接收下一条 */
        __disable_irq();
        strncpy(local_buf, rx_buffer, 29);
        local_buf[29] = '\0';
        rx_index = 0;
        rx_complete = 0;
        __enable_irq();

        if(sscanf(local_buf, "A%d:%d:%d", &alarm_id, &h, &m) == 3) {
            if(alarm_id >= 0 && alarm_id < 3 && h < 24 && m < 60) {
                g_alarms[alarm_id].Hour = h;
                g_alarms[alarm_id].Minute = m;
                g_alarms[alarm_id].Enable = 1;    /* 开启该闹钟 */
                g_alarms[alarm_id].Triggered = 0; /* 重置触发状态 */
                g_disp_update = 1;                /* 刷新屏幕显示 */

                printf("Set Alarm %d to %02d:%02d Success!\r\n", alarm_id, h, m);
            } else {
                printf("Error: Time out of range! (H<24, M<60)\r\n");
            }
        } else {
            printf("Error: Format incorrect! Received: %s. Use A0:08:30\r\n", local_buf);
        }
    }
}

/*==================================================================
 * 按键处理 (Key1切换设置项, Key2+, Key3-)
 * Key1-PB13, Key2-PB14, Key3-PB15, Key4-PD8
 *================================================================*/
static void Process_Key(void)
{
    uint8_t key_val = BSP_Key_Scan(Key1 | Key2 | Key3);

    if (key_val & Key1) {
        BSP_Delay_ms(50);  /* 消抖 */
        if (BSP_Key_Scan(Key1) & Key1) {
            key_set_mode++;
            if (key_set_mode > 5) key_set_mode = 0;
            g_disp_update = 1;
        }
    }

    if (key_val & Key2) {
        BSP_Delay_ms(50);
        if (BSP_Key_Scan(Key2) & Key2) {
            switch (key_set_mode) {
                case 1: if (++g_year > 2099) g_year = 2020; break;
                case 2: if (++g_month > 12) g_month = 1; break;
                case 3: if (++g_day > 31) g_day = 1; break;
                case 4: if (++g_hour > 23) g_hour = 0; break;
                case 5: if (++g_minute > 59) g_minute = 0; break;
                default: break;
            }
            g_disp_update = 1;
        }
    }

    if (key_val & Key3) {
        BSP_Delay_ms(50);
        if (BSP_Key_Scan(Key3) & Key3) {
            switch (key_set_mode) {
                case 1: if (--g_year < 2020) g_year = 2099; break;
                case 2: if (--g_month < 1) g_month = 12; break;
                case 3: if (--g_day < 1) g_day = 31; break;
                case 4: if (--g_hour > 23) g_hour = 23; break;
                case 5: if (--g_minute > 59) g_minute = 59; break;
                default: break;
            }
            g_disp_update = 1;
        }
    }
}

/*==================================================================
 * 闹钟检查
 *================================================================*/
static void Check_Alarm(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        if (g_alarms[i].Enable && !g_alarms[i].Triggered &&
            g_hour == g_alarms[i].Hour && g_minute == g_alarms[i].Minute) {

            GPIO_ResetBits(GPIOE, GPIO_Pin_2);
            alarm_red_led_timer = 30000;
            g_alarms[i].Triggered = 1;

            printf("Alarm %d triggered! %02d:%02d\r\n", i, g_alarms[i].Hour, g_alarms[i].Minute);
        }
        if (g_alarms[i].Triggered && g_minute != g_alarms[i].Minute) {
            g_alarms[i].Triggered = 0;
        }
    }
}

/*==================================================================
 * OLED 显示
 *================================================================*/
static void Display_Time(void)
{
    char buf[22];
    int16_t temp_int;

    /* 第1行: 年-月-日 星期 */
    sprintf(buf, "%04d-%02d-%02d W%d", g_year, g_month, g_day, g_weekday);

    if (key_set_mode == 0)
        BSP_OLED_ShowString(0, 0, (uint8_t*)buf);
    else
        BSP_OLED_ShowString(0, 0, (uint8_t*)buf);

    /* 第3行: 时:分:秒 */
    sprintf(buf, "  %02d:%02d:%02d", g_hour, g_minute, g_second);
    BSP_OLED_ShowString(0, 2, (uint8_t*)buf);

    /* 第5行: 闹钟状态或设置模式 */
    if (key_set_mode > 0) {
        sprintf(buf, "Set: %d", key_set_mode);
        BSP_OLED_ShowString(0, 4, (uint8_t*)buf);
    } else {
        sprintf(buf, "A0:%02d:%02d A1:%02d:%02d",
                g_alarms[0].Hour, g_alarms[0].Minute,
                g_alarms[1].Hour, g_alarms[1].Minute);
        BSP_OLED_ShowString(0, 4, (uint8_t*)buf);
    }

    /* 第7行: 温度(任务6) + 闹钟3(任务5) — ADC 0~4095 映射 0.0~100.0°C */
    temp_int = (int32_t)g_temp_adc * 100 / 4095;
    sprintf(buf, "T:%3d.%d A2:%02d:%02d", temp_int / 10, temp_int % 10,
            g_alarms[2].Hour, g_alarms[2].Minute);
    BSP_OLED_ShowString(0, 6, (uint8_t*)buf);
}

/*==================================================================
 * 主函数
 *================================================================*/
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    BSP_LED_Init();
    BSP_Delay_Init();
    BSP_OLED_Init();
    BSP_Timer_Init();     /* TIM2 1ms中断 */
    BSP_Key_Init();
    BSP_USART1_Init(115200);  /* PC通信设置闹钟 */
    BSP_ADC_Init();           /* ADC温度采集初始化 (任务6) */

    GPIO_SetBits(GPIOE, GPIO_Pin_2);  /* 关闭红灯(PE2) */

    BSP_OLED_Clear();
    BSP_OLED_ShowString(0, 0, (uint8_t*)"SmartClock Init");
    BSP_Delay_ms(200);
    BSP_OLED_Clear();

    printf("SmartClock started.\r\n");

    while (1)
    {
        /* --- 新增：处理串口命令 --- */
        Process_Serial_Command();

        /* 每秒更新 */
        if (one_second_flag) {
            one_second_flag = 0;

            Display_Time();
            BSP_LED_Toggle(LED0);  /* LED0闪烁指示运行 */

            /* 闹钟检查 (任务5) */
            Check_Alarm();

            /* 任务6: 读取ADC模拟温度 */
            g_temp_adc = BSP_ADC_Read();

            /* 任务7: 温度超阈值 -> 红灯报警 + 串口通知PC */
            if (g_temp_adc > TEMP_THRESHOLD) {
                GPIO_ResetBits(GPIOE, GPIO_Pin_2);  /* 点亮红灯 */
                if (!g_temp_over_threshold) {
                    g_temp_over_threshold = 1;
                    printf("WARNING: Temp over threshold! ADC=%d\r\n", g_temp_adc);
                }
            } else {
                if (g_temp_over_threshold) {
                    g_temp_over_threshold = 0;
                    if (alarm_red_led_timer == 0) {
                        GPIO_SetBits(GPIOE, GPIO_Pin_2);  /* 关灯 */
                    }
                    printf("Temperature back to normal. ADC=%d\r\n", g_temp_adc);
                }
            }
        }

        /* 按键扫描 */
        Process_Key();

        /* 进入设置模式时强制刷新 */
        if (g_disp_update) {
            g_disp_update = 0;
            Display_Time();
        }
    }
}
