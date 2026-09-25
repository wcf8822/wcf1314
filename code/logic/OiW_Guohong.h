#ifndef __OiW_Guohong_H
#define __OiW_Guohong_H

#include "main.h"
#include "rs485.h"

void OiW_Guohong_Get_Modbus_ID(void);
void OiW_Guohong_rs485_GetValue(PtrToDOProbe ptd);
void OiW_Guohong_rs485_GetSN(PtrToDOProbe ptd);
void OiW_Guohong_rs485_GetSHWVersion(PtrToDOProbe ptd);
void OiW_Guohong_rs485_Get_Status(PtrToDOProbe ptd);
void OiW_Guohong_rs485_Cal(PtrToDOProbe ptd,float Value,uint8_t Cal_num);
void OiW_Guohong_rs485_Set_Temp_Model(PtrToDOProbe ptd,uint8_t model);
void OiW_Guohong_rs485_Set_Filter_Model(PtrToDOProbe ptd,uint8_t model);
void OiW_Guohong_rs485_Set_Clear_Model(PtrToDOProbe ptd,uint16_t model);
void OiW_Guohong_rs485_Set_Mes_Model(PtrToDOProbe ptd,uint8_t model);
void OiW_Guohong_rs485_Set_Temp_k_Model(PtrToDOProbe ptd,float Value);
void OiW_Guohong_rs485_SetSN(PtrToDOProbe ptd,uint8_t *dat);
void OiW_Guohong_rs485_SetSHWVersion(PtrToDOProbe ptd,uint8_t *dat);
void OiW_Guohong_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void OiW_Guohong_rs485_Get_Cal_value(PtrToDOProbe ptd);
void OiW_Guohong_rs485_Set_Cal_info(PtrToDOProbe ptd,uint8_t *dat);
void OiW_Guohong_rs485_Set_OIW_B(PtrToDOProbe ptd,float Value);
void OiW_Guohong_rs485_Set_OIW_K(PtrToDOProbe ptd,float Value);
void OiW_Guohong_rs485_Set_Clear_move(PtrToDOProbe ptd,uint8_t model);
void OiW_Guohong_rs485_Set_Zero_Cal(PtrToDOProbe ptd);
void OiW_Guohong_rs485_Set_OIW_K_Reset(PtrToDOProbe ptd,float Value);
void OiW_Guohong_rs485_Set_OIW_B_Reset(PtrToDOProbe ptd,float Value);


//全局变量
uint16_t Get_OIW_Mes_Mode(void);
void Set_OIW_Mes_Mode(uint16_t value);
uint16_t Get_OIW_Clear_Time(void);
void Set_OIW_Clear_Time(uint16_t value);
uint16_t Get_OIW_Lvbo(void);
void Set_OIW_Lvbo(uint16_t value);
uint16_t Get_OIW_Temp_mode(void);
void Set_OIW_Temp_mode(uint16_t value);
float Get_OIW_Temp_value(void);
void Set_OIW_Temp_value(float value);


#endif
