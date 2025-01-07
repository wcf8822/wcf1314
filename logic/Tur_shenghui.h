#ifndef __TUR_shenghui_H
#define __TUR_shenghui_H

#include "main.h"
#include "rs485.h"
void Tur_shenghui_rs485_GetModbusId(void);
void Tur_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void Tur_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void Tur_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void Tur_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value);
void Tur_shenghui_rs485_Second(PtrToDOProbe ptd,float Value);
void Tur_shenghui_rs485_Third(PtrToDOProbe ptd,float Value);
void Tur_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void Tur_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void Tur_shenghui_rs485_Get_Tur_Status(PtrToDOProbe ptd);



#endif
