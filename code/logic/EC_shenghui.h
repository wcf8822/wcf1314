#ifndef __EC_shenghui_H
#define __EC_shenghui_H

#include "main.h"
#include "rs485.h"
void EC_shenghui_rs485_GetModbusId(void);
void EC_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void EC_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void EC_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void EC_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value);
void EC_shenghui_rs485_Second(PtrToDOProbe ptd,float Value);
void EC_shenghui_rs485_CalNumber(PtrToDOProbe ptd, uint16_t Value);
void EC_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void EC_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void EC_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);


#endif
