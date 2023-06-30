#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "top.h"
#include "motor.h"

void RPI_Uart_Init(u32 bound);
void RPI_Send(void);

#endif