#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

void uart_init(u32 bound);
void USART1_SendChar(unsigned char data);
#endif


