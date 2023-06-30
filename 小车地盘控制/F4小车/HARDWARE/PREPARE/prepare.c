#include "prepare.h"


void Initialization(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  							//��ʼ����ʱ����
	LED_Init();
	LCD_Init();
	RPI_Uart_Init(115200);					//��ʼ������1������Ϊ115200
	Encoder_Init();									//��ʼ���ĸ����ӵı�����
	PS2_Init();
	Motor_PWM_Init(999,83);
	Acquisition_Init(999,8399);			//100ms�ٶȲɼ� 
}