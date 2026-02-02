#ifndef __MLSS_lanchang_H
#define __MLSS_lanchang_H

#include "main.h"
#include "rs485.h"
void MLSS_Lan_rs485_GetModbusId(void);
void MLSS_Lan_rs485_GetValue(PtrToDOProbe ptd);
void MLSS_Lan_rs485_Get_K_B(PtrToDOProbe ptd);
void MLSS_Lan_rs485_Set_Clear(PtrToDOProbe ptd);
void MLSS_Lan_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
#endif
