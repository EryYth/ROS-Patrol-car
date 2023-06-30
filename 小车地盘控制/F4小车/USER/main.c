#include "prepare.h"




int main(void)
{
	Initialization();
	
  while(1)
	{
		Run_Spend();
		RPI_Send();
		Remote_control();
		Get_Pwm_Set();
//		LCD_ShowNum(160,50,wheel1_pid.Current_value,6,16);
//		LCD_ShowNum(160,70,wheel2_pid.Current_value,6,16);
		LCD_ShowNum(160,90,wheel3_pid.Current_value,6,16);
		LCD_ShowNum(160,110,wheel4_pid.Current_value,6,16);
		delay_ms(50);
	}
}



