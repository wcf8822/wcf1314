#ifndef __COD_shenghui_H
#define __COD_shenghui_H

#include "main.h"
#include "rs485.h"

void COD_shenghui_rs485_GetModbusId(void);
void COD_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void COD_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void COD_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void COD_shenghui_Tur_rs485_Frist(PtrToDOProbe ptd,float Value);
void COD_shenghui_Tur_rs485_Second(PtrToDOProbe ptd,float Value);
void COD_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value);
void COD_shenghui_rs485_Second(PtrToDOProbe ptd,float Value);
void COD_shenghui_rs485_Third(PtrToDOProbe ptd,float Value);
void COD_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void COD_shenghui_rs485_ZeroCal(PtrToDOProbe ptd);
void COD_shenghui_rs485_Cleanse(PtrToDOProbe ptd);
void COD_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void COD_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);






#endif
