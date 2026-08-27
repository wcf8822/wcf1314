#ifndef __DY05_H
#define __DY05_H

#include "main.h"
#include "rs485.h"
void DY05_rs485_GetModbusId(void);
void DY05_rs485_GetValue(PtrToDOProbe ptd);
void DY05_rs485_GetSalinity(PtrToDOProbe ptd);
void DY05_rs485_SetSalinity(PtrToDOProbe ptd, float sal);
void DY05_rs485_GetPressure(PtrToDOProbe ptd);
void DY05_rs485_SetPressure(PtrToDOProbe ptd, float press);
void DY05_rs485_GetSHWVersion(PtrToDOProbe ptd);
void DY05_rs485_Set_Temp_Cal(PtrToDOProbe ptd,float Value);
void DY05_rs485_Set_Zero_Cal(PtrToDOProbe ptd);
void DY05_rs485_Set_Slp_Cal(PtrToDOProbe ptd);
void DY05_rs485_ClearCal(PtrToDOProbe ptd);
void DY05_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);

void DY05_rs485_GetTemp_CalValue(PtrToDOProbe ptd);
void DY05_rs485_mode(PtrToDOProbe ptd);
void DY05_SetTemp_Cal_value(PtrToDOProbe ptd ,uint8_t *dat);
void DY05_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void DY05_SetSHWVersion(PtrToDOProbe ptd);
void DY05_SetSalinityArr(PtrToDOProbe ptd, uint8_t* dat);
void DY05_SetPressureArr(PtrToDOProbe ptd, uint8_t* dat);
#endif
