#include "timer.h"



void Acquisition_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 
	TIM_Cmd(TIM7,ENABLE); 
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 
	{
		Get_Speed();
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  
}