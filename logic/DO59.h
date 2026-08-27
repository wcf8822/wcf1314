#ifndef __DO59_H
#define __DO59_H

#include "main.h"
#include "rs485.h"
void DO59_rs485_GetModbusId(void);
void DO59_rs485_GetValue(PtrToDOProbe ptd);
void DO59_rs485_GetSalinity(PtrToDOProbe ptd);
void DO59_rs485_SetSalinity(PtrToDOProbe ptd, float sal);
void DO59_rs485_GetPressure(PtrToDOProbe ptd);
void DO59_rs485_SetPressure(PtrToDOProbe ptd, float press);
void DO59_rs485_Set_Temp_Cal(PtrToDOProbe ptd,float Value);
void DO59_rs485_Set_Zero_Cal(PtrToDOProbe ptd);
void DO59_rs485_Set_Slp_Cal(PtrToDOProbe ptd);
void DO59_rs485_ClearCal(PtrToDOProbe ptd);
void DO59_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);



void DO59_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void DO59_SetSHWVersion(PtrToDOProbe ptd);
void DO59_SetSalinityArr(PtrToDOProbe ptd, uint8_t* dat);
void DO59_SetPressureArr(PtrToDOProbe ptd, uint8_t* dat);
#endif
