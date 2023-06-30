#include "control.h"


void Remote_control(void)
{
	short RX,RY,LX,LY;
	
	if(app_ps2_deal() != PSB_L2) return;
	
	RX = PS2_AnologData(PSS_RX) - 128;
	RY = PS2_AnologData(PSS_RY) - 128;
	LX = PS2_AnologData(PSS_LX) - 128;
	LY = PS2_AnologData(PSS_LY) - 128;	
//	LCD_ShowNum(160,50,RX+128,6,16);
//	LCD_ShowNum(160,70,RY+128,6,16);
//	LCD_ShowNum(160,90,LX+128,6,16);
//	LCD_ShowNum(160,110,LY+128,6,16); 
	
	
	if((abs(LY)>15)){
		Set_Speed(-(LY / 128.0) * 0.1 + (RX / 128.0) * 0.05, -(LY / 128.0) * 0.1 - (RX / 128.0) * 0.05);
	}
	else{
		Clear_PWM();
		Set_Speed(0,0);
	}
	
}

