#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

#define LED0				((uint8_t)0x01)
#define LED1				((uint8_t)0x02)
#define LED2				((uint8_t)0x04)
#define LED3				((uint8_t)0x08)
#define LED4				((uint8_t)0x10)
#define LEDAll      ((uint8_t)0xFF)

#define RCC_LED				(RCC_APB2Periph_GPIOE)
#define GPIO_PORT_LED		GPIOE
#define GPIO_PIN_LED0		(GPIO_Pin_6)
#define GPIO_PIN_LED1		(GPIO_Pin_2)
#define GPIO_PIN_LED2		(GPIO_Pin_3)
#define GPIO_PIN_LED3		(GPIO_Pin_4)
#define GPIO_PIN_LED4		(GPIO_Pin_5)

void BSP_LED_Init(void);

void BSP_LED_On(uint8_t LEDNumber);
void BSP_LED_Off(uint8_t LEDNumber);
void BSP_LED_Toggle(uint8_t LEDNumber);

#endif
