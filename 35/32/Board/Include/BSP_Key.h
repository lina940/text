#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"

#define Key1				((uint8_t)0x01)
#define Key2				((uint8_t)0x02)
#define Key3				((uint8_t)0x04)
#define Key4				((uint8_t)0x08)
#define KeyAll			(Key1|Key2|Key3|Key4)

#define RCC_Key1		(RCC_APB2Periph_GPIOB)
#define GPIO_PORT_Key1		GPIOB
#define GPIO_PIN_Key1		(GPIO_Pin_13)
#define RCC_Key2			(RCC_APB2Periph_GPIOB)
#define GPIO_PORT_Key2  	GPIOB
#define GPIO_PIN_Key2		(GPIO_Pin_14)
#define RCC_Key3			(RCC_APB2Periph_GPIOB)
#define GPIO_PORT_Key3		GPIOB
#define GPIO_PIN_Key3		(GPIO_Pin_15)
#define RCC_Key4			(RCC_APB2Periph_GPIOD)
#define GPIO_PORT_Key4  	GPIOD
#define GPIO_PIN_Key4		(GPIO_Pin_8)

void BSP_Key_Init(void);

uint8_t BSP_Key_Scan(uint8_t KeyCount);

#endif
