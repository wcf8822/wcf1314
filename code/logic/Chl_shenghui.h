#ifndef __Chl_shenghui_H
#define __Chl_shenghui_H

#include "main.h"
#include "rs485.h"
void Chl_shenghui_rs485_GetModbusId(void);
void Chl_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void Chl_shenghui_rs485_GetVol(PtrToDOProbe ptd);
void Chl_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void Chl_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void Chl_shenghui_rs485_GetMeasure(PtrToDOProbe ptd);
void Chl_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value);
void Chl_shenghui_rs485_Second(PtrToDOProbe ptd,float Value);
void Chl_shenghui_rs485_Measure_Switch(PtrToDOProbe ptd, uint16_t Value);
void Chl_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void Chl_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void Chl_shenghui_Set_Measure_Range(PtrToDOProbe ptd, uint8_t* dat);
void Chl_shenghui_UpdateTemp2DO_Vol(PtrToDOProbe ptd, uint8_t *dat);
void Chl_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void Chl_shenghui_rs485_GetCal_Status(PtrToDOProbe ptd);


#endif
