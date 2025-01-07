#ifndef __DO_shenghui_H
#define __DO_shenghui_H

#include "main.h"
#include "rs485.h"

void DO_shenghui_rs485_GetModbusId(void);
void DO_shenghui_rs485_GetTempTwoDO(PtrToDOProbe ptd);
void DO_shenghui_rs485_GetDOPercent(PtrToDOProbe ptd);
void DO_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void DO_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void DO_shenghui_rs485_ZeroCal(PtrToDOProbe ptd);
void DO_shenghui_rs485_FullCal(PtrToDOProbe ptd);
void DO_shenghui_rs485_GetSalinity(PtrToDOProbe ptd);
void DO_shenghui_rs485_SetSalinity(PtrToDOProbe ptd, float sal);
void DO_shenghui_rs485_GetPressure(PtrToDOProbe ptd);
void DO_shenghui_rs485_SetPressure(PtrToDOProbe ptd, float press);
void DO_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void DO_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void DO_shenghui_UpdateTemp2DO_Percent(PtrToDOProbe ptd, uint8_t *dat);
void DO_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void DO_shenghui_SetSalinityArr(PtrToDOProbe ptd, uint8_t* dat);
void DO_shenghui_SetPressureArr(PtrToDOProbe ptd, uint8_t* dat);

#endif
