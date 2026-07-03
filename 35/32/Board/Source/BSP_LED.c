/**********************************************************
*    模块：BSP_LED 
*    描述：板级 LED 功能模块驱动
		   LED0-PE6 LED1-PE2 LED2-PE3 LED3-PE4 LED4-PE5 低电平点亮
*    作者：
*    时间：
*    版本：V1.0.0
**********************************************************/
#include "BSP_LED.h"

/* 内部函数声明 */
static void BSP_LED_GPIO_Init(void);

/*******************************************************
*	功能说明: 板级 LED 初始化函数
*******************************************************/
void BSP_LED_Init(void)
{
	BSP_LED_GPIO_Init();
}

/*******************************************************
*	功能说明: 板级 LED GPIO 内部初始化函数
*******************************************************/
static void BSP_LED_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_LED, ENABLE);//使能对应时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED0 | GPIO_PIN_LED1 | \
	                 GPIO_PIN_LED2 | GPIO_PIN_LED3 | GPIO_PIN_LED4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 //推挽输出
	GPIO_Init(GPIO_PORT_LED, &GPIO_InitStructure);
	
	BSP_LED_Off(LEDAll);
}

/*******************************************************
*	功能说明: 板级 LED点亮 功能函数
*	形    参: LEDNumber
*******************************************************/
void BSP_LED_On(uint8_t LEDNumber)
{
	if (LEDNumber & LED0)
		GPIO_ResetBits(GPIO_PORT_LED, GPIO_PIN_LED0);
	if (LEDNumber & LED1)
		GPIO_ResetBits(GPIO_PORT_LED, GPIO_PIN_LED1);
	if (LEDNumber & LED2)
		GPIO_ResetBits(GPIO_PORT_LED, GPIO_PIN_LED2);
	if (LEDNumber & LED3)
		GPIO_ResetBits(GPIO_PORT_LED, GPIO_PIN_LED3);
	if (LEDNumber & LED4)
		GPIO_ResetBits(GPIO_PORT_LED, GPIO_PIN_LED4);
}

/*******************************************************
*	功能说明: 板级 LED熄灭 功能函数
*	形    参: LEDNumber
*******************************************************/
void BSP_LED_Off(uint8_t LEDNumber)
{
	if (LEDNumber & LED0)
		GPIO_SetBits(GPIO_PORT_LED, GPIO_PIN_LED0);		
	if (LEDNumber & LED1)
		GPIO_SetBits(GPIO_PORT_LED, GPIO_PIN_LED1);
	if (LEDNumber & LED2)
		GPIO_SetBits(GPIO_PORT_LED, GPIO_PIN_LED2);
	if (LEDNumber & LED3)
		GPIO_SetBits(GPIO_PORT_LED, GPIO_PIN_LED3);
	if (LEDNumber & LED4)
		GPIO_SetBits(GPIO_PORT_LED, GPIO_PIN_LED4);
}

/***************************************************
*	功能说明: 板级 LED 状态翻转功能函数
*	形    参: LEDNumber
****************************************************/
void BSP_LED_Toggle(uint8_t LEDNumber)
{
	uint8_t val;
	if (LEDNumber & LED0){
		val = GPIO_ReadOutputDataBit(GPIO_PORT_LED, GPIO_PIN_LED0);
		GPIO_WriteBit(GPIO_PORT_LED, GPIO_PIN_LED0, (BitAction)(!val)); 		
	}
	if (LEDNumber & LED1){
		val = GPIO_ReadOutputDataBit(GPIO_PORT_LED, GPIO_PIN_LED1);
		GPIO_WriteBit(GPIO_PORT_LED, GPIO_PIN_LED1, (BitAction)(!val)); 		
	}
	if (LEDNumber & LED2){
		val = GPIO_ReadOutputDataBit(GPIO_PORT_LED, GPIO_PIN_LED2);
		GPIO_WriteBit(GPIO_PORT_LED, GPIO_PIN_LED2, (BitAction)(!val)); 		
	}
	if (LEDNumber & LED3){
		val = GPIO_ReadOutputDataBit(GPIO_PORT_LED, GPIO_PIN_LED3);
		GPIO_WriteBit(GPIO_PORT_LED, GPIO_PIN_LED3, (BitAction)(!val)); 		
	}
	if (LEDNumber & LED4){
		val = GPIO_ReadOutputDataBit(GPIO_PORT_LED, GPIO_PIN_LED4);
		GPIO_WriteBit(GPIO_PORT_LED, GPIO_PIN_LED4, (BitAction)(!val)); 		
	}
}
