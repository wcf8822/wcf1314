#ifndef __PH_DPH07_H
#define __PH_DPH07_H

#include "main.h"
#include "rs485.h"
void pH_DpH07_rs485_GetModbusId(void);
void pH_DpH07_rs485_GetValue(PtrToDOProbe ptd);
void pH_DpH07_rs485_GetSN(PtrToDOProbe ptd);
void pH_DpH07_rs485_GetSHWVersion(PtrToDOProbe ptd);
void pH_DpH07_rs485_Cal_pH(PtrToDOProbe ptd,uint16_t Value);

void pH_DpH07_rs485_SetTemp(PtrToDOProbe ptd, uint16_t Value);
void pH_DpH07_rs485_ClearCal(PtrToDOProbe ptd);
void pH_DpH07_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void pH_DpH07_SetSN(PtrToDOProbe ptd);

void pH_DpH07_rs485_GetStander(PtrToDOProbe ptd);
void pH_DpH07_rs485_SetStander(PtrToDOProbe ptd, uint16_t Value);
void pH_DpH07_SetStander(PtrToDOProbe ptd,uint8_t *dat);

#endif
