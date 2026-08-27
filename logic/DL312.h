#ifndef __DL312_H
#define __DL312_H

#include "main.h"
#include "rs485.h"
void DL312_rs485_GetModbusId(void);
void DL312_rs485_GetValue(PtrToDOProbe ptd);
void DL312_rs485_GetSN(PtrToDOProbe ptd);
void DL312_rs485_GetSHWVersion(PtrToDOProbe ptd);
void DL312_rs485_Set_Temp_Cal(PtrToDOProbe ptd,float Value);
void DL312_rs485_GetTemp_CalValue(PtrToDOProbe ptd);
void DL312_SetTemp_Cal_value(PtrToDOProbe ptd ,uint8_t *dat);

void DL312_rs485_Set_Zero_Cal(PtrToDOProbe ptd,int16_t Value);
void DL312_rs485_Set_Slp_Cal(PtrToDOProbe ptd,int16_t Value);
void DL312_rs485_ClearCal(PtrToDOProbe ptd);
void DL312_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);


void DL312_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void DL312_SetSHWVersion(PtrToDOProbe ptd);

void DL312_rs485_Set_Frist_Value(PtrToDOProbe ptd,float Value);
void DL312_rs485_Set_Secend_Value(PtrToDOProbe ptd,float Value);
#endif
