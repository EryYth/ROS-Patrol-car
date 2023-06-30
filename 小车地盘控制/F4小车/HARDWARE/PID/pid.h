#ifndef __PID_H
#define __PID_H

#include "sys.h"


typedef struct {
	float KP,KI,KD;
	float max,min;
	float Current_value,Target_value;
	float Error,L_Error,LL_Error;   //Îó²î
	float Out_value;
}PID; 


void Incremental_PID(PID* pid);
void Clear_accumulation(PID* pid);
	
#endif
