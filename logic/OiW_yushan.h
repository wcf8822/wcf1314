#ifndef __OiW_yushan_H
#define __OiW_yushan_H

#include "main.h"
#include "rs485.h"

void OiW_yushan_rs485_GetModbusId(void);
void OiW_yushan_rs485_GetValue(PtrToDOProbe ptd);
void OiW_yushan_rs485_GetSN(PtrToDOProbe ptd);
void OiW_yushan_rs485_GetSHWVersion(PtrToDOProbe ptd);
void OiW_yushan_rs485_Set_OIW_K(PtrToDOProbe ptd,float Value);
void OiW_yushan_rs485_Set_OIW_B(PtrToDOProbe ptd,float Value);
void OiW_yushan_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void OiW_yushan_rs485_SetSN(PtrToDOProbe ptd,uint8_t *dat);
void OiW_yushan_rs485_SetSHWVersion(PtrToDOProbe ptd,uint8_t *dat);
void OiW_yushan_rs485_GetCal_k_b(PtrToDOProbe ptd);
void OiW_yushan_rs485_SetCal_k_b(PtrToDOProbe ptd,uint8_t *dat);
void OiW_yushan_rs485_Set_OIW_K_b_Reset(PtrToDOProbe ptd,float value_1,float value_2);

#endif
