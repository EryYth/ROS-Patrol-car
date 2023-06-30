#include "pid.h"


void Clear_accumulation(PID* pid)
{
	pid->Out_value = 0;
	pid->Error = 0;
	pid->L_Error = 0;
	pid->LL_Error = 0;
}

void Incremental_PID(PID* pid)
{
	//��¼��ǰ���
	float Error;
	Error = pid->Target_value - pid->Current_value;
	//������ֵ
	pid->Out_value += pid->KP * (Error - pid->L_Error) \
									+ pid->KI * Error \
									+ pid->KD * (Error - 2 * pid->L_Error + pid->LL_Error);
	//ˢ���������
	pid->LL_Error = pid->L_Error;
	pid->L_Error = Error;
	//��ֵ
	if(pid->Out_value > pid->max)pid->Out_value = pid->max;
	if(pid->Out_value < pid->min)pid->Out_value = pid->min;
}

