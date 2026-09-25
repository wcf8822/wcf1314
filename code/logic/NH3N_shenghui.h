#ifndef __NH3N_shenghui_H
#define __NH3N_shenghui_H

#include "main.h"
#include "rs485.h"
void NH3N_shenghui_rs485_GetModbusId(void);
void NH3N_shenghui_rs485_GetValue(PtrToDOProbe ptd);
void NH3N_shenghui_rs485_GetSN(PtrToDOProbe ptd);
void NH3N_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd);
void NH3N_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value);
void NH3N_shenghui_rs485_Second(PtrToDOProbe ptd,float Value);
void NH3N_shenghui_pH_rs485_Frist(PtrToDOProbe ptd,float Value);
void NH3N_shenghui_pH_rs485_Second(PtrToDOProbe ptd,float Value);
void NH3N_shenghui_pH_rs485_Third(PtrToDOProbe ptd,float Value);
void NH3N_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void NH3N_shenghui_rs485_ClearCal(PtrToDOProbe ptd);
void shenghui_SetSN(PtrToDOProbe ptd, uint8_t* buff, uint8_t len);
void shenghui_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data);
void NH3N_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);

#endif
