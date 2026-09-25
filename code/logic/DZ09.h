#ifndef __DZ09_H
#define __DZ09_H

#include "main.h"
#include "rs485.h"
void DZ09_rs485_GetModbusId(void);
void DZ09_rs485_GetValue(PtrToDOProbe ptd);
void DZ09_rs485_GetSN(PtrToDOProbe ptd);
void DZ09_rs485_GetSHWVersion(PtrToDOProbe ptd);
void DZ09_rs485_Set_Temp_Cal(PtrToDOProbe ptd,int16_t Value);
void DZ09_rs485_GetTemp_CalValue(PtrToDOProbe ptd);
void DZ09_SetTemp_Cal_value(PtrToDOProbe ptd ,uint8_t *dat);

void DZ09_rs485_Set_Zero_Cal(PtrToDOProbe ptd,int16_t Value);
void DZ09_rs485_Set_Slp_Cal(PtrToDOProbe ptd,float Value);
void DZ09_rs485_ClearCal(PtrToDOProbe ptd);
void DZ09_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void Set_TUR_Connect_FLAG(uint8_t value);
uint8_t Get_TUR_Connect_FLAG(void);
void DZ09_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void DZ09_SetSHWVersion(PtrToDOProbe ptd);
void DZ09_rs485_Set_Temp_Cal_zero(PtrToDOProbe ptd,int16_t Value);

#endif
