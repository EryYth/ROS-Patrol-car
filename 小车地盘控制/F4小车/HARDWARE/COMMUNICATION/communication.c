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
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Recive_Data.buffer[Rcount] = USART_ReceiveData(USART1);
		(Recive_Data.buffer[0] == 0x67)?(Rcount++):(Rcount = 0);
		if (Rcount == PROTOCL_DATA_SIZE)	//��֤���ݰ��ĳ���
		{
			if(Recive_Data.Sensor_Str.Header == PROTOCOL_HEADER)	//��֤���ݰ���ͷ��У����Ϣ
			{
				if(Recive_Data.Sensor_Str.End_flag == PROTOCOL_END)	//��֤���ݰ���β��У����Ϣ
				{
					//������λ���������ʹ�����˲�����Ӧ���˶�   ת�ٴ���
					Speed_Transformation(Recive_Data.Sensor_Str.X_speed, Recive_Data.Sensor_Str.Z_speed);
					RX_Error = 0;
					LED1=!LED1;	
				}
			}
			Rcount = 0;
		}
	}
} 