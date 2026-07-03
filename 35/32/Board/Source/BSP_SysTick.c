
#include "BSP_SysTick.h"

volatile uint32_t sys_ticks=0;

void BSP_SysTick_Init(void)
{
	/*
	选择时钟源，SysTick_CLKSourceConfig()是misc.c中定义的函数	
	*/
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//选择时钟 HCLK/8
//	SysTick->LOAD=SystemCoreClock/8/100;	//加载计数初值10ms需要的systick时钟数
//	SysTick->VAL=0x00;    //清空计数器
//	
//	NVIC_SetPriority(SysTick_IRQn, 15);  //设置SysTick优先级
//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk ;	//允许SysTick中断	
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒计数	  
	
	/*
	SysTick_Config()为core_cm3.h中定义的内联函数，默认使用HCLK为时钟源，允许中断，开启计数
	*/
	SysTick_Config(720000);  //720000/72M=1/100，即10ms
}	

void BSP_SysTick_Start(void){
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒计数	
}

void BSP_SysTick_Stop(void){
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
}

void BSP_SysTick_ms(int ms){
	int val = sys_ticks, diff=0;
	do{
		diff = (sys_ticks - val) * 10; 
		diff = diff >= 0 ? diff : -diff;			
	}while(diff < ms);
}

