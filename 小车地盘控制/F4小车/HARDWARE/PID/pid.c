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
	//记录当前误差
	float Error;
	Error = pid->Target_value - pid->Current_value;
	//求出输出值
	pid->Out_value += pid->KP * (Error - pid->L_Error) \
									+ pid->KI * Error \
									+ pid->KD * (Error - 2 * pid->L_Error + pid->LL_Error);
	//刷新误差数据
	pid->LL_Error = pid->L_Error;
	pid->L_Error = Error;
	//限值
	if(pid->Out_value > pid->max)pid->Out_value = pid->max;
	if(pid->Out_value < pid->min)pid->Out_value = pid->min;
}

