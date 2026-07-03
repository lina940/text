/*
***************************************************************************
*    模块：BSP_ExtInt 
*    描述：板级 ExtInt 功能模块驱动
		   Key1-PB13 Key2-PB14 Key3-PB15 Key4-PD8 按下返回低电平
*    作者：Huo
*    时间：2018.03.01
*    版本：V1.0.0
***************************************************************************
*/
#include "BSP_ExtInt.h"

/* 内部函数声明 */
static void BSP_ExtInt_NVIC_Init(void);
static void BSP_ExtInt_EXTI_Init(void);

/*
***************************************************************************
*	函 数 名: BSP_ExtInt_Init
*	功能说明: 板级 ExtInt 初始化函数
*	形    参: 无
*	返 回 值: 无
***************************************************************************
*/
void BSP_ExtInt_Init(void)
{
//	BSP_Key_GPIO_Init();
	BSP_ExtInt_EXTI_Init();
	BSP_ExtInt_NVIC_Init();	
}

/*
***************************************************************************
*	函 数 名: BSP_ExtInt_EXTI_Init
*	功能说明: 板载ExtInt EXTI初始化函数
*	形    参: 无
*	返 回 值: 无
***************************************************************************
*/
static void BSP_ExtInt_EXTI_Init(void)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能复用功能时钟	

	/* GPIOB.13 Key1 中断线以及中断初始化配置 下降沿触发 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	

	/* GPIOB.14 Key2 中断线以及中断初始化配置  下降沿触发 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;
	EXTI_Init(&EXTI_InitStructure);	 	

	/* GPIOB.15 Key3 中断线以及中断初始化配置 下降沿触发 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
	EXTI_Init(&EXTI_InitStructure);	  	

	/* GPIOD.8 Key4 中断线以及中断初始化配置 下降沿触发 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource8);
	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
	EXTI_Init(&EXTI_InitStructure);	  	
}

/*
***************************************************************************
*	函 数 名: BSP_ExtInt_NVIC_Init
*	功能说明: 板载ExtInt NVIC初始化函数
*	形    参: 无
*	返 回 值: 无
***************************************************************************
*/
static void BSP_ExtInt_NVIC_Init(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;

	/* 使能按键 Key1 Key2 Key3 所在的外部中断通道 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
	/* 抢占优先级设置，优先级分组为 4 的情况下，抢占优先级可设置范围 0-15 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	/* 子优先级设置，优先级分组为 4 的情况下，子优先级无效，取数值 0 即可 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);

	/* 使能按键 Key4 所在的外部中断通道 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				
	/* 抢占优先级设置，优先级分组为 4 的情况下，抢占优先级可设置范围 0-15 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	/* 子优先级设置，优先级分组为 4 的情况下，子优先级无效，取数值 0 即可 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
}

