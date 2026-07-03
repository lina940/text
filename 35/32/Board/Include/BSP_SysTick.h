#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "stm32f10x.h"
	 
void BSP_SysTick_Init(void);
void BSP_SysTick_ms(int ms);

#endif
