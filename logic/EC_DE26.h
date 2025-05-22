#ifndef __EC_DE26_H
#define __EC_DE26_H

#include "main.h"
#include "rs485.h"

void EC_DE26_rs485_GetModbusId(void);
void EC_DE26_rs485_GetValue(PtrToDOProbe ptd);
void EC_DE26_rs485_GetSN(PtrToDOProbe ptd);
void EC_DE26_rs485_GetSHWVersion(PtrToDOProbe ptd);
void EC_DE26_rs485_Frist(PtrToDOProbe ptd,float Value);
void EC_DE26_rs485_Zero(PtrToDOProbe ptd,float Value);
void EC_DE26_rs485_SetTemp(PtrToDOProbe ptd, float temp);
void EC_DE26_rs485_SetMode(PtrToDOProbe ptd,uint16_t Value);
void EC_DE26_rs485_ClearCal(PtrToDOProbe ptd);
void EC_DE26_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void EC_DE26_SetSHWVersion(PtrToDOProbe ptd);
void EC_DE26_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void EC_DE26_SetTds_value(PtrToDOProbe ptd,float Value);
void EC_DE26_SetTemp_value(PtrToDOProbe ptd,float Value);
void EC_DE26_rs485_Get_tds_temp_value(PtrToDOProbe ptd);
void EC_DE26_Settds_temp_xishu(PtrToDOProbe ptd,uint8_t *dat);

#endif
