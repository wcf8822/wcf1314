#ifndef __BGA_shenghui_H
#define __BGA_shenghui_H

#include "main.h"
#include "rs485.h"

void BGA_shenghui_rs485_GetModbusId(void);
void BGA_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void BGA_shenghui_rs485_GetVol(PtrToDOProbe ptd);
void BGA_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void BGA_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void BGA_shenghui_rs485_Get_Cal_INFO(PtrToDOProbe ptd);
void BGA_shenghui_rs485_Get_Cal_temp(PtrToDOProbe ptd);
void BGA_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void BGA_shesnghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void BGA_shenghui_cal_rs485_Frist(PtrToDOProbe ptd,float Value);
void BGA_shenghui_cal_rs485_Second(PtrToDOProbe ptd,float Value);
void BGA_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);

#endif

