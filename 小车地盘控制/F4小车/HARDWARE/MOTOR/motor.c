#include "motor.h"


PID wheel1_pid = {0.8,0.3,0.1,WHEELM,-WHEELM};	//{1.1,0.5,0.1,WHEELM,-WHEELM};
PID wheel2_pid = {0.8,0.3,0.1,WHEELM,-WHEELM};  //{1.1,0.5,0.1,WHEELM,-WHEELM};
PID wheel3_pid = {0.8,0.3,0.1,WHEELM,-WHEELM};  //{1.1,0.5,0.1,WHEELM,-WHEELM};
PID wheel4_pid = {0.8,0.3,0.1,WHEELM,-WHEELM};  //{1.1,0.5,0.1,WHEELM,-WHEELM};

float Wheel_Speed_L, Wheel_Speed_R;
float Wheel_Speed_Target_L, Wheel_Speed_Target_R;
float Vx, Vz;

void Motor_PWM_Init(u16 arr,u16 psc)
{
	Motor_Init();
	PWM_Init(arr, psc);
}



//����1����ת���� PB6 PB7
//����2����ת���� PC8 PC9
//����3����ת���� PB10 PB11
//����4����ת���� PB12 PB14
void Motor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
  GPIO_Init(GPIOB, &GPIO_InitStructure);	

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
}


//����1PWM��� TIM10->CCR1 PF6
//����2PWM��� TIM11->CCR1 PF7
//����3PWM��� TIM2->CCR3  PA2
//����4PWM��� TIM2->CCR4  PA3
void PWM_Init(u16 arr,u16 psc)
{		 		
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	//TIM2ʱ��ʹ��    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10|RCC_APB2Periph_TIM11,ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOF, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM2); 			//TIM2   ch2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2); 			//TIM2   ch3
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_TIM10); 			//TIM10  ch1
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_TIM11); 			//TIM11  ch1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //��ʼ��PF9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);
	
	//��ʼ��TIM2 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);  
	
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);  
	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  
	 
  TIM_CtrlPWMOutputs(TIM2,ENABLE);	
  TIM_ARRPreloadConfig(TIM2,ENABLE);//ARPEʹ�� 
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM14
	 
	TIM_CtrlPWMOutputs(TIM10,ENABLE);	
  TIM_ARRPreloadConfig(TIM10,ENABLE);//ARPEʹ�� 
	TIM_Cmd(TIM10, ENABLE);  //ʹ��TIM14
	  
	TIM_CtrlPWMOutputs(TIM11,ENABLE);	
  TIM_ARRPreloadConfig(TIM11,ENABLE);//ARPEʹ�� 
	TIM_Cmd(TIM11, ENABLE);  //ʹ��TIM14
	
 
} 




/*��ֵ����*/
/*��ڲ�����PID������ɺ������PWMֵ*/
//����PWM
void Set_Pwm(int moto1,int moto2,int moto3,int moto4)
{
    	if(moto1>0)	WHEEL1_IN2=0,			WHEEL1_IN1=1;
			else 	      WHEEL1_IN2=1,			WHEEL1_IN1=0;
			WHEEL1_PWM=abs(moto1);
		  if(moto2>0)	WHEEL2_IN2=0,			WHEEL2_IN1=1;
			else        WHEEL2_IN2=1,			WHEEL2_IN1=0;
			WHEEL2_PWM=abs(moto2);
	
			if(moto3>0)	WHEEL3_IN2=0,			WHEEL3_IN1=1;
			else        WHEEL3_IN2=1,			WHEEL3_IN1=0;
			WHEEL3_PWM=abs(moto3);
			if(moto4>0)	WHEEL4_IN2=0,			WHEEL4_IN1=1;
			else        WHEEL4_IN2=1,			WHEEL4_IN1=0;
			WHEEL4_PWM=abs(moto4);
}
//�����ٶȣ����ٶ�תΪ������ֵ 
//�õ�������Ŀ��ֵ
void Set_Speed(float wheel_l, float wheel_r)
{
	wheel1_pid.Target_value = wheel3_pid.Target_value = wheel_l * CONTROL_TIMER_CYCLE	/ WHEEL_DIAMETER / PI * ENCODER_COUNT_VALUE * 2;
	wheel2_pid.Target_value = wheel4_pid.Target_value = wheel_r * CONTROL_TIMER_CYCLE	/ WHEEL_DIAMETER / PI * ENCODER_COUNT_VALUE * 2;
}

//�õ���������ǰֵ
void Get_Encoder(void)
{
	wheel1_pid.Current_value = Read_Encoder(1);
	wheel2_pid.Current_value = Read_Encoder(2);
	wheel3_pid.Current_value = Read_Encoder(3);
	wheel4_pid.Current_value = Read_Encoder(4);
}

//ͨ��������Ŀ��ֵ����ǰֵ���õ�PWMֵ
void Get_Pwm_Set(void)
{
	Incremental_PID(&wheel1_pid);
	Incremental_PID(&wheel2_pid);
	Incremental_PID(&wheel3_pid);
	Incremental_PID(&wheel4_pid);
	
  Set_Pwm(wheel1_pid.Out_value, wheel2_pid.Out_value, wheel3_pid.Out_value, wheel4_pid.Out_value);
}

void Clear_PWM(void)
{
	Clear_accumulation(&wheel1_pid);
	Clear_accumulation(&wheel2_pid);
	Clear_accumulation(&wheel3_pid);
	Clear_accumulation(&wheel4_pid);
}


void Get_Speed(void)
{
	Get_Encoder();
	Wheel_Speed_L = \
	(WHEEL_DIAMETER * PI * (((wheel1_pid.Current_value + wheel3_pid.Current_value) / 2) / ENCODER_COUNT_VALUE) / CONTROL_TIMER_CYCLE);
	Wheel_Speed_R = \
	(WHEEL_DIAMETER * PI * (((wheel2_pid.Current_value + wheel4_pid.Current_value) / 2) / ENCODER_COUNT_VALUE) / CONTROL_TIMER_CYCLE);
}

void Speed_Transformation(float vx, float vz)
{
	if(vx == 0.0f){			//ԭ����ת��ֹ
	Wheel_Speed_Target_R = vz * Base_Width / 2.0f;
	Wheel_Speed_Target_L  = (-1) * Wheel_Speed_Target_R;
	}
	else if(vz == 0.0f){	//��ֹ����ǰ���˶�
		Wheel_Speed_Target_R = Wheel_Speed_Target_L = vx;
	}	
	else{					//��ǰ�����ߺ��˹�����ת��
		Wheel_Speed_Target_L = vx - vz * Base_Width / 2.0f;
		Wheel_Speed_Target_R = vx + vz * Base_Width / 2.0f;
	}
	LCD_ShowNum(160,50,abs(vx*100),6,16);
		LCD_ShowNum(160,70,abs(vz*100),6,16);
}

void Speed_Inverse_Transformation(float *x, float *y, float *z)
{
	*x = (Wheel_Speed_L + Wheel_Speed_R) / 2.0f;
	*y = 0;
	*z = (Wheel_Speed_L - Wheel_Speed_R) / Base_Width;
}

void Run_Spend(void)
{
	RX_Error++;
	if(RX_Error == 100){
		RX_Error = 0;
		Wheel_Speed_Target_L = Wheel_Speed_Target_R = 0;
	}
	Set_Speed(Wheel_Speed_Target_L, Wheel_Speed_Target_R);
}