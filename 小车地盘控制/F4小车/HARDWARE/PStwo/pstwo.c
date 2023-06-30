#include "pstwo.h"


u16 Handkey;
u8 Comd[2]={0x01,0x42};	//��ʼ�����������
u8 Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //���ݴ洢����
u8 scan[9]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//{0x01,0x42,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};	// ���Ͷ�ȡ

u16 MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK,
	  PSB_TRIANGLE,
	  PSB_CIRCLE,
	  PSB_CROSS,
	  PSB_SQUARE
	};	//����ֵ�밴����

//�ֱ��ӿڳ�ʼ��    ����  DI->PG2 
//                  ���  DO->PG3    CS->PF4  CLK->PG5
void PS2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);  	/*����GPIOAB������ʱ��*/
	
  	GPIO_InitStructure.GPIO_Pin = PS2_MISO_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      /*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(PS2_MISO_PORT, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin = PS2_SCK_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      /*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(PS2_SCK_PORT, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = PS2_CS_PIN ;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      /*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(PS2_CS_PORT, &GPIO_InitStructure);	


	GPIO_InitStructure.GPIO_Pin = PS2_MOSI_PIN;	//DI
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(PS2_MOSI_PORT, &GPIO_InitStructure);
	
	DO_H;
	CLK_H;
	CS_H; 	 											  
}

//��ȡ�ֱ�����
u8 PS2_ReadData(u8 command)
{

	u8 i,j=1;
	u8 res=0; 
    for(i=0; i<=7; i++)          
    {
		if(command&0x01)
			DO_H;
		else
			DO_L;
		command = command >> 1;
		delay_us(6);//10
		CLK_L;
		delay_us(6); //10
		if(DI) 
			res = res + j;
		j = j << 1; 
		CLK_H;
		delay_us(6);//10	 
    }
    DO_H;
	  delay_us(55);//50
    return res;	
}

//�Զ������� PS2 �����ݽ��д���
//����Ϊ 0�� δ����Ϊ 1
unsigned char PS2_DataKey()
{
	u8 index = 0, i = 0;

	PS2_ClearData();
	CS_L;
	for(i=0;i<9;i++)	//����ɨ�谴��
	{
		Data[i] = PS2_ReadData(scan[i]);	
	} 
	CS_H;
	

	Handkey=(Data[4]<<8)|Data[3];     //����16������  ����Ϊ0�� δ����Ϊ1
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(MASK[index]-1)))==0)
			return index+1;
	}
	return 0;          //û���κΰ�������
}


////���ֱ���������
//void PS2_Cmd(u8 CMD)
//{
//	volatile u16 ref=0x01;
//	Data[1] = 0;
//	for(ref=0x01;ref<0x0100;ref<<=1)
//	{
//		if(ref&CMD)
//		{
//			DO_H;                   //�����Ϊ����λ
//		}
//		else DO_L;

//		CLK_H;                        //ʱ������
//		delay_us(50);
//		CLK_L;
//		delay_us(50);
//		CLK_H;
//		if(DI)
//			Data[1] = ref|Data[1];
//	}
//}
////�ж��Ƿ�Ϊ���ģʽ
////����ֵ��0�����ģʽ
////		  ����������ģʽ
//u8 PS2_RedLight(void)
//{
//	CS_L;
//	PS2_Cmd(Comd[0]);  //��ʼ����
//	PS2_Cmd(Comd[1]);  //��������
//	CS_H;
//	if( Data[1] == 0X73)   return 0 ;
//	else return 1;

//}
////��ȡ�ֱ�����
//void PS2_ReadData(void)
//{
//	volatile u8 byte=0;
//	volatile u16 ref=0x01;

//	CS_L;

//	PS2_Cmd(Comd[0]);  //��ʼ����
//	PS2_Cmd(Comd[1]);  //��������

//	for(byte=2;byte<9;byte++)          //��ʼ��������
//	{
//		for(ref=0x01;ref<0x100;ref<<=1)
//		{
//			CLK_H;
//			CLK_L;
//			delay_us(50);
//			CLK_H;
//		      if(DI)
//		      Data[byte] = ref|Data[byte];
//		}
//        delay_us(50);
//	}
//	CS_H;	
//}

////�Զ�������PS2�����ݽ��д���      ֻ�����˰�������         Ĭ�������Ǻ��ģʽ  ֻ��һ����������ʱ
////����Ϊ0�� δ����Ϊ1
//u8 PS2_DataKey()
//{
//	u8 index;

//	PS2_ClearData();
//	PS2_ReadData();

//	Handkey=(Data[4]<<8)|Data[3];     //����16������  ����Ϊ0�� δ����Ϊ1
//	for(index=0;index<16;index++)
//	{	    
//		if((Handkey&(1<<(MASK[index]-1)))==0)
//		return index+1;
//	}
//	return 0;          //û���κΰ�������
//}

//�õ�һ��ҡ�˵�ģ����	 ��Χ0~256
u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

//������ݻ�����
void PS2_ClearData()
{
	u8 a;
	for(a=0;a<9;a++)
		Data[a]=0x00;
}

u8 app_ps2_deal(void)
{
	u8 PS2_KEY = 0;

	
	__set_PRIMASK(1);  //���жϣ�  
	 
	PS2_KEY = PS2_DataKey();	 //�ֱ�����������
	   
	__set_PRIMASK(0);//���ж�

	return PS2_KEY;
//	switch(PS2_KEY)
//	{
//		case PSB_SELECT: 	  break;
//		case PSB_L3:     	  break;  
//		case PSB_R3:        break;  
//		case PSB_START:   	break;  
//		case PSB_PAD_UP: 	  break;  
//		case PSB_PAD_RIGHT:	break;
//		case PSB_PAD_DOWN:  break; 
//		case PSB_PAD_LEFT:  break; 
//		case PSB_L2:      	 	
//		case PSB_R2:        break; 
//		case PSB_L1:      	break; 
//		case PSB_R1:      	break;     
//		case PSB_TRIANGLE:	break; 							
//		case PSB_CIRCLE:  	break;  	
//		case PSB_CROSS:   	break; 					
//		case PSB_SQUARE:  	break;  	
//	}

}
