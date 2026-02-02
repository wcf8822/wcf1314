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
	ZHONGXING,
	LIHE,
};


typedef float value_type;

/////////////////////////////////////////////////////////////////////////////////需要加设备编号
typedef struct struct_setting{
	uint8_t IsChn;            //是否显示中文
	uint8_t Is_pH_Group;            //pH的标液组选择
	uint8_t HaveGps;
	
	uint8_t customize;        //定制开机标志
	
	uint8_t contrast;         //开机对比度 默认对比度是浓的
 	uint8_t BattertType;    //电池类型   
	uint8_t KeyPadTone;       //是否有按键音
	uint8_t AlarmTone;
	
	uint8_t AutoLock_pH;       //是否自动锁定
	uint8_t AutoLock_DO;       //是否自动锁定
	uint8_t AutoLock_EC;       //是否自动锁定
	uint8_t AutoLock_Tur;      //是否自动锁定
	uint8_t AutoLock_ORP;      //是否自动锁定
	uint8_t AutoLock_NH4;      //是否自动锁定
	uint8_t AutoLock_Chl;      //是否自动锁定
	uint8_t AutoLock_Bga;      //是否自动锁定
	uint8_t AutoLock_COD;      //是否自动锁定
	uint8_t AutoLock_MLSS;      //是否自动锁定
	uint8_t AutoLock_OIW;      //是否自动锁定
	uint8_t AutoLock_TSS;      //是否自动锁定
	uint8_t AutoLock_SAL;      //是否自动锁定
	uint8_t AutoLock_OIW_ppm;      //是否自动锁定
	
	uint8_t AutoLock_level_pH;       //自动锁定等级
	uint8_t AutoLock_level_DO;       //自动锁定等级
	uint8_t AutoLock_level_EC;       //自动锁定等级
	uint8_t AutoLock_level_Tur;      //自动锁定等级
	uint8_t AutoLock_level_ORP;      //自动锁定等级
	uint8_t AutoLock_level_NH4;      //自动锁定等级
	uint8_t AutoLock_level_Chl;      //自动锁定等级
	uint8_t AutoLock_level_Bga;      //自动锁定等级
	uint8_t AutoLock_level_COD;    //自动锁定等级
	uint8_t AutoLock_level_MLSS;    //自动锁定等级
	uint8_t AutoLock_level_OIW;    //自动锁定等级
	uint8_t AutoLock_level_TSS;    //自动锁定等级
	uint8_t AutoLock_level_SAL;    //自动锁定等级
	uint8_t AutoLock_level_OIW_ppm;    //自动锁定等级
	
	uint8_t IsAlarm_pH;       //是否报警
	uint8_t IsAlarm_DO;       //是否报警
	uint8_t IsAlarm_EC;       //是否报警
	uint8_t IsAlarm_Tur;      //是否报警
	uint8_t IsAlarm_ORP;      //是否报警
	uint8_t IsAlarm_NH4;      //是否报警
	uint8_t IsAlarm_Chl;      //是否报警
	uint8_t IsAlarm_Bga;      //是否报警
	uint8_t IsAlarm_COD;    //是否报警
	uint8_t IsAlarm_MLSS;    //是否报警
	uint8_t IsAlarm_OIW;    //是否报警
	uint8_t IsAlarm_TSS;    //是否报警
	uint8_t IsAlarm_SAL;	//是否报警
	uint8_t IsAlarm_OIW_ppm;    //是否报警

	uint8_t AutoShut;         //0 5 10 20
	
	uint8_t IsOpen_SlideAvg_pH;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_DO;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_EC;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_Tur;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_ORP;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_NH4;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_Chl;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_Bga;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_COD;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_MLSS;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_OIW;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_OIW_ppm;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_TSS;  //是否开启滑动平均
	uint8_t IsOpen_SlideAvg_SAL;  //是否开启滑动平均
	
	uint8_t SlideAvgTimes_pH;    //滑动平均次数
	uint8_t SlideAvgTimes_DO;    //滑动平均次数
	uint8_t SlideAvgTimes_EC;    //滑动平均次数
	uint8_t SlideAvgTimes_Tur;    //滑动平均次数
	uint8_t SlideAvgTimes_ORP;    //滑动平均次数
	uint8_t SlideAvgTimes_NH4;    //滑动平均次数
	uint8_t SlideAvgTimes_Chl;    //滑动平均次数
	uint8_t SlideAvgTimes_Bga;    //滑动平均次数
	uint8_t SlideAvgTimes_COD;    //滑动平均次数
	uint8_t SlideAvgTimes_MLSS;	  //滑动平均次数
	uint8_t SlideAvgTimes_OIW;	  //滑动平均次数
	uint8_t SlideAvgTimes_OIW_ppm;	  //滑动平均次数
	uint8_t SlideAvgTimes_TSS;	  //滑动平均次数
	uint8_t SlideAvgTimes_SAL;	  //滑动平均次数

	uint8_t IsSelect_pH;       //是否选中加入到读取指令
	uint8_t IsSelect_DO;       //是否选中   海发、禹山溶解氧
	uint8_t IsSelect_DO_shenghui;       //昇辉溶解氧
	uint8_t IsSelect_EC;       //是否选中
	uint8_t IsSelect_Tur;      //是否选中
	uint8_t IsSelect_ORP;      //是否选中
	uint8_t IsSelect_NH4_DN02;      //自研氨氮
	uint8_t IsSelect_NH4_shenghui;      //昇辉氨氮
	uint8_t IsSelect_Chl;      //是否选中
	uint8_t IsSelect_Bga;      //是否选中
	uint8_t IsSelect_COD_DC18;    //自研  COD
	uint8_t IsSelect_COD_shenghui;    //昇辉 COD	
	
	value_type LowThreshold_pH;  //低门限报警阈值
	value_type HighThreshold_pH; //高门限报警阈值
	
	value_type LowThreshold_DO;  //低门限报警阈值
	value_type HighThreshold_DO; //高门限报警阈值
		
	value_type LowThreshold_EC;  //低门限报警阈值
	value_type HighThreshold_EC; //高门限报警阈值
	
	value_type LowThreshold_Tur;  //低门限报警阈值
	value_type HighThreshold_Tur; //高门限报警阈值
	
	value_type LowThreshold_ORP;  //低门限报警阈值
	value_type HighThreshold_ORP; //高门限报警阈值
	
	value_type LowThreshold_NH4;  //低门限报警阈值
	value_type HighThreshold_NH4; //高门限报警阈值
		
	value_type LowThreshold_Chl;  //低门限报警阈值
	value_type HighThreshold_Chl; //高门限报警阈值
	
	value_type LowThreshold_Bga;  //低门限报警阈值
	value_type HighThreshold_Bga; //高门限报警阈值
	
	value_type LowThreshold_COD;  //低门限报警阈值
	value_type HighThreshold_COD; //高门限报警阈值

	value_type LowThreshold_Mlss;  //低门限报警阈值
	value_type HighThreshold_Mlss; //高门限报警阈值

	value_type LowThreshold_OIW;  //低门限报警阈值
	value_type HighThreshold_OIW; //高门限报警阈值

	value_type LowThreshold_OIW_ppm;  //低门限报警阈值
	value_type HighThreshold_OIW_ppm; //高门限报警阈值

	value_type LowThreshold_TSS;  //低门限报警阈值
	value_type HighThreshold_TSS; //高门限报警阈值

	value_type LowThreshold_SAL;
	value_type HighThreshold_SAL; //高门限报警阈值

	value_type AirPressure;   //气压补偿
	value_type Salinity;      //盐度值

	uint8_t LockSave; // 锁定保存的使能
	uint16_t IntervalTime;  // 间隔保存时间	
	
	uint8_t KeyGps;        //GPS开关标志
	uint8_t TEMP_Unit;		
	uint8_t DC18_Set;		
	uint8_t SinMesTime;	   //单次测量时间
	uint8_t Instrument_model;	   //仪表型号(580或者702)
	uint8_t Orp_low_Threshold_pn;		//orp低门限报警阈值 正负号
	uint8_t Orp_high_Threshold_pn;		//orp高门限报警阈值 正负号
	uint8_t Sal_unit;		//0:ppt 5:百分号 10:千分号
	uint8_t Mes_Unit;	   //测量单位

	uint8_t wendujingdu;
	uint8_t guanji_flag;
}setting_t;

enum AUTOLOCK_ENUM{
	AUTOLOCK_OFF,     //关闭自动锁定
	AUTOLOCK_AUTO,    //自动锁定
	AUTOLOCK_MANUAL   //手动锁定
};



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


/*设置单次测量时间*/
void setting_Setdanciceliangtime(uint8_t time);
/*获取单次测量时间*/
uint8_t setting_Getdanciceliangtime(void);

void setting_Set_Temp_Unit(uint8_t Unit);
/*获取温度单位设置*/
uint8_t setting_Get_Temp_Unit(void);

void setting_SetIsOpen_SlideAvg_DO(uint8_t IsOpen); //设置是否开启滑动平均
uint8_t setting_GetIsOpen_SlideAvg_DO(void);        //设置滑动平均值
/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_pH(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_pH(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_FCL(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_FCL(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_EC(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_EC(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_Tur(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_Tur(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_ORP(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_ORP(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_NH4(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_NH4(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_F(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_F(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_CL(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_CL(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_Chl(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_Chl(void);


/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_Bga(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_Bga(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_COD(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_COD(void);

void setting_SetIsOpen_SlideAvg_MLSS(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_MLSS(void);

uint8_t setting_GetSlideAvgTimes_OIW(void);
void setting_SetSlideAvgTimes_OIW(uint8_t times);

uint8_t setting_GetSlideAvgTimes_TSS(void);
void setting_SetSlideAvgTimes_TSS(uint8_t times);

uint8_t setting_GetSlideAvgTimes_SAL(void);
void setting_SetSlideAvgTimes_SAL(uint8_t times);
/*滑动平均次数设置*/
uint8_t setting_GetSlideAvgTimes_DO(void);
void setting_SetSlideAvgTimes_DO(uint8_t times);

uint8_t setting_GetSlideAvgTimes_pH(void);
void setting_SetSlideAvgTimes_pH(uint8_t times);

uint8_t setting_GetSlideAvgTimes_FCL(void);
void setting_SetSlideAvgTimes_FCL(uint8_t times);

uint8_t setting_GetSlideAvgTimes_EC(void);
void setting_SetSlideAvgTimes_EC(uint8_t times);

uint8_t setting_GetSlideAvgTimes_Tur(void);
void setting_SetSlideAvgTimes_Tur(uint8_t times);


uint8_t setting_GetSlideAvgTimes_ORP(void);
void setting_SetSlideAvgTimes_ORP(uint8_t times);

uint8_t setting_GetSlideAvgTimes_NH4(void);
void setting_SetSlideAvgTimes_NH4(uint8_t times);

uint8_t setting_GetSlideAvgTimes_F(void);
void setting_SetSlideAvgTimes_F(uint8_t times);

uint8_t setting_GetSlideAvgTimes_CL(void);
void setting_SetSlideAvgTimes_CL(uint8_t times);

uint8_t setting_GetSlideAvgTimes_Chl(void);
void setting_SetSlideAvgTimes_Chl(uint8_t times);

uint8_t setting_GetSlideAvgTimes_Bga(void);
void setting_SetSlideAvgTimes_Bga(uint8_t times);

/*滑动平均次数设置*/
uint8_t setting_GetSlideAvgTimes_COD(void);
void setting_SetSlideAvgTimes_COD(uint8_t times);

uint8_t setting_GetSlideAvgTimes_MLSS(void);
void setting_SetSlideAvgTimes_MLSS(uint8_t times);


uint8_t setting_GetOrp_low_Threshold_pn(void);
void setting_SetOrp_low_Threshold_pn(uint8_t value);

uint8_t setting_GetOrp_high_Threshold_pn(void);
void setting_SetOrp_high_Threshold_pn(uint8_t value);

uint8_t setting_GET_SHUT_FLAG(void);
void setting_SET_SHUT_FLAG(uint8_t value);



/*获取和设置语言设置*/
void setting_SetIsChn(uint8_t IsChn);
uint8_t setting_GetIsChn(void);


/*设置pH 的标液组*/
void setting_SetIs_pH_Group(uint8_t Is_pH_Group);
/*获取中英文设置*/
uint8_t setting_GetIs_pH_Group(void);


uint8_t setting_GetTemp_jingdu(void);
void setting_SetTemp_jingdu(uint8_t value);



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
/*设置和读取是否有gps*/
uint8_t setting_GetHaveGps(void);
void setting_SetHaveGps(uint8_t have);
/*设置和读取对比度设置*/
uint8_t setting_GetContrast(void);
void setting_SetContrast(uint8_t contrast);

/*设置和读取电池类型*/
uint8_t setting_GetBattertType(void);
void setting_SetBattertType(uint8_t BattertType);


/*自动锁定设置*/

uint8_t setting_AutoLock_num(void);
void setting_SetAutoLock_num(uint8_t value);


uint8_t setting_GetAutoLock_DO(void);
void setting_SetAutoLock_DO(uint8_t AutoLock);
uint8_t setting_GetAutoLock_pH(void);
void setting_SetAutoLock_pH(uint8_t AutoLock);
uint8_t setting_GetAutoLock_Tur(void);
void setting_SetAutoLock_Tur(uint8_t AutoLock);
uint8_t setting_GetAutoLock_FCL(void);
void setting_SetAutoLock_FCL(uint8_t AutoLock);
uint8_t setting_GetAutoLock_EC(void);
void setting_SetAutoLock_EC(uint8_t AutoLock);
uint8_t setting_GetAutoLock_ORP(void);
void setting_SetAutoLock_ORP(uint8_t AutoLock);
uint8_t setting_GetAutoLock_NH4(void);
void setting_SetAutoLock_NH4(uint8_t AutoLock);
uint8_t setting_GetAutoLock_F(void);
void setting_SetAutoLock_F(uint8_t AutoLock);
uint8_t setting_GetAutoLock_CL(void);
void setting_SetAutoLock_CL(uint8_t AutoLock);
uint8_t setting_GetAutoLock_Chl(void);
void setting_SetAutoLock_Chl(uint8_t AutoLock);
uint8_t setting_GetAutoLock_Bga(void);
void setting_SetAutoLock_Bga(uint8_t AutoLock);
uint8_t setting_GetAutoLock_COD(void);
void setting_SetAutoLock_COD(uint8_t AutoLock);
uint8_t setting_GetAutoLock_MLSS(void);
void setting_SetAutoLock_MLSS(uint8_t AutoLock);
uint8_t setting_GetAutoLock_OIW(void);
void setting_SetAutoLock_OIW(uint8_t AutoLock);
uint8_t setting_GetAutoLock_TSS(void);
void setting_SetAutoLock_TSS(uint8_t AutoLock);
uint8_t setting_GetAutoLock_TSS(void);
void setting_SetAutoLock_TSS(uint8_t AutoLock);
uint8_t setting_GetAutoLock_SAL(void);
void setting_SetAutoLock_SAL(uint8_t AutoLock);


uint8_t setting_GetAutoLockLevel_DO(void);
void setting_SetAutoLockLevel_DO(uint8_t level);
uint8_t setting_GetAutoLockLevel_pH(void);
void setting_SetAutoLockLevel_pH(uint8_t level);
uint8_t setting_GetAutoLockLevel_Tur(void);
void setting_SetAutoLockLevel_Tur(uint8_t level);
uint8_t setting_GetAutoLockLevel_FCL(void);
void setting_SetAutoLockLevel_FCL(uint8_t level);
uint8_t setting_GetAutoLockLevel_EC(void);
void setting_SetAutoLockLevel_EC(uint8_t level);
uint8_t setting_GetAutoLockLevel_ORP(void);
void setting_SetAutoLockLevel_ORP(uint8_t level);
uint8_t setting_GetAutoLockLevel_NH4(void);
void setting_SetAutoLockLevel_NH4(uint8_t level);
uint8_t setting_GetAutoLockLevel_F(void);
void setting_SetAutoLockLevel_F(uint8_t level);
uint8_t setting_GetAutoLockLevel_CL(void);
void setting_SetAutoLockLevel_CL(uint8_t level);
uint8_t setting_GetAutoLockLevel_Chl(void);
void setting_SetAutoLockLevel_Chl(uint8_t level);
uint8_t setting_GetAutoLockLevel_Bga(void);
void setting_SetAutoLockLevel_Bga(uint8_t level);
uint8_t setting_GetAutoLockLevel_COD(void);
void setting_SetAutoLockLevel_COD(uint8_t level);
uint8_t setting_GetAutoLockLevel_MLSS(void);
void setting_SetAutoLockLevel_MLSS(uint8_t level);
uint8_t setting_GetAutoLockLevel_OIW(void);
void setting_SetAutoLockLevel_OIW(uint8_t level);
uint8_t setting_GetAutoLockLevel_TSS(void);
void setting_SetAutoLockLevel_TSS(uint8_t level);
uint8_t setting_GetAutoLockLevel_SAL(void);
void setting_SetAutoLockLevel_SAL(uint8_t level);
/*设置、获取是否选中*/
/*获取是否选中*/
uint8_t setting_GetIsSelect_DO(void);
void setting_SetIsSelect_DO(uint8_t sSelect)	;
void setting_SetIsSelect_DO_shenghui(uint8_t sSelect);
uint8_t setting_GetIsSelect_DO_shenghui(void);

uint8_t setting_GetIsSelect_pH(void);
void setting_SetIsSelect_pH(uint8_t sSelect);

uint8_t setting_GetIsSelect_Tur(void);
void setting_SetIsSelect_Tur(uint8_t sSelect);

uint8_t setting_GetIsSelect_FCL(void);
void setting_SetIsSelect_FCL(uint8_t sSelect);

uint8_t setting_GetIsSelect_EC(void);
void setting_SetIsSelect_EC(uint8_t sSelect);

uint8_t setting_GetIsSelect_ORP(void);
void setting_SetIsSelect_ORP(uint8_t sSelect);

void setting_SetIsSelect_NH4_DN02(uint8_t sSelect);
uint8_t setting_GetIsSelect_NH4_DN02(void);
void setting_SetIsSelect_NH4_shenghui(uint8_t sSelect);
uint8_t setting_GetIsSelect_NH4_shenghui(void);

void setting_SetIsSelect_F(uint8_t sSelect);
uint8_t setting_GetIsSelect_F(void);

void setting_SetIsSelect_CL(uint8_t sSelect);
uint8_t setting_GetIsSelect_CL(void);

void setting_SetIsSelect_Chl(uint8_t sSelect);
uint8_t setting_GetIsSelect_Chl(void);

void setting_SetIsSelect_Bga(uint8_t sSelect);
uint8_t setting_GetIsSelect_Bga(void);

void setting_SetIsSelect_COD_DC18(uint8_t sSelect);
uint8_t setting_GetIsSelect_COD_DC18(void);
void setting_SetIsSelect_COD_shenghui(uint8_t sSelect);
uint8_t setting_GetIsSelect_COD_shenghui(void);


//报警使能
void setting_SetIsAlarm_DO(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_DO(void);
void setting_SetIsAlarm_pH(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_pH(void);
void setting_SetIsAlarm_Tur(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_Tur(void);
void setting_SetIsAlarm_FCL(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_FCL(void);
void setting_SetIsAlarm_EC(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_EC(void);
void setting_SetIsAlarm_ORP(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_ORP(void);
void setting_SetIsAlarm_NH4(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_NH4(void);
void setting_SetIsAlarm_F(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_F(void);
void setting_SetIsAlarm_CL(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_CL(void);
void setting_SetIsAlarm_Chl(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_Chl(void);
void setting_SetIsAlarm_Bga(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_Bga(void);
void setting_SetIsAlarm_COD(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_COD(void);

void setting_SetIsAlarm_MLSS(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_MLSS(void);

void setting_SetIsAlarm_OIW(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_OIW(void);

void setting_SetIsAlarm_TSS(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_TSS(void);

void setting_SetIsAlarm_SAL(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_SAL(void);

//报警上下限
value_type setting_GetHighThreshold_pH(void);
void setting_SetHighThreshold_pH(value_type value);
value_type setting_GetLowThreshold_pH(void);
void setting_SetLowThreshold_pH(value_type value);
value_type setting_GetHighThreshold_FCL(void);
void setting_SetHighThreshold_FCL(value_type value);
value_type setting_GetLowThreshold_FCL(void);
void setting_SetLowThreshold_FCL(value_type value);
value_type setting_GetHighThreshold_EC(void);
void setting_SetHighThreshold_EC(value_type value);
value_type setting_GetLowThreshold_EC(void);
void setting_SetLowThreshold_EC(value_type value);
value_type setting_GetHighThreshold_Tur(void);
void setting_SetHighThreshold_Tur(value_type value);
value_type setting_GetLowThreshold_Tur(void);
void setting_SetLowThreshold_Tur(value_type value);
value_type setting_GetHighThreshold_ORP(void);
void setting_SetHighThreshold_ORP(value_type value);
value_type setting_GetLowThreshold_ORP(void);
void setting_SetLowThreshold_ORP(value_type value);
value_type setting_GetHighThreshold_NH4(void);
void setting_SetHighThreshold_NH4(value_type value);
value_type setting_GetLowThreshold_NH4(void);
void setting_SetLowThreshold_NH4(value_type value);
value_type setting_GetHighThreshold_F(void);
void setting_SetHighThreshold_F(value_type value);
value_type setting_GetLowThreshold_F(void);
void setting_SetLowThreshold_F(value_type value);
value_type setting_GetHighThreshold_CL(void);
void setting_SetHighThreshold_CL(value_type value);
value_type setting_GetLowThreshold_CL(void);
void setting_SetLowThreshold_CL(value_type value);
value_type setting_GetHighThreshold_Chl(void);
void setting_SetHighThreshold_Chl(value_type value);
value_type setting_GetLowThreshold_Chl(void);
void setting_SetLowThreshold_Chl(value_type value);
value_type setting_GetHighThreshold_Bga(void);
void setting_SetHighThreshold_Bga(value_type value);
value_type setting_GetLowThreshold_Bga(void);
void setting_SetLowThreshold_Bga(value_type value);
value_type setting_GetHighThreshold_COD(void);
void setting_SetHighThreshold_COD(value_type value);
value_type setting_GetLowThreshold_COD(void);
void setting_SetLowThreshold_COD(value_type value);
value_type setting_GetHighThreshold_MLSS(void);
void setting_SetHighThreshold_MLSS(value_type value);
value_type setting_GetLowThreshold_MLSS(void);
void setting_SetLowThreshold_MLSS(value_type value);
value_type setting_GetHighThreshold_OIW(void);
void setting_SetHighThreshold_OIW(value_type value);
value_type setting_GetLowThreshold_OIW(void);
void setting_SetLowThreshold_OIW(value_type value);

value_type setting_GetHighThreshold_TSS(void);
void setting_SetHighThreshold_TSS(value_type value);

value_type setting_GetLowThreshold_TSS(void);
void setting_SetLowThreshold_TSS(value_type value);

value_type setting_GetHighThreshold_SAL(void);
void setting_SetHighThreshold_SAL(value_type value);
value_type setting_GetLowThreshold_SAL(void);
void setting_SetLowThreshold_SAL(value_type value);

void setting_SetIsOpen_SlideAvg_OIW(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_OIW(void);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_TSS(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_TSS(void);
/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_SAL(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_SAL(void);

void setting_SetKeyGPS(uint8_t KeyGps);
uint8_t setting_GetIsKeyGps(void);




//设置间隔保存时间
uint16_t setting_GetAutoIntervalTime(void);
void setting_SetAutoIntervalTime(uint16_t time);
//设置锁定保存开关
void setting_SetLockSave(uint8_t KeyPadTone);
uint8_t setting_GetLockSave(void);

//设置产品型号
void setting_Set_Type(uint8_t model);
uint8_t setting_Get_Type(void);

void setting_SetKey_DC18_Set(uint8_t KeyGps);
uint8_t setting_GetIsKey_DC18_Set(void);
	

/*设置盐度单位*/
void setting_SetSal_Unit(uint8_t value);
/*获取盐度单位*/
uint8_t setting_GetSal_Uni(void);

uint8_t setting_GetMesUnit(void);
void setting_SetMesUnit(uint8_t value);


void setting_SetIsAlarm_OIW_ppm(uint8_t IsAlarm);
uint8_t setting_GetIsAlarm_OIW_ppm(void);


value_type setting_GetHighThreshold_OIW_ppm(void);
void setting_SetHighThreshold_OIW_ppm(value_type value);
value_type setting_GetLowThreshold_OIW_ppm(void);
void setting_SetLowThreshold_OIW_ppm(value_type value);

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_OIW_ppm(uint8_t IsOpen);
uint8_t setting_GetIsOpen_SlideAvg_OIW_ppm(void);


uint8_t setting_GetSlideAvgTimes_OIW_ppm(void);
void setting_SetSlideAvgTimes_OIW_ppm(uint8_t times);

uint8_t setting_GetAutoLock_OIW_ppm(void);
void setting_SetAutoLock_OIW_ppm(uint8_t AutoLock);

uint8_t setting_GetAutoLockLevel_OIW_ppm(void);
void setting_SetAutoLockLevel_OIW_ppm(uint8_t level);

#endif
