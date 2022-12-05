//用来保存和读取设置参数的
#include "setting.h"
#include "spi_flash.h"
#include <string.h>
#include "rtc.h"

STATIC setting_union setting;



/*设置中英文*/
void setting_SetIsChn(uint8_t IsChn)
{
	setting.setting_struct.IsChn = IsChn;
}
/*获取中英文设置*/
uint8_t setting_GetIsChn(void)
{
	return setting.setting_struct.IsChn;
}

/*获取定制类型(开机界面)*/
uint8_t setting_GetLogo(void)
{
	return setting.setting_struct.customize;
}

/*设置按键音开关*/
void setting_SetKeyPadTone(uint8_t KeyPadTone)
{
	setting.setting_struct.KeyPadTone = KeyPadTone;
}

/*设置和读取是否有gps*/
uint8_t setting_GetHaveGps(void)
{
	return setting.setting_struct.HaveGps;
}
void setting_SetHaveGps(uint8_t have)
{
	setting.setting_struct.HaveGps = have;
}







/*获取按键音*/
uint8_t setting_GetKeyPadTone(void)
{
	return setting.setting_struct.KeyPadTone;
}
/*设置报警音开关*/
void setting_SetAlarmTone(uint8_t AlarmTone)
{
	setting.setting_struct.AlarmTone = AlarmTone;
}
/*获取报警音*/
uint8_t setting_GetAlarmTone(void)
{
	return setting.setting_struct.AlarmTone;
}

//	uint8_t IsAlarm_pH;        //是否报警
//	uint8_t IsAlarm_DO;        //是否报警
//	uint8_t IsAlarm_FCL;        //是否报警
//	uint8_t IsAlarm_EC;        //是否报警
//	uint8_t IsAlarm_Tur;        //是否报警
//	uint8_t IsAlarm_ORP;        //是否报警
//	uint8_t IsAlarm_NH4;        //是否报警
//	uint8_t IsAlarm_F;        //是否报警
//	uint8_t IsAlarm_CL;        //是否报警
//	uint8_t IsAlarm_Chl;        //是否报警
//	uint8_t IsAlarm_Bga;        //是否报警
//	uint8_t IsAlarm_CODuv;        //是否报警

/*设置是否报警*/
void setting_SetIsAlarm_DO(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_DO = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_DO(void)
{
	return setting.setting_struct.IsAlarm_DO;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_DO(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_DO = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_DO(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_DO;
}

/*
#include <stdio.h>

uint8_t temp_buff[13] = {0};

void flash_test(void)
{
	W25QXX_Write((uint8_t *)"hello world", 0, sizeof("hello world"));
	
	sFLASH_ReadBuffer(temp_buff, 0, sizeof("hello world"));	
	printf("%s\n", temp_buff);
}
*/

/*从flash读取设置*/
void FlashToSetting(void)
{
	W25QXX_Read(setting.setting_arr, SETTING_START_ADDR, sizeof(setting.setting_arr));	
}

/*将设置保存到flash*/
void SettingToFlash(void)
{
	W25QXX_Write(setting.setting_arr, SETTING_START_ADDR, sizeof(setting.setting_arr));
}

void init_setting(void)
{
	uint8_t bool_temp;
	uint8_t customize_temp;
	
	W25QXX_Read(&bool_temp, SETTING_CNORENG_ADDR, 1);//获取初始化是否是中英文设置
	setting.setting_struct.IsChn   = (bool_temp ? 1 : 0);//如果是1的话就初始化成中文 如果是0的话初始化成英文
	
	W25QXX_Read(&bool_temp, SETTING_HAVEGPS_ADDR, 1);//获取初始化是否有gps设置
	setting.setting_struct.HaveGps   = (bool_temp ? 1 : 0);//1就有 0就没有
	
	W25QXX_Read(&customize_temp, SETTING_LOGO_ADDR, 1);//获取初始化是否是中英文设置
	switch(customize_temp)
	{
		case LUHENG:
			setting.setting_struct.customize = LUHENG;
			break;
		
		case HENGLAN:
			setting.setting_struct.customize = HENGLAN;
			break;
		
		default:
			setting.setting_struct.customize = ZHONGXING;
			break;
	}
	
	setting.setting_struct.KeyPadTone = 1;
	setting.setting_struct.AlarmTone = 1;
	
	/*锁定模式*/
	setting.setting_struct.AutoLock_pH = 0;
	setting.setting_struct.AutoLock_DO = 0;
	setting.setting_struct.AutoLock_FCL = 0;
	setting.setting_struct.AutoLock_EC = 0;
	setting.setting_struct.AutoLock_Tur = 0;
	setting.setting_struct.AutoLock_ORP = 0;
	setting.setting_struct.AutoLock_NH4 = 0;
	setting.setting_struct.AutoLock_F = 0;
	setting.setting_struct.AutoLock_CL = 0;
	setting.setting_struct.AutoLock_Chl = 0;
	setting.setting_struct.AutoLock_Bga = 0;
	setting.setting_struct.AutoLock_CODuv = 0;
	
	/*自动锁定等级*/
	setting.setting_struct.AutoLock_level_pH = 0;
	setting.setting_struct.AutoLock_level_DO = 0;
	setting.setting_struct.AutoLock_level_FCL = 0;
	setting.setting_struct.AutoLock_level_EC = 0;
	setting.setting_struct.AutoLock_level_Tur = 0;
	setting.setting_struct.AutoLock_level_ORP = 0;
	setting.setting_struct.AutoLock_level_NH4 = 0;
	setting.setting_struct.AutoLock_level_F = 0;
	setting.setting_struct.AutoLock_level_CL = 0;
	setting.setting_struct.AutoLock_level_Chl = 0;
	setting.setting_struct.AutoLock_level_Bga = 0;
	setting.setting_struct.AutoLock_level_CODuv = 0;
	
	/*是否开启滑动平均功能*/
	setting.setting_struct.IsOpen_SlideAvg_pH = 0;
	setting.setting_struct.IsOpen_SlideAvg_DO = 0;
	setting.setting_struct.IsOpen_SlideAvg_FCL = 0;
	setting.setting_struct.IsOpen_SlideAvg_EC = 0;
	setting.setting_struct.IsOpen_SlideAvg_Tur = 0;
	setting.setting_struct.IsOpen_SlideAvg_ORP = 0;
	setting.setting_struct.IsOpen_SlideAvg_NH4 = 0;
	setting.setting_struct.IsOpen_SlideAvg_F = 0;
	setting.setting_struct.IsOpen_SlideAvg_CL = 0;
	setting.setting_struct.IsOpen_SlideAvg_Chl = 0;
	setting.setting_struct.IsOpen_SlideAvg_Bga = 0;
	setting.setting_struct.IsOpen_SlideAvg_CODuv = 0;
	
	/*滑动平均次数*/
	setting.setting_struct.SlideAvgTimes_pH = 2;
	setting.setting_struct.SlideAvgTimes_DO = 2;
	setting.setting_struct.SlideAvgTimes_FCL = 2;
	setting.setting_struct.SlideAvgTimes_EC = 2;
	setting.setting_struct.SlideAvgTimes_Tur = 2;
	setting.setting_struct.SlideAvgTimes_ORP = 2;
	setting.setting_struct.SlideAvgTimes_NH4 = 2;
	setting.setting_struct.SlideAvgTimes_F = 2;
	setting.setting_struct.SlideAvgTimes_CL = 2;
	setting.setting_struct.SlideAvgTimes_Chl = 2;
	setting.setting_struct.SlideAvgTimes_Bga = 2;
	setting.setting_struct.SlideAvgTimes_CODuv = 2;
	
	setting.setting_struct.IsAlarm_pH = 0;
	setting.setting_struct.IsAlarm_DO = 0;
	setting.setting_struct.IsAlarm_FCL = 0;
	setting.setting_struct.IsAlarm_EC = 0;
	setting.setting_struct.IsAlarm_Tur = 0;
	setting.setting_struct.IsAlarm_ORP = 0;
	setting.setting_struct.IsAlarm_NH4 = 0;
	setting.setting_struct.IsAlarm_F = 0;
	setting.setting_struct.IsAlarm_CL = 0;
	setting.setting_struct.IsAlarm_Chl = 0;
	setting.setting_struct.IsAlarm_Bga = 0;
	setting.setting_struct.IsAlarm_CODuv = 0;
	
	
	setting.setting_struct.AutoShut = 10;
	
	setting.setting_struct.LowThreshold_pH = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_pH = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_DO = 0.00;  //低门限报警阈值    判断mg/L
	setting.setting_struct.HighThreshold_DO = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_FCL = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_FCL = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_EC = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_EC = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_Tur = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Tur = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_ORP = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_ORP = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_NH4 = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_NH4 = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_F = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_F = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_CL = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_CL = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_Chl = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Chl = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_Bga = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Bga = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_CODuv = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_CODuv = 0.00; //高门限报警阈值
	
	setting.setting_struct.AirPressure = 0.0;    //大气压补偿
	setting.setting_struct.Salinity = 0.0;       //盐度值
	
}



/*
	uint8_t IsChn :1;         //是否显示中文
	uint8_t KeyPadTone:1;     //是否有按键音
	uint8_t AlarmTone:1;      //是否打开报警提示音
	uint8_t IsAlarm:1;        //是否报警
	
	uint8_t AutoShut;         //0 5 10 20
	uint8_t ModbusId;         //0-255
	
	value_type LowThreshold;  //低门限报警阈值
	value_type HighThreshold; //高门限报警阈值
	value_type AirPressure;   //气压补偿
	value_type Salinity;      //盐度值
*/
/*恢复出厂设置*/
void setting_reset(void)
{
	init_setting();
	
	SettingToFlash();
	
	HAL_Delay(20);
	FlashToSetting();
}

void first_write(void)
{
//	uint8_t FirstStart_arr[sizeof(setting.setting_arr)]={0}; //第一次写初始化设置的数组
	uint8_t count_data[2] = {0, 0};                          //清记录数的数组
	uint8_t first_start = SETTING_FIRSTRUN_JUDGE;            //将第一次标志位写入数值
	
	init_setting();                                          //初始化设置
	
	//memcpy(FirstStart_arr, setting.setting_arr, sizeof(setting.setting_arr));
	
	W25QXX_Write(&first_start, SETTING_FIRSTRUN_ADDR, 1);                              //清第一次上电
	W25QXX_Write(count_data, LOG_COUNT_ADDR, 2);                                       //清记录条数
	
	SettingToFlash();
	
//	W25QXX_Write(setting.setting_arr, SETTING_START_ADDR, sizeof(setting.setting_arr));//将出厂设置写入flash
	
	HAL_Delay(20);
	FlashToSetting();
}


/*获取大气压的补偿值*/
value_type setting_GetAirCompensate(void)
{
	return setting.setting_struct.AirPressure;
}
void setting_SetAirCompensate(value_type value)
{
	setting.setting_struct.AirPressure = value;
}

/*获取盐度值*/
value_type setting_GetSalinity(void)
{
	return setting.setting_struct.Salinity;
}
void setting_SetSalinity(value_type value)
{
	setting.setting_struct.Salinity = value;
}

value_type setting_GetHighThreshold_DO(void)
{
	return setting.setting_struct.HighThreshold_DO;
}
void setting_SetHighThreshold_DO(value_type value)
{
	setting.setting_struct.HighThreshold_DO = value;
}
value_type setting_GetLowThreshold_DO(void)
{
	return setting.setting_struct.LowThreshold_DO;
}
void setting_SetLowThreshold_DO(value_type value)
{
	setting.setting_struct.LowThreshold_DO = value;
}


uint8_t setting_GetSlideAvgTimes_DO(void)
{
	return setting.setting_struct.SlideAvgTimes_DO;
}
void setting_SetSlideAvgTimes_DO(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_DO = times;
}


uint8_t setting_GetAutoLock_DO(void)
{
	return setting.setting_struct.AutoLock_DO;
}
void setting_SetAutoLock_DO(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_DO = AutoLock;
}

uint8_t setting_GetAutoLockLevel_DO(void)
{
	return setting.setting_struct.AutoLock_level_DO;
}
void setting_SetAutoLockLevel_DO(uint8_t level)
{
	setting.setting_struct.AutoLock_level_DO = level;
}



uint8_t setting_GetAutoShut(void)
{
	return setting.setting_struct.AutoShut;
}
void setting_SetAutoShut(uint8_t value)
{
	setting.setting_struct.AutoShut = value;
	RTC_UpdateShutDownTime(setting_GetAutoShut());
}


void settting_SetInitIsChn(uint8_t IsChn)
{
	W25QXX_Write(&IsChn, SETTING_CNORENG_ADDR, 1);
}

void setting_SetInitLogo(uint8_t logo)
{
	W25QXX_Write(&logo, SETTING_LOGO_ADDR, 1);
}








