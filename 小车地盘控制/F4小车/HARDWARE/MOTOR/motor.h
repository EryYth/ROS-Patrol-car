#ifndef __MOTOR_H
#define __MOTOR_H

#include "top.h"
#include "pid.h"	 


#define WHEEL1_IN1   PBout(6)
#define WHEEL1_IN2   PBout(7)
#define WHEEL2_IN1   PCout(8)
#define WHEEL2_IN2   PCout(9)
#define WHEEL3_IN1   PBout(10)
#define WHEEL3_IN2   PBout(11)
#define WHEEL4_IN1   PBout(12)
#define WHEEL4_IN2   PBout(13)
#define WHEEL1_PWM   TIM10->CCR1  //PF6  电机1
#define WHEEL2_PWM   TIM11->CCR1  //PF7   电机2
#define WHEEL3_PWM   TIM2->CCR3  //PA2  电机3
#define WHEEL4_PWM   TIM2->CCR4  //PA3   电机4
#define WHEELM			 999


#define	ENCODER_COUNT_VALUE 1320.0f
#define	WHEEL_DIAMETER			0.067f	//主动轮直径	
#define	CONTROL_TIMER_CYCLE	0.1f
#define PI									3.1415f
#define Base_Width					0.196f


extern PID wheel1_pid,wheel2_pid,wheel3_pid,wheel4_pid;


void PWM_Init(u16 arr,u16 psc);
void Motor_Init(void);
void Set_Pwm(int moto1,int moto2,int moto3,int moto4);
void Set_Speed(float wheel_l, float wheel_r);
void Get_Pwm_Set(void);
void Get_Encoder(void);
void Motor_PWM_Init(u16 arr,u16 psc);
void Clear_PWM(void);
void Get_Speed(void);
void Speed_Transformation(float vx, float vz);
void Speed_Inverse_Transformation(float *x, float *y, float *z);
void Run_Spend(void);

#endif
