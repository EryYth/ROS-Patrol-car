#ifndef _ENCODER_H
#define _ENCODER_H

#include "sys.h"

void Encoder_Init_TIM3(void);
void Encoder_Init_TIM4(void);
void Encoder_Init_TIM8(void);
void Encoder_Init_TIM5(void);
void Encoder_Init(void);
int Read_Encoder(u8 TIMX);


#endif
