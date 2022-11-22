#ifndef __SETTING_H
#define __SETTING_H

#include "stdint.h"
#include "main.h"

/*
1% = 10000ppm = 10000mg/L
ppm = mg/L
*/

/*自动锁定的阈值*/
extern float AutoLock_value[3];

enum customize_enum
{
	LUHENG,
	HENGLAN,
	ZHONGXING
};


typedef float value_type;

/////////////////////////////////////////////////////////////////////////////////需要加设备编号
typedef struct struct_setting{
	uint8_t IsChn;            //是否显示中文
	uint8_t HaveGps;
	
	uint8_t customize;        //定制开机标志
	
	uint8_t KeyPadTone;       //是否有按键音
	uint8_t AlarmTone;
	
	uint8_t AutoLock_pH;       //是否自动锁定
	uint8_t AutoLock_DO;       //是否自动锁定
	uint8_t AutoLock_FCL;      //是否自动锁定
	uint8_t AutoLock_EC;       //是否自动锁定
	uint8_t AutoLock_Tur;      //是否自动锁定
	uint8_t AutoLock_ORP;      //是否自动锁定
	uint8_t AutoLock_NH4;      //是否自动锁定
	uint8_t AutoLock_F;        //是否自动锁定
	uint8_t AutoLock_CL;       //是否自动锁定
	uint8_t AutoLock_Chl;      //是否自动锁定
	uint8_t AutoLock_Bga;      //是否自动锁定
	uint8_t AutoLock_CODuv;    //是否自动锁定
	
	uint8_t AutoLock_level_pH;       //自动锁定等级
	uint8_t AutoLock_level_DO;       //自动锁定等级
	uint8_t AutoLock_level_FCL;      //自动锁定等级
	uint8_t AutoLock_level_EC;       //自动锁定等级
	uint8_t AutoLock_level_Tur;      //自动锁定等级
	uint8_t AutoLock_level_ORP;      //自动锁定等级
	uint8_t AutoLock_level_NH4;      //自动锁定等级
	uint8_t AutoLock_level_F;        //自动锁定等级
	uint8_t AutoLock_level_CL;       //自动锁定等级
	uint8_t AutoLock_level_Chl;      //自动锁定等级
	uint8_t AutoLock_level_Bga;      //自动锁定等级
	uint8_t AutoLock_level_CODuv;    //自动锁定等级
	
	uint8_t IsAlarm_pH;       //是否报警
	uint8_t IsAlarm_DO;       //是否报警
	uint8_t IsAlarm_FCL;      //是否报警
	uint8_t IsAlarm_EC;       //是否报警
	uint8_t IsAlarm_Tur;      //是否报警
	uint8_t IsAlarm_ORP;      //是否报警
	uint8_t IsAlarm_NH4;      //是否报警
	uint8_t IsAlarm_F;        //是否报警
	uint8_t IsAlarm_CL;       //是否报警
	uint8_t IsAlarm_Chl;      //是否报警
	uint8_t IsAlarm_Bga;      //是否报警
	uint8_t IsAlarm_CODuv;    //是否报警
	
	uint8_t AutoShut;         //0 5 10 20
	
	uint8_t IsOpen_SlideAvg;  //是否开启滑动平均
	uint8_t SlideAvgTimes;    //滑动平均次数
	
	value_type LowThreshold_pH;  //低门限报警阈值
	value_type HighThreshold_pH; //高门限报警阈值
	
	value_type LowThreshold_DO;  //低门限报警阈值
	value_type HighThreshold_DO; //高门限报警阈值
	
	value_type LowThreshold_FCL;  //低门限报警阈值
	value_type HighThreshold_FCL; //高门限报警阈值
	
	value_type LowThreshold_EC;  //低门限报警阈值
	value_type HighThreshold_EC; //高门限报警阈值
	
	value_type LowThreshold_Tur;  //低门限报警阈值
	value_type HighThreshold_Tur; //高门限报警阈值
	
	value_type LowThreshold_ORP;  //低门限报警阈值
	value_type HighThreshold_ORP; //高门限报警阈值
	
	value_type LowThreshold_NH4;  //低门限报警阈值
	value_type HighThreshold_NH4; //高门限报警阈值
	
	value_type LowThreshold_F;  //低门限报警阈值
	value_type HighThreshold_F; //高门限报警阈值
	
	value_type LowThreshold_CL;  //低门限报警阈值
	value_type HighThreshold_CL; //高门限报警阈值
	
	value_type LowThreshold_Chl;  //低门限报警阈值
	value_type HighThreshold_Chl; //高门限报警阈值
	
	value_type LowThreshold_Bga;  //低门限报警阈值
	value_type HighThreshold_Bga; //高门限报警阈值
	
	value_type LowThreshold_CODuv;  //低门限报警阈值
	value_type HighThreshold_CODuv; //高门限报警阈值
	
	value_type AirPressure;   //气压补偿
	value_type Salinity;      //盐度值
	
	
}setting_t;

enum AUTOLOCK_ENUM{
	AUTOLOCK_OFF,     //关闭自动锁定
	AUTOLOCK_AUTO,    //自动锁定
	AUTOLOCK_MANUAL   //手动锁定
};

//#define SETTING_START_ADDR    (LOG_COUNT_ADDR-(sizeof(setting_t))) //初始化设置起始地址

typedef union {
	setting_t setting_struct;
	uint8_t setting_arr[sizeof(setting_t)];
}setting_union;

/*获取和设置按键音*/
void setting_SetKeyPadTone(uint8_t KeyPadTone);
uint8_t setting_GetKeyPadTone(void);

/*获取和设置报警音*/
void setting_SetAlarmTone(uint8_t AlarmTone);
uint8_t setting_GetAlarmTone(void);

/*设置报警*/
void setting_SetIsAlarm_DO(uint8_t IsAlarm);

/*获取报警设置*/
uint8_t setting_GetIsAlarm_DO(void);


void setting_SetIsOpen_SlideAvg(uint8_t IsOpen); //设置是否开启滑动平均
uint8_t setting_GetIsOpen_SlideAvg(void);        //设置滑动平均值

uint8_t setting_GetSlideAvgTimes(void);
void setting_SetSlideAvgTimes(uint8_t times);


/*获取和设置语言设置*/
void setting_SetIsChn(uint8_t IsChn);
uint8_t setting_GetIsChn(void);

/*获取定制设置*/
uint8_t setting_GetLogo(void);

/*从flash获取设置*/
void FlashToSetting(void);
/*将设置写入flash*/
void SettingToFlash(void);

/*获取和设置大气压补偿*/
value_type setting_GetAirCompensate(void);
void setting_SetAirCompensate(value_type value);

/*获取和设置盐度值*/
value_type setting_GetSalinity(void);
void setting_SetSalinity(value_type value);

/*初始化flash中的设置并刷新现有设置*/
void setting_reset(void);

/*第一次启动写入第一位和初始化设置*/
void first_write(void);

/*初始化设置*/
void init_setting(void);

/*获取和设置高低门限报警阈值*/
value_type setting_GetHighThreshold_DO(void);
void setting_SetHighThreshold_DO(value_type value);
value_type setting_GetLowThreshold_DO(void);
void setting_SetLowThreshold_DO(value_type value);


/*获取和设置自动关机时间*/
uint8_t setting_GetAutoShut(void);
void setting_SetAutoShut(uint8_t value);

uint8_t setting_GetHaveGps(void);
void setting_SetHaveGps(uint8_t have);

/*自动锁定设置*/

uint8_t setting_GetAutoLock_DO(void);
void setting_SetAutoLock_DO(uint8_t AutoLock);
uint8_t setting_GetAutoLockLevel_DO(void);
void setting_SetAutoLockLevel_DO(uint8_t level);

#endif
