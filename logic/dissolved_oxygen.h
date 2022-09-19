#ifndef __DISSOLVED_OXYGEN_H
#define __DISSOLVED_OXYGEN_H

//dissolved_oxygen


#include "main.h"
#include "rs485.h"


typedef struct{
	uint8_t k0[4];
	uint8_t k1[4];
	uint8_t k2[4];
	uint8_t k3[4];
	uint8_t k4[4];
	uint8_t k5[4];
	uint8_t k6[4];
	uint8_t k7[4];
}SensorCap_t;

/*当前设备相关*/
void DO_SetCurDO(uint8_t ModbusId, PtrToDOProbe DO_head);
PtrToDOProbe get_CurDo(void);
void DO_ClearCueDO(void);

/*设备链表相关*/
void DO_AddProbe(uint8_t ModbusId, PtrToDOProbe *DO_head);   //添加溶解氧设备
void DO_DelProbe(uint8_t ModbusId, PtrToDOProbe *DO_head);   //删除设备
PtrToDOProbe DO_FindByName(uint8_t* name, PtrToDOProbe *DO_head);
uint8_t DO_GetIsInit(PtrToDOProbe ptd);
void DO_SetIsInit(PtrToDOProbe ptd);
uint8_t DO_GetIsGetedValue(PtrToDOProbe ptd);
void DO_SetIsGetedValue(PtrToDOProbe ptd);

void DO_UpdatePressSal(PtrToDOProbe *DO_head);

/*设备携带信息*/
void DO_rs485_GetModbusId(void);
void DO_rs485_GetSN(PtrToDOProbe ptd);
void DO_rs485_GetSHWVersion(PtrToDOProbe ptd);
void DO_rs485_GetKB(PtrToDOProbe ptd);
void DO_rs485_GetSalinity(PtrToDOProbe ptd);//获取盐度数据
void DO_rs485_GetPressure(PtrToDOProbe ptd);//获取气压数据


/*测量获取数据*/
void DO_rs485_Start(PtrToDOProbe ptd);
void DO_rs485_Stop(PtrToDOProbe ptd);
void DO_rs485_GetTempTwoDO(PtrToDOProbe ptd);
void DO_rs485_GetTemperature(PtrToDOProbe ptd);
void DO_rs485_GetDOPercent(PtrToDOProbe ptd);
void DO_rs485_GetDOmgL(PtrToDOProbe ptd);

/*测量设置数据*/
void DO_rs485_SetKB(PtrToDOProbe ptd, float k, float b);
void DO_rs485_SetK(PtrToDOProbe ptd, float k);
void DO_rs485_SetB(PtrToDOProbe ptd, float b);
void DO_rs485_SetSalinity(PtrToDOProbe ptd, float sal);
void DO_rs485_SetPressure(PtrToDOProbe ptd, float press);
void DO_rs485_SetAddr(PtrToDOProbe ptd, uint8_t NewId);

/*获取do值*/
float DO_GetKFloat(PtrToDOProbe ptd);
float DO_GetBFloat(PtrToDOProbe ptd);
float DO_GetPressure(PtrToDOProbe ptd);
float DO_GetSalinity(PtrToDOProbe ptd);
float DO_GetDOPercent(PtrToDOProbe ptd);
float DO_GetDOmgL(PtrToDOProbe ptd);
float DO_GetTemperature(PtrToDOProbe ptd);

/*从rxbuf中获取do数据*/
void DO_SetSN(PtrToDOProbe ptd, uint8_t* buff, uint8_t len);
void DO_SetKBArr(PtrToDOProbe ptd, uint8_t* data);
void DO_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data);
void DO_SetPressureArr(PtrToDOProbe ptd, uint8_t* dat);
void DO_SetSalinityArr(PtrToDOProbe ptd, uint8_t* dat);
void DO_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat);

/*额外功能*/
uint8_t DO_ValueCheckFirst(PtrToDOProbe ptd, float data);
void DO_SetTempZero(void);
uint8_t DO_GetValueLocked(PtrToDOProbe ptd); //获取do是否被锁住
void DO_SetValueLocked(PtrToDOProbe ptd);    //直接锁住
void DO_SetValueUnlocked(PtrToDOProbe ptd);  //解锁
void clear_DOShakeCount(void);

void DO_zero_buf_mgl(PtrToDOProbe p);
void DO_zero_buf_percent(PtrToDOProbe p);

#endif













