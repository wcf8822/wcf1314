#ifndef __ORP_DR31_H
#define __ORP_DR31_H

#include "main.h"
#include "rs485.h"
void ORP_DR31_rs485_GetModbusId(void);
void ORP_DR31_rs485_GetValue(PtrToDOProbe ptd);
void ORP_DR31_rs485_CAL(PtrToDOProbe ptd,int16_t Value);
void ORP_DR31_rs485_ClearCal(PtrToDOProbe ptd);
void ORP_DR31_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
#endif
