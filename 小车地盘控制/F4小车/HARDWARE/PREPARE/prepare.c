#include "prepare.h"


void Initialization(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  							//初始化延时函数
	LED_Init();
	LCD_Init();
	RPI_Uart_Init(115200);					//初始化串口1波特率为115200
	Encoder_Init();									//初始化四个轮子的编码器
	PS2_Init();
	Motor_PWM_Init(999,83);
	Acquisition_Init(999,8399);			//100ms速度采集 
}