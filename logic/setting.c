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
/*设置按键音开关*/
void setting_SetKeyPadTone(uint8_t KeyPadTone)
{
	setting.setting_struct.KeyPadTone = KeyPadTone;
}


uint8_t setting_GetAutoLock(void)
{
	return setting.setting_struct.AutoLock;
}
void setting_SetAutoLock(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock = AutoLock;
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
void setting_SetIsAlarm_pH(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_pH = IsAlarm;
}
void setting_SetIsAlarm_DO(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_DO = IsAlarm;
}
void setting_SetIsAlarm_FCL(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_FCL = IsAlarm;
}
void setting_SetIsAlarm_EC(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_EC = IsAlarm;
}
void setting_SetIsAlarm_Tur(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_Tur = IsAlarm;
}
void setting_SetIsAlarm_ORP(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_ORP = IsAlarm;
}
void setting_SetIsAlarm_NH4(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_NH4 = IsAlarm;
}
void setting_SetIsAlarm_F(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_F = IsAlarm;
}
void setting_SetIsAlarm_CL(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_CL = IsAlarm;
}
void setting_SetIsAlarm_Chl(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_Chl = IsAlarm;
}
void setting_SetIsAlarm_Bga(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_Bga = IsAlarm;
}
void setting_SetIsAlarm_CODuv(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_CODuv = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_pH(void)
{
	return setting.setting_struct.IsAlarm_pH;
}
uint8_t setting_GetIsAlarm_DO(void)
{
	return setting.setting_struct.IsAlarm_DO;
}
uint8_t setting_GetIsAlarm_FCL(void)
{
	return setting.setting_struct.IsAlarm_FCL;
}
uint8_t setting_GetIsAlarm_EC(void)
{
	return setting.setting_struct.IsAlarm_EC;
}
uint8_t setting_GetIsAlarm_Tur(void)
{
	return setting.setting_struct.IsAlarm_Tur;
}
uint8_t setting_GetIsAlarm_ORP(void)
{
	return setting.setting_struct.IsAlarm_ORP;
}
uint8_t setting_GetIsAlarm_NH4(void)
{
	return setting.setting_struct.IsAlarm_NH4;
}
uint8_t setting_GetIsAlarm_F(void)
{
	return setting.setting_struct.IsAlarm_F;
}
uint8_t setting_GetIsAlarm_CL(void)
{
	return setting.setting_struct.IsAlarm_CL;
}
uint8_t setting_GetIsAlarm_Chl(void)
{
	return setting.setting_struct.IsAlarm_Chl;
}
uint8_t setting_GetIsAlarm_Bga(void)
{
	return setting.setting_struct.IsAlarm_Bga;
}
uint8_t setting_GetIsAlarm_CODuv(void)
{
	return setting.setting_struct.IsAlarm_CODuv;
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
	uint8_t IsCN;
	
	W25QXX_Read(&IsCN, SETTING_CNORENG_ADDR, 1);//获取初始化是否是中英文设置
	
	setting.setting_struct.IsChn   = IsCN ? 1 : 0;//如果是1的话就初始化成中文 如果是0的话初始化成英文
	setting.setting_struct.KeyPadTone = 1;
	setting.setting_struct.AlarmTone = 1;
	
	setting.setting_struct.AutoLock = 0;
	
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
	
	HAL_Delay(5);
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
	
	W25QXX_Write(setting.setting_arr, SETTING_START_ADDR, sizeof(setting.setting_arr));//将出厂设置写入flash
	
	W25QXX_Write(count_data, LOG_COUNT_ADDR, 2);                                       //清记录条数
	
	HAL_Delay(5);
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


value_type setting_GetHighThreshold_pH(void)
{
	return setting.setting_struct.HighThreshold_pH;
}
void setting_SetHighThreshold_pH(value_type value)
{
	setting.setting_struct.HighThreshold_pH = value;
}
value_type setting_GetLowThreshold_pH(void)
{
	return setting.setting_struct.LowThreshold_pH;
}
void setting_SetLowThreshold_pH(value_type value)
{
	setting.setting_struct.LowThreshold_pH = value;
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



value_type setting_GetHighThreshold_FCL(void)
{
	return setting.setting_struct.HighThreshold_FCL;
}
void setting_SetHighThreshold_FCL(value_type value)
{
	setting.setting_struct.HighThreshold_FCL = value;
}
value_type setting_GetLowThreshold_FCL(void)
{
	return setting.setting_struct.LowThreshold_FCL;
}
void setting_SetLowThreshold_FCL(value_type value)
{
	setting.setting_struct.LowThreshold_FCL = value;
}



value_type setting_GetHighThreshold_EC(void)
{
	return setting.setting_struct.HighThreshold_EC;
}
void setting_SetHighThreshold_EC(value_type value)
{
	setting.setting_struct.HighThreshold_EC = value;
}
value_type setting_GetLowThreshold_EC(void)
{
	return setting.setting_struct.LowThreshold_EC;
}
void setting_SetLowThreshold_EC(value_type value)
{
	setting.setting_struct.LowThreshold_EC = value;
}



value_type setting_GetHighThreshold_Tur(void)
{
	return setting.setting_struct.HighThreshold_Tur;
}
void setting_SetHighThreshold_Tur(value_type value)
{
	setting.setting_struct.HighThreshold_Tur = value;
}
value_type setting_GetLowThreshold_Tur(void)
{
	return setting.setting_struct.LowThreshold_Tur;
}
void setting_SetLowThreshold_Tur(value_type value)
{
	setting.setting_struct.LowThreshold_Tur = value;
}



value_type setting_GetHighThreshold_ORP(void)
{
	return setting.setting_struct.HighThreshold_ORP;
}
void setting_SetHighThreshold_ORP(value_type value)
{
	setting.setting_struct.HighThreshold_ORP = value;
}
value_type setting_GetLowThreshold_ORP(void)
{
	return setting.setting_struct.LowThreshold_ORP;
}
void setting_SetLowThreshold_ORP(value_type value)
{
	setting.setting_struct.LowThreshold_ORP = value;
}



value_type setting_GetHighThreshold_NH4(void)
{
	return setting.setting_struct.HighThreshold_NH4;
}
void setting_SetHighThreshold_NH4(value_type value)
{
	setting.setting_struct.HighThreshold_NH4 = value;
}
value_type setting_GetLowThreshold_NH4(void)
{
	return setting.setting_struct.LowThreshold_NH4;
}
void setting_SetLowThreshold_NH4(value_type value)
{
	setting.setting_struct.LowThreshold_NH4 = value;
}



value_type setting_GetHighThreshold_F(void)
{
	return setting.setting_struct.HighThreshold_F;
}
void setting_SetHighThreshold_F(value_type value)
{
	setting.setting_struct.HighThreshold_F = value;
}
value_type setting_GetLowThreshold_F(void)
{
	return setting.setting_struct.LowThreshold_F;
}
void setting_SetLowThreshold_F(value_type value)
{
	setting.setting_struct.LowThreshold_F = value;
}



value_type setting_GetHighThreshold_CL(void)
{
	return setting.setting_struct.HighThreshold_CL;
}
void setting_SetHighThreshold_CL(value_type value)
{
	setting.setting_struct.HighThreshold_CL = value;
}
value_type setting_GetLowThreshold_CL(void)
{
	return setting.setting_struct.LowThreshold_CL;
}
void setting_SetLowThreshold_CL(value_type value)
{
	setting.setting_struct.LowThreshold_CL = value;
}



value_type setting_GetHighThreshold_Chl(void)
{
	return setting.setting_struct.HighThreshold_Chl;
}
void setting_SetHighThreshold_Chl(value_type value)
{
	setting.setting_struct.HighThreshold_Chl = value;
}
value_type setting_GetLowThreshold_Chl(void)
{
	return setting.setting_struct.LowThreshold_Chl;
}
void setting_SetLowThreshold_Chl(value_type value)
{
	setting.setting_struct.LowThreshold_Chl = value;
}



value_type setting_GetHighThreshold_Bga(void)
{
	return setting.setting_struct.HighThreshold_Bga;
}
void setting_SetHighThreshold_Bga(value_type value)
{
	setting.setting_struct.HighThreshold_Bga = value;
}
value_type setting_GetLowThreshold_Bga(void)
{
	return setting.setting_struct.LowThreshold_Bga;
}
void setting_SetLowThreshold_Bga(value_type value)
{
	setting.setting_struct.LowThreshold_Bga = value;
}



value_type setting_GetHighThreshold_CODuv(void)
{
	return setting.setting_struct.HighThreshold_CODuv;
}
void setting_SetHighThreshold_CODuv(value_type value)
{
	setting.setting_struct.HighThreshold_CODuv = value;
}
value_type setting_GetLowThreshold_CODuv(void)
{
	return setting.setting_struct.LowThreshold_CODuv;
}
void setting_SetLowThreshold_CODuv(value_type value)
{
	setting.setting_struct.LowThreshold_CODuv = value;
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










