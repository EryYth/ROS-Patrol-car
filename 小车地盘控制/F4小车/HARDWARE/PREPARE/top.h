#ifndef __TOP_H
#define __TOP_H

#include "led.h" 
#include "key.h"
#include "lcd.h"
#include "usart.h"
#include "pstwo.h"
#include "encoder.h"

#define PROTOCL_DATA_SIZE 29
#define PROTOCOL_HEADER		0X01234567
#define PROTOCOL_END		0X89

extern uint8_t RX_Error;

typedef struct __IMU_Str_
{
	short X_data;
	short Y_data;
	short Z_data;
}IMU_Str;

typedef union _Odom_Data_   
{
	unsigned char buffer[PROTOCL_DATA_SIZE];
	struct _Sensor_Str_
	{
		unsigned int Header;
		
		float X_speed;			
		float Y_speed;
		float Z_speed;
		
		IMU_Str Link_Accelerometer;		//º”ÀŸº∆
		IMU_Str Link_Gyroscope;		//Õ”¬›“«
		
		unsigned char End_flag;
	}Sensor_Str;
}Odom_Data;

extern Odom_Data Send_Data,Recive_Data;

#endif
