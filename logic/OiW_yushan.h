#ifndef __OiW_yushan_H
#define __OiW_yushan_H

#include "main.h"
#include "rs485.h"

void OiW_yushan_rs485_GetModbusId(uint8_t modbus_id);

void OiW_yushan_rs485_GetValue(PtrToDOProbe ptd,uint8_t modbus_id);
void OiW_yushan_rs485_GetSN(PtrToDOProbe ptd,uint8_t modbus_id);

void OiW_yushan_rs485_GetSHWVersion(PtrToDOProbe ptd,uint8_t modbus_id);

void OiW_yushan_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void OiW_yushan_rs485_SetSN(PtrToDOProbe ptd,uint8_t *dat);
void OiW_yushan_rs485_SetSHWVersion(PtrToDOProbe ptd,uint8_t *dat);
void OiW_yushan_rs485_GetCal_k_b(PtrToDOProbe ptd,uint8_t modbus_id);

void OiW_yushan_rs485_SetCal_k_b(PtrToDOProbe ptd,uint8_t *dat);
void OiW_yushan_rs485_Set_OIW_K_b_Reset(PtrToDOProbe ptd,float value_1,float value_2,uint8_t modbus_id);


#endif
