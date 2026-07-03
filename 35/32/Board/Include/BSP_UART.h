#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "stm32f10x.h"

void BSP_USART1_Init(uint32_t BaudRate);
void USART1_SendByte(uint8_t data);
void USART1_SendString(char *str);

#endif
