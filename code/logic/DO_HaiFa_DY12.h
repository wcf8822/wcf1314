#ifndef __DO_HaiFa_DY12_H
#define __DO_HaiFa_DY12_H

#include "main.h"
#include "rs485.h"
void DO_HaiFa_DY12_rs485_GetModbusId(void);
void DO_HaiFa_DY12_rs485_GetValue(PtrToDOProbe ptd);
void DO_HaiFa_DY12_rs485_SetSN_SH(PtrToDOProbe ptd);
void DO_HaiFa_DY12_rs485_Set_Cmd_open(PtrToDOProbe ptd);
void DO_HaiFa_DY12_rs485_Set_Cmd_close(PtrToDOProbe ptd);
void DO_HaiFa_DY12_rs485_Set_Sal(PtrToDOProbe ptd,int16_t value);
void DO_HaiFa_DY12_rs485_Set_Press(PtrToDOProbe ptd,int16_t value);
void DO_HaiFa_DY12_rs485_ClearCal(PtrToDOProbe ptd);
void DO_HaiFa_DY12_rs485_Set_Cal(PtrToDOProbe ptd,uint8_t value);
void DO_HaiFa_DY12_rs485_Set_Break(PtrToDOProbe ptd);
void DO_HaiFa_DY12_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);
void DO_HaiFa_DY12_rs485_Get_Sal_Press(PtrToDOProbe ptd);

uint8_t Get_DY12_Set_Sal(void);
int16_t Get_DY12_Set_Sal_Value(void);
uint8_t Get_DY12_Set_Press(void);
int16_t Get_DY12_Set_PressValue(void);
uint8_t Get_DY12_Set_Zero_Cal(void);
int16_t Get_DY12_Set_Zero_Cal_Value(void);
uint8_t Get_DY12_Set_Full_Cal(void);
int16_t Get_DY12_Set_Full_Cal_Value(void);
void Set_DY12_Sal_Flag(uint8_t value);
void Set_DY12_Press_Flag(uint8_t value);
void Set_DY12_Zero_Flag(uint8_t value);
void Set_DY12_Full_Flag(uint8_t value);
void Set_DY12_Sal_Value(int16_t value);
void Set_DY12_Press_Value(int16_t value);
void Set_DY12_Zero_Value(int16_t value);
void Set_DY12_Full_Value(int16_t value);

#endif
