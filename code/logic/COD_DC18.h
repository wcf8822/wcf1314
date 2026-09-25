#ifndef __COD_DC18_H
#define __COD_DC18_H

#include "main.h"
#include "rs485.h"


void COD_DC18_rs485_GetModbusId(uint8_t modbus_id);
void COD_DC18_rs485_GetValue(PtrToDOProbe ptd,uint8_t modbus_id);
void COD_DC18_rs485_GetSN(PtrToDOProbe ptd,uint8_t modbus_id);
void COD_DC18_rs485_GetSHWVersion(PtrToDOProbe ptd,uint8_t modbus_id);
void COD_DC18_Tur_rs485_ZeroCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id);
void COD_DC18_Tur_rs485_SlopeCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id);
void COD_DC18_rs485_ZeroCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id);
void COD_DC18_rs485_SlopeCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id);
void COD_DC18_rs485_SetTemp(PtrToDOProbe ptd, int16_t temp ,uint8_t modbus_id);
void COD_DC18_rs485_Coefficient(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id);
void COD_DC18_rs485_Cleanse(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id);
void COD_DC18_rs485_ClearCal(PtrToDOProbe ptd,uint8_t modbus_id);
void COD_DC17_rs485_Set_Mes(PtrToDOProbe ptd,uint16_t Value);
void COD_DC17_rs485_Set_Mes_Time(PtrToDOProbe ptd,uint16_t Value);
void COD_DC18_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len);
void COD_DC18_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data);
void COD_DC18_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void COD_DC18_rs485_Get_U(PtrToDOProbe ptd,uint8_t modbus_id);
void COD_DC17_rs485_Get_Mes_Para(PtrToDOProbe ptd);
void COD_DC18_Set_U(PtrToDOProbe ptd, uint8_t* data);
void COD_DC17_Set_Mes_Para(PtrToDOProbe ptd, uint8_t* data);
void COD_DC17_UPdata_Mes_Para(PtrToDOProbe ptd, uint8_t* data);//只更新测量模式
void COD_DC17_rs485_IN_tur_ZeroCal(PtrToDOProbe ptd,float Value);//内部校准
void COD_DC17_rs485_INZeroCal(PtrToDOProbe ptd,float Value);//内部校准
#endif
