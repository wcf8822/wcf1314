#ifndef __DX01_H
#define __DX01_H

#include "main.h"
#include "rs485.h"

void DX01_rs485_GetModbusId(void);
void DX01_rs485_GetValue(PtrToDOProbe ptd);
void DX01_rs485_GetTemp_CalValue(PtrToDOProbe ptd);
void DX01_rs485_Get_Avg_num(PtrToDOProbe ptd);
void DX01_rs485_Set_Temp_Cal(PtrToDOProbe ptd,int16_t Value);
void DX01_rs485_Set_Zero_Cal(PtrToDOProbe ptd,int16_t Value);
void DX01_rs485_Set_Slp_Cal(PtrToDOProbe ptd,float Value);
void DX01_rs485_ClearCal(PtrToDOProbe ptd);
void DX01_rs485_Set_Clear(PtrToDOProbe ptd);
void DX01_rs485_Set_Clear_time(PtrToDOProbe ptd,uint16_t value);
void DX01_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void DX01_SetSHWVersion(PtrToDOProbe ptd);
void DX01_SetTemp_Cal_value(PtrToDOProbe ptd ,uint8_t *dat);
void DX01_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void DX01_rs485_GetClear_time(PtrToDOProbe ptd);
void DX01_rs485_SetTemp_CalValue(PtrToDOProbe ptd,uint8_t *dat);
void DX01_rs485_Set_Avg_num(PtrToDOProbe ptd,uint8_t *dat);
void DX01_rs485_SetClear_time(PtrToDOProbe ptd,uint8_t *dat);
void DX01_rs485_Set_Temp_Cal_zero(PtrToDOProbe ptd,int16_t Value);

#endif
