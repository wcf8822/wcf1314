#ifndef __PH_shenghui_H
#define __PH_shenghui_H

#include "main.h"
#include "rs485.h"
void pH_shenghui_rs485_GetModbusId(void);
void pH_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void pH_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void pH_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void pH_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value);
void pH_shenghui_rs485_Second(PtrToDOProbe ptd,float Value);
void pH_shenghui_rs485_Third(PtrToDOProbe ptd,float Value);
void pH_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void pH_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void pH_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);


#endif
