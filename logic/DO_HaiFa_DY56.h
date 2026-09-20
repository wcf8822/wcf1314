#ifndef __DO_HaiFa_DY56_H
#define __DO_HaiFa_DY56_H

#include "main.h"
#include "rs485.h"
void DO_HaiFa_DY56_rs485_GetModbusId(void);
void DO_HaiFa_DY56_rs485_GetValue(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_SetSN_SH(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_Set_Cmd_open(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_Set_Cmd_close(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_Set_Sal(PtrToDOProbe ptd,int16_t value);
void DO_HaiFa_DY56_rs485_Set_Press(PtrToDOProbe ptd,int16_t value);
void DO_HaiFa_DY56_rs485_Set_Temp_Cal(PtrToDOProbe ptd,int16_t value);
void DO_HaiFa_DY56_rs485_ClearCal(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_Set_Cal(PtrToDOProbe ptd,uint8_t value);
void DO_HaiFa_DY56_rs485_Set_Break(PtrToDOProbe ptd);
void DO_HaiFa_DY56_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void DO_HaiFa_DY56_rs485_GetSalinity(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_GetPressure(PtrToDOProbe ptd);
void DO_HaiFa_DY56_rs485_Get_Temp_Cal(PtrToDOProbe ptd);
void DO_HaiFa_DY56_SetTemp_Cal_value(PtrToDOProbe ptd, uint8_t *dat);
void DO_HaiFa_DY56_SetSalinityArr(PtrToDOProbe ptd, uint8_t *dat);
void DO_HaiFa_DY56_SetPressureArr(PtrToDOProbe ptd, uint8_t *dat);

uint8_t Get_DY56_Set_Sal(void);
int16_t Get_DY56_Set_Sal_Value(void);
uint8_t Get_DY56_Set_Press(void);
int16_t Get_DY56_Set_PressValue(void);
uint8_t Get_DY56_Set_Temp_Cal(void);
int16_t Get_DY56_Set_Temp_Cal_Value(void);
uint8_t Get_DY56_Temp_Cal_Pending(void);
void Clear_DY56_Temp_Cal_Pending(void);
float Get_DY56_Temp_Cal_Std(void);
uint8_t Get_DY56_Set_Zero_Cal(void);
int16_t Get_DY56_Set_Zero_Cal_Value(void);
uint8_t Get_DY56_Set_Full_Cal(void);
int16_t Get_DY56_Set_Full_Cal_Value(void);
uint8_t Get_DY56_ClearCal_Flag(void);
void Set_DY56_Sal_Flag(uint8_t value);
void Set_DY56_Press_Flag(uint8_t value);
void Set_DY56_Temp_Cal_Flag(uint8_t value);
void Set_DY56_Zero_Flag(uint8_t value);
void Set_DY56_Full_Flag(uint8_t value);
void Set_DY56_Sal_Value(int16_t value);
void Set_DY56_Press_Value(int16_t value);
void Set_DY56_Temp_Cal_Value(int16_t value);
void Set_DY56_Zero_Value(int16_t value);
void Set_DY56_Full_Value(int16_t value);
void Set_DY56_ClearCal_Flag(uint8_t value);

#endif
