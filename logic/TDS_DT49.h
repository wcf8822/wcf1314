#ifndef __TDS_DT49_H
#define __TDS_DT49_H

#include "main.h"
#include "rs485.h"

extern void TDS_DT49_rs485_GetModbusId(void);
extern void TDS_DT49_rs485_GetValue(void);
extern void TDS_DT49_rs485_Get_tds_temp_value(void);
extern void TDS_DT49_rs485_GetSN(void);
extern void TDS_DT49_rs485_GetSHWVersion(void);
extern void TDS_DT49_rs485_Standard(PtrToDOProbe ptd,float Value);
extern void TDS_DT49_SetTds_value(PtrToDOProbe ptd,float Value);
extern void TDS_DT49_rs485_SetTemp(PtrToDOProbe ptd, float temp);
extern void TDS_DT49_rs485_Zero(PtrToDOProbe ptd,float Value);
extern void TDS_DT49_rs485_SetMode(PtrToDOProbe ptd,uint16_t Value);
extern void TDS_DT49_rs485_ClearCal(PtrToDOProbe ptd);


#endif
