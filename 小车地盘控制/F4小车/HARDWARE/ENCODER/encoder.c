#include "encoder.h"

void Encoder_Init(void)
{
	Encoder_Init_TIM3();	//轮子1
	Encoder_Init_TIM4();	//轮子2
	Encoder_Init_TIM5();	//轮子3
	Encoder_Init_TIM8();	//轮子4
	TIM3->CNT = 0x7fff;
	TIM4->CNT = 0x7fff;
	TIM5->CNT = 0x7fff;
	TIM8->CNT = 0x7fff;
	
	
}

void Encoder_Init_TIM3(void)
{
  	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	   
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           
    GPIO_Init(GPIOA, &GPIO_InitStructure);					        

    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                      
    TIM_TimeBaseStructure.TIM_Period = 65534;                         
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
		
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//???????3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
		
    
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);                           
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    

	  TIM_SetCounter(TIM3,0);//速度模式初始化为0
    TIM_Cmd(TIM3, ENABLE); 
}
/**************************************************************************
函数功能：把TIM4初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM4(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef   TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);            
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
	  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;	
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           
    GPIO_Init(GPIOD, &GPIO_InitStructure);					        


    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                      
    TIM_TimeBaseStructure.TIM_Period = 65534;                         
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
		
    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);                           
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
		

    TIM_SetCounter(TIM4,0);//速度模式初始化为0
    TIM_Cmd(TIM4, ENABLE); 
}



void Encoder_Init_TIM5(void)
{
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);            
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
	
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	

    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                      
    TIM_TimeBaseStructure.TIM_Period = 65534;                         
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
   
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
		

    TIM_ClearFlag(TIM5, TIM_FLAG_Update);                           
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	  TIM_SetCounter(TIM5,0);//速度模式初始化为0
    TIM_Cmd(TIM5, ENABLE);
}

void Encoder_Init_TIM8(void)
{
  	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);            
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	   
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           
    GPIO_Init(GPIOC, &GPIO_InitStructure);					        

    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                      
    TIM_TimeBaseStructure.TIM_Period = 65534;                         
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    
		
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//???????3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);
		
    
    TIM_ClearFlag(TIM8, TIM_FLAG_Update);                           
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
    

	  TIM_SetCounter(TIM8,0);//速度模式初始化为0	 
    TIM_Cmd(TIM8, ENABLE); 
}


/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
   short int Encoder_TIM;    
   switch(TIMX)
	 {
	   case 1:   Encoder_TIM= TIM3 -> CNT - 0x7fff; TIM3 -> CNT=0x7fff;	break;
		 case 2:   Encoder_TIM= TIM4 -> CNT - 0x7fff; TIM4 -> CNT=0x7fff;	break;	
		 case 3:   Encoder_TIM= TIM5 -> CNT - 0x7fff; TIM5 -> CNT=0x7fff;	break;
		 case 4:   Encoder_TIM= TIM8 -> CNT - 0x7fff; TIM8 -> CNT=0x7fff;	break;
		 default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
	 
}


 


