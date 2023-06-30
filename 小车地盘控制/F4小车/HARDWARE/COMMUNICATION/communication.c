#include "communication.h"

Odom_Data Send_Data, Recive_Data;
uint8_t RX_Error = 0;

void RPI_Uart_Init(u32 bound)
{
	uart_init(bound);
}

void RPI_Send(void)
{
	u8 i = 0;
	
	Send_Data.Sensor_Str.Header = PROTOCOL_HEADER;
	Send_Data.Sensor_Str.End_flag = PROTOCOL_END;
	
	Speed_Inverse_Transformation(&Send_Data.Sensor_Str.X_speed, &Send_Data.Sensor_Str.Y_speed, &Send_Data.Sensor_Str.Z_speed);
	
	for(i=0; i<PROTOCL_DATA_SIZE; i++)
	{
		USART1_SendChar(Send_Data.buffer[i]);
	}
}


unsigned char Rcount = 0;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Recive_Data.buffer[Rcount] = USART_ReceiveData(USART1);
		(Recive_Data.buffer[0] == 0x67)?(Rcount++):(Rcount = 0);
		if (Rcount == PROTOCL_DATA_SIZE)	//验证数据包的长度
		{
			if(Recive_Data.Sensor_Str.Header == PROTOCOL_HEADER)	//验证数据包的头部校验信息
			{
				if(Recive_Data.Sensor_Str.End_flag == PROTOCOL_END)	//验证数据包的尾部校验信息
				{
					//接收上位机控制命令，使机器人产生相应的运动   转速处理
					Speed_Transformation(Recive_Data.Sensor_Str.X_speed, Recive_Data.Sensor_Str.Z_speed);
					RX_Error = 0;
					LED1=!LED1;	
				}
			}
			Rcount = 0;
		}
	}
} 