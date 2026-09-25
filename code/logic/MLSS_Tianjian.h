#ifndef __MLSS_Tianjian_H
#define __MLSS_Tianjian_H

#include "main.h"
#include "rs485.h"

float Getting_MLSS_Temp_B(void);
void Setting_MLSS_Temp_B(float valve);
void MLSS_Tianjian_rs485_GetModbusId(void);
void MLSS_Tianjian_rs485_GetValue(PtrToDOProbe ptd);
void MLSS_Tianjian_GetSN_Software(PtrToDOProbe ptd);
void MLSS_Tianjian_SetSHWVersion(PtrToDOProbe ptd,uint8_t* buff);
void MLSS_Tianjian_Get_Unit(PtrToDOProbe ptd);
void MLSS_Tianjian_Reset_Info(PtrToDOProbe ptd);
void MLSS_Tianjian_rs485_SetZero(PtrToDOProbe ptd, float Value);
void MLSS_Tianjian_rs485_Set_K(PtrToDOProbe ptd, float Value);
void MLSS_Tianjian_rs485_Set_Signal(PtrToDOProbe ptd, float Value, float Value2,uint16_t id);
void MLSS_Tianjian_rs485_Set_Signal_two(PtrToDOProbe ptd, float Value ,uint16_t id);
void MLSS_Tianjian_rs485_Set_Cal_Num(PtrToDOProbe ptd ,uint8_t num);
void MLSS_Tianjian_rs485_Set_Unit(PtrToDOProbe ptd ,uint8_t num);
void MLSS_Tianjian_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void MLSS_Tianjian_rs485_Get_Cal_Num(PtrToDOProbe ptd);
void MLSS_Tianjian_rs485_Set_Cal_Num1(PtrToDOProbe ptd,uint8_t* buff);

#endif
