#ifndef __EC_DE40_H
#define __EC_DE40_H

#include "main.h"
#include "rs485.h"

void EC_DE40_rs485_GetModbusId(uint8_t modbus);
void EC_DE40_rs485_GetValue(PtrToDOProbe ptd,uint8_t modbus);
void EC_DE40_rs485_GetSN(PtrToDOProbe ptd,uint8_t modbus);

void EC_DE40_rs485_GetSHWVersion(PtrToDOProbe ptd,uint8_t modbus);

void EC_DE40_rs485_Frist(PtrToDOProbe ptd,float Value);
void EC_DE40_rs485_Zero(PtrToDOProbe ptd,float Value);
void EC_DE40_rs485_SetTemp(PtrToDOProbe ptd, float temp,uint8_t modbus);
void EC_DE40_rs485_SetMode(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus);
void EC_DE40_rs485_ClearCal(PtrToDOProbe ptd,uint8_t modbus);
void EC_DE40_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void EC_DE40_SetSHWVersion(PtrToDOProbe ptd);
void EC_DE40_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void EC_DE40_SetTds_value(PtrToDOProbe ptd,float Value);
void EC_DE40_SetTemp_value(PtrToDOProbe ptd,float Value);
void EC_DE40_rs485_Get_tds_temp_value(PtrToDOProbe ptd,uint8_t modbus);

void EC_DE40_Settds_temp_xishu(PtrToDOProbe ptd,uint8_t *dat);
void EC_DE40_rs485_Set_sal_unit(PtrToDOProbe ptd,uint8_t value,uint8_t modbus);
void EC_DE40_rs485_SAL_cal(PtrToDOProbe ptd,float Value,uint8_t modbus);
void EC_DE40_rs485_Get_sal_unit(PtrToDOProbe ptd,uint8_t modbus);
void EC_DE40_Set_sal_unit(PtrToDOProbe ptd,uint8_t *dat);
void EC_DS46_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);

#endif
