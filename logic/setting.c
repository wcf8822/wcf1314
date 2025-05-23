//用来保存和读取设置参数的
#include "setting.h"
#include "spi_flash.h"
#include <string.h>
#include "rtc.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
STATIC setting_union setting;



/*设置DC18的浊度显示开关*/
void setting_SetKey_DC18_Set(uint8_t DC18_Set)
{
	setting.setting_struct.DC18_Set = DC18_Set;
}
/*获取DC18的浊度显示开关设置*/
uint8_t setting_GetIsKey_DC18_Set(void)
{
	return setting.setting_struct.DC18_Set;
}


/*设置GPS开关*/
void setting_SetKeyGPS(uint8_t KeyGps)
{
	setting.setting_struct.KeyGps = KeyGps;
}
/*获取GPS开关设置*/
uint8_t setting_GetIsKeyGps(void)
{
	return setting.setting_struct.KeyGps;
}


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


/*设置盐度单位*/
void setting_SetSal_Unit(uint8_t value)
{
	setting.setting_struct.Sal_unit = value;
}
/*获取盐度单位*/
uint8_t setting_GetSal_Uni(void)
{
	return setting.setting_struct.Sal_unit;
}


/*设置pH 的标液组*/
void setting_SetIs_pH_Group(uint8_t Is_pH_Group)
{
	setting.setting_struct.Is_pH_Group = Is_pH_Group;
}
/*获取pH 的标液组*/
uint8_t setting_GetIs_pH_Group(void)
{
	return setting.setting_struct.Is_pH_Group;
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

/*设置和读取对比度设置*/
uint8_t setting_GetContrast(void)
{
	return setting.setting_struct.contrast;
}
void setting_SetContrast(uint8_t contrast)
{
	setting.setting_struct.contrast = contrast;
}

/*设置和读取电池类型*/
uint8_t setting_GetBattertType(void)
{
	return setting.setting_struct.BattertType;
}
void setting_SetBattertType(uint8_t BattertType)
{
	setting.setting_struct.BattertType = BattertType;
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

/*设置单次测量时间*/
void setting_Setdanciceliangtime(uint8_t time)
{
	setting.setting_struct.SinMesTime = time;
}
/*获取单次测量时间*/
uint8_t setting_Getdanciceliangtime(void)
{
	return setting.setting_struct.SinMesTime;
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

/*设置是否报警*/
void setting_SetIsAlarm_pH(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_pH = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_pH(void)
{
	return setting.setting_struct.IsAlarm_pH;
}

/*设置是否报警*/
void setting_SetIsAlarm_Tur(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_Tur = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_Tur(void)
{
	return setting.setting_struct.IsAlarm_Tur;
}


/*设置是否报警*/
void setting_SetIsAlarm_FCL(uint8_t IsAlarm)
{
	// setting.setting_struct.IsAlarm_FCL = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_FCL(void)
{
	// return setting.setting_struct.IsAlarm_FCL;
}

/*设置是否报警*/
void setting_SetIsAlarm_EC(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_EC = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_EC(void)
{
	return setting.setting_struct.IsAlarm_EC;
}



/*设置是否报警*/
void setting_SetIsAlarm_ORP(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_ORP = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_ORP(void)
{
	return setting.setting_struct.IsAlarm_ORP;
}

/*设置是否报警*/
void setting_SetIsAlarm_NH4(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_NH4 = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_NH4(void)
{
	return setting.setting_struct.IsAlarm_NH4;
}
/*设置是否报警*/


void setting_SetIsAlarm_F(uint8_t IsAlarm)
{
	// setting.setting_struct.IsAlarm_F = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_F(void)
{
	// return setting.setting_struct.IsAlarm_F;
}

void setting_SetIsAlarm_CL(uint8_t IsAlarm)
{
	// setting.setting_struct.IsAlarm_CL = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_CL(void)
{
	// return setting.setting_struct.IsAlarm_CL;
}


void setting_SetIsAlarm_Chl(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_Chl = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_Chl(void)
{
	return setting.setting_struct.IsAlarm_Chl;
}


void setting_SetIsAlarm_Bga(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_Bga = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_Bga(void)
{
	return setting.setting_struct.IsAlarm_Bga;
}


/*设置是否报警*/
void setting_SetIsAlarm_COD(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_COD = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_COD(void)
{
	return setting.setting_struct.IsAlarm_COD;
}

/*设置是否报警*/
void setting_SetIsAlarm_MLSS(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_MLSS = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_MLSS(void)
{
	return setting.setting_struct.IsAlarm_MLSS;
}

/*设置是否报警*/
void setting_SetIsAlarm_OIW(uint8_t IsAlarm)
{
	setting.setting_struct.IsAlarm_OIW = IsAlarm;
}

/*获取是否报警*/
uint8_t setting_GetIsAlarm_OIW(void)
{
	return setting.setting_struct.IsAlarm_OIW;
}

/*设置是否选中*/
void setting_SetIsSelect_DO(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_DO = sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_DO(void)
{
	return setting.setting_struct.IsSelect_DO;
}

/*设置是否选中*/
void setting_SetIsSelect_DO_shenghui(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_DO_shenghui = sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_DO_shenghui(void)
{
	return setting.setting_struct.IsSelect_DO_shenghui;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_pH(void)
{
	return setting.setting_struct.IsSelect_pH;
}

/*设置是否选中*/
void setting_SetIsSelect_pH(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_pH = sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_Tur(void)
{
	return setting.setting_struct.IsSelect_Tur;
}

/*设置是否选中*/
void setting_SetIsSelect_Tur(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_Tur = sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_FCL(void)
{
	// return setting.setting_struct.IsSelect_FCL;
}

/*设置是否选中*/
void setting_SetIsSelect_FCL(uint8_t sSelect)
{
	// setting.setting_struct.IsSelect_FCL = sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_EC(void)
{
	return setting.setting_struct.IsSelect_EC;
}

/*设置是否选中*/
void setting_SetIsSelect_EC(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_EC= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_ORP(void)
{
	return setting.setting_struct.IsSelect_ORP;
}

/*设置是否选中*/
void setting_SetIsSelect_ORP(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_ORP = sSelect;
}


/*设置是否选中*/
void setting_SetIsSelect_NH4_DN02(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_NH4_DN02 = sSelect;
}
/*获取是否选中*/
uint8_t setting_GetIsSelect_NH4_DN02(void)
{
	return setting.setting_struct.IsSelect_NH4_DN02;
}

/*设置是否选中*/
void setting_SetIsSelect_NH4_shenghui(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_NH4_shenghui = sSelect;
}
/*获取是否选中*/
uint8_t setting_GetIsSelect_NH4_shenghui(void)
{
	return setting.setting_struct.IsSelect_NH4_shenghui;
}

/*设置是否选中*/
void setting_SetIsSelect_F(uint8_t sSelect)
{
	// setting.setting_struct.IsSelect_F= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_F(void)
{
	// return setting.setting_struct.IsSelect_F;
}


/*设置是否选中*/
void setting_SetIsSelect_CL(uint8_t sSelect)
{
	// setting.setting_struct.IsSelect_F= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_CL(void)
{
	// return setting.setting_struct.IsSelect_F;
}

/*设置是否选中*/
void setting_SetIsSelect_Chl(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_Chl= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_Chl(void)
{
	return setting.setting_struct.IsSelect_Chl;
}

/*设置是否选中*/
void setting_SetIsSelect_Bga(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_Bga= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_Bga(void)
{
	return setting.setting_struct.IsSelect_Bga;
}

/*设置是否选中*/
void setting_SetIsSelect_COD_DC18(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_COD_DC18= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_COD_DC18(void)
{
	return setting.setting_struct.IsSelect_COD_DC18;
}
/*设置是否选中*/
void setting_SetIsSelect_COD_shenghui(uint8_t sSelect)
{
	setting.setting_struct.IsSelect_COD_shenghui= sSelect;
}

/*获取是否选中*/
uint8_t setting_GetIsSelect_COD_shenghui(void)
{
	return setting.setting_struct.IsSelect_COD_shenghui;
}


/*设置温度单位*/
//1:℉
//0:℃
void setting_Set_Temp_Unit(uint8_t Unit)
{
	setting.setting_struct.TEMP_Unit = Unit;
}
/*获取温度单位设置*/
uint8_t setting_Get_Temp_Unit(void)
{
	return setting.setting_struct.TEMP_Unit;
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

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_pH(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_pH = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_pH(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_pH;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_FCL(uint8_t IsOpen)
{
	// setting.setting_struct.IsOpen_SlideAvg_FCL = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_FCL(void)
{
	// return setting.setting_struct.IsOpen_SlideAvg_FCL;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_EC(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_EC = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_EC(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_EC;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_Tur(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_Tur = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_Tur(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_Tur;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_ORP(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_ORP = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_ORP(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_ORP;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_NH4(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_NH4 = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_NH4(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_NH4;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_F(uint8_t IsOpen)
{
	// setting.setting_struct.IsOpen_SlideAvg_F = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_F(void)
{
	// return setting.setting_struct.IsOpen_SlideAvg_F;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_CL(uint8_t IsOpen)
{
	// setting.setting_struct.IsOpen_SlideAvg_CL = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_CL(void)
{
	// return setting.setting_struct.IsOpen_SlideAvg_CL;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_Chl(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_Chl = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_Chl(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_Chl;
}


/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_Bga(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_Bga = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_Bga(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_Bga;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_COD(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_COD = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_COD(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_COD;
}

/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_MLSS(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_MLSS = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_MLSS(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_MLSS;
}


/*是否开启滑动平均设置*/
void setting_SetIsOpen_SlideAvg_OIW(uint8_t IsOpen)
{
	setting.setting_struct.IsOpen_SlideAvg_OIW = IsOpen;
}
uint8_t setting_GetIsOpen_SlideAvg_OIW(void)
{
	return setting.setting_struct.IsOpen_SlideAvg_OIW;
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
	
	W25QXX_Read(&bool_temp, SETTING_CONTRAST_ADDR, 1);//获取初始化是否有高低对比度
	setting.setting_struct.contrast  = (bool_temp ? 1 : 0);//1是浓 0是淡
	
	W25QXX_Read(&bool_temp, BATTERT_TYPE_ADDR, 1);//获取初始化是否有电池类型设置
	setting.setting_struct.BattertType  = (bool_temp ? 1 : 0);//0是锂电池  1是干电池
	
	W25QXX_Read(&bool_temp, B580_D702_TYPE_ADDR, 1);//获取初始化是否有电池类型设置
	setting.setting_struct.Instrument_model  = (bool_temp ? 1 : 0);//0是702  1是580

	W25QXX_Read(&customize_temp, SETTING_LOGO_ADDR, 1);//获取初始化是否是中英文设置
	switch(customize_temp)
	{
		case LUHENG:
			setting.setting_struct.customize = LUHENG;
			break;
		
		case HENGLAN:
			setting.setting_struct.customize = HENGLAN;
			break;

		case LIHE:
			setting.setting_struct.customize = LIHE;
			break;
		
		default:
			setting.setting_struct.customize = ZHONGXING;
			break;
	}
	
	setting.setting_struct.KeyPadTone = 1;
	setting.setting_struct.AlarmTone = 1;
	setting.setting_struct.Orp_high_Threshold_pn  = 68;
	setting.setting_struct.Orp_low_Threshold_pn  = 68;
	
	/*锁定模式*/
	setting.setting_struct.AutoLock_pH = 0;
	setting.setting_struct.AutoLock_DO = 0;
	setting.setting_struct.AutoLock_EC = 0;
	setting.setting_struct.AutoLock_Tur = 0;
	setting.setting_struct.AutoLock_ORP = 0;
	setting.setting_struct.AutoLock_NH4 = 0;
	setting.setting_struct.AutoLock_Chl = 0;
	setting.setting_struct.AutoLock_Bga = 0;
	setting.setting_struct.AutoLock_COD= 0;
	setting.setting_struct.AutoLock_MLSS = 0;
	
	/*自动锁定等级*/
	setting.setting_struct.AutoLock_level_pH = 0;
	setting.setting_struct.AutoLock_level_DO = 0;
	setting.setting_struct.AutoLock_level_EC = 0;
	setting.setting_struct.AutoLock_level_Tur = 0;
	setting.setting_struct.AutoLock_level_ORP = 0;
	setting.setting_struct.AutoLock_level_NH4 = 0;
	setting.setting_struct.AutoLock_level_Chl = 0;
	setting.setting_struct.AutoLock_level_Bga = 0;
	setting.setting_struct.AutoLock_level_COD = 0;
	setting.setting_struct.AutoLock_level_MLSS = 0;
	
	/*是否开启滑动平均功能*/
	setting.setting_struct.IsOpen_SlideAvg_pH = 0;
	setting.setting_struct.IsOpen_SlideAvg_DO = 0;
	setting.setting_struct.IsOpen_SlideAvg_EC = 0;
	setting.setting_struct.IsOpen_SlideAvg_Tur = 0;
	setting.setting_struct.IsOpen_SlideAvg_ORP = 0;
	setting.setting_struct.IsOpen_SlideAvg_NH4 = 0;
	setting.setting_struct.IsOpen_SlideAvg_Chl = 0;
	setting.setting_struct.IsOpen_SlideAvg_Bga = 0;
	setting.setting_struct.IsOpen_SlideAvg_COD = 0;
	setting.setting_struct.IsOpen_SlideAvg_MLSS = 0;
	
	/*滑动平均次数*/
	setting.setting_struct.SlideAvgTimes_pH = 2;
	setting.setting_struct.SlideAvgTimes_DO = 2;
	setting.setting_struct.SlideAvgTimes_EC = 2;
	setting.setting_struct.SlideAvgTimes_Tur = 2;
	setting.setting_struct.SlideAvgTimes_ORP = 2;
	setting.setting_struct.SlideAvgTimes_NH4 = 2;
	setting.setting_struct.SlideAvgTimes_Chl = 2;
	setting.setting_struct.SlideAvgTimes_Bga = 2;
	setting.setting_struct.SlideAvgTimes_COD = 2;
	setting.setting_struct.SlideAvgTimes_MLSS = 2;
	
	setting.setting_struct.IsAlarm_pH = 0;
	setting.setting_struct.IsAlarm_DO = 0;
	setting.setting_struct.IsAlarm_EC = 0;
	setting.setting_struct.IsAlarm_Tur = 0;
	setting.setting_struct.IsAlarm_ORP = 0;
	setting.setting_struct.IsAlarm_NH4 = 0;
	setting.setting_struct.IsAlarm_Chl = 0;
	setting.setting_struct.IsAlarm_Bga = 0;
	setting.setting_struct.IsAlarm_COD = 0;
	setting.setting_struct.IsAlarm_MLSS = 0;

	setting.setting_struct.IsSelect_DO = 1;
	setting.setting_struct.IsSelect_DO_shenghui = 1;
	setting.setting_struct.IsSelect_pH = 1;
	setting.setting_struct.IsSelect_EC = 1;
	setting.setting_struct.IsSelect_Tur = 1;
	setting.setting_struct.IsSelect_ORP = 1;
	setting.setting_struct.IsSelect_NH4_DN02 = 1;
	setting.setting_struct.IsSelect_NH4_shenghui = 1;
	setting.setting_struct.IsSelect_Chl = 1;
	setting.setting_struct.IsSelect_Bga = 1;
	setting.setting_struct.IsSelect_COD_DC18 = 1;
	setting.setting_struct.IsSelect_COD_shenghui = 1;

	
	setting.setting_struct.AutoShut = 10;
	
	setting.setting_struct.LowThreshold_pH = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_pH = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_DO = 0.00;  //低门限报警阈值    判断mg/L
	setting.setting_struct.HighThreshold_DO = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_EC = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_EC = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_Tur = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Tur = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_ORP = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_ORP = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_NH4 = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_NH4 = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_Chl = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Chl = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_Bga = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Bga = 0.00; //高门限报警阈值

	setting.setting_struct.LowThreshold_COD = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_COD = 0.00; //高门限报警阈值
	setting.setting_struct.LowThreshold_Mlss = 0.00;  //低门限报警阈值
	setting.setting_struct.HighThreshold_Mlss = 0.00; //高门限报警阈值
	
	setting.setting_struct.AirPressure = 0.0;    //大气压补偿
	setting.setting_struct.Salinity = 0.0;       //盐度值

	setting.setting_struct.IntervalTime = 0;
	setting.setting_struct.LockSave = 0;
	
	setting.setting_struct.KeyGps = 0;	
	
	setting.setting_struct.TEMP_Unit = 0;//默认温度单位为 ℃
	setting.setting_struct.DC18_Set = 0;	
	setting.setting_struct.Sal_unit = 0;//默认ppt
	setting.setting_struct.Mes_Unit = 0;//默认mg

}

/*恢复出厂设置*/
void setting_reset(void)
{
	init_setting();

	//清除所有历史数据
	for(uint8_t i = 0;i<log_count_Max;i++)
	{
		log_SetLogCount(0,i);
	}

	SettingToFlash();
	
	HAL_Delay(20);
	FlashToSetting();
}


void first_write(void)
{
	uint8_t first_start = SETTING_FIRSTRUN_JUDGE;            //将第一次标志位写入数值
		
	init_setting();     		//初始化设置
	setting_SetBattertType(0);   //第一次开机设置成默认锂电池   0是锂电池  1是干电池	
	W25QXX_Write(&first_start, SETTING_FIRSTRUN_ADDR, 1);                              //清第一次上电
	
	for(uint8_t i = 0;i<log_count_Max;i++)
	{
		log_SetLogCount(0,i);
	}

	SettingToFlash();
		
	HAL_Delay(20);
	FlashToSetting();
}


//设置间隔保存时间
uint16_t setting_GetAutoIntervalTime(void)
{
	return setting.setting_struct.IntervalTime;
}
void setting_SetAutoIntervalTime(uint16_t time)
{
	setting.setting_struct.IntervalTime = time;
}

//设置锁定保存开关
void setting_SetLockSave(uint8_t KeyPadTone)
{
	setting.setting_struct.LockSave = KeyPadTone;
}
uint8_t setting_GetLockSave(void)
{
	return setting.setting_struct.LockSave;
}

//设置产品型号
void setting_Set_Type(uint8_t model)
{
	setting.setting_struct.Instrument_model = model;
}
uint8_t setting_Get_Type(void)
{
	return setting.setting_struct.Instrument_model;
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

value_type setting_GetHighThreshold_FCL(void)
{
	// return setting.setting_struct.HighThreshold_FCL;
}
void setting_SetHighThreshold_FCL(value_type value)
{
	// setting.setting_struct.HighThreshold_FCL = value;
}
value_type setting_GetLowThreshold_FCL(void)
{
	// return setting.setting_struct.LowThreshold_FCL;
}
void setting_SetLowThreshold_FCL(value_type value)
{
	// setting.setting_struct.LowThreshold_FCL = value;
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
	setting.setting_struct.HighThreshold_NH4= value;
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
	// return setting.setting_struct.HighThreshold_F;
}
void setting_SetHighThreshold_F(value_type value)
{
	// setting.setting_struct.HighThreshold_F= value;
}
value_type setting_GetLowThreshold_F(void)
{
	// return setting.setting_struct.LowThreshold_F;
}
void setting_SetLowThreshold_F(value_type value)
{
	// setting.setting_struct.LowThreshold_F = value;
}

value_type setting_GetHighThreshold_CL(void)
{
	// return setting.setting_struct.HighThreshold_CL;
}
void setting_SetHighThreshold_CL(value_type value)
{
	// setting.setting_struct.HighThreshold_CL= value;
}
value_type setting_GetLowThreshold_CL(void)
{
	// return setting.setting_struct.LowThreshold_CL;
}
void setting_SetLowThreshold_CL(value_type value)
{
	// setting.setting_struct.LowThreshold_CL = value;
}

value_type setting_GetHighThreshold_Chl(void)
{
	return setting.setting_struct.HighThreshold_Chl;
}
void setting_SetHighThreshold_Chl(value_type value)
{
	setting.setting_struct.HighThreshold_Chl= value;
}
value_type setting_GetLowThreshold_Chl(void)
{
	return setting.setting_struct.LowThreshold_Chl;
}
void setting_SetLowThreshold_Chl(value_type value)
{
	setting.setting_struct.LowThreshold_Chl= value;
}

value_type setting_GetHighThreshold_Bga(void)
{
	return setting.setting_struct.HighThreshold_Bga;
}
void setting_SetHighThreshold_Bga(value_type value)
{
	setting.setting_struct.HighThreshold_Bga= value;
}
value_type setting_GetLowThreshold_Bga(void)
{
	return setting.setting_struct.LowThreshold_Bga;
}
void setting_SetLowThreshold_Bga(value_type value)
{
	setting.setting_struct.LowThreshold_Bga= value;
}


value_type setting_GetHighThreshold_COD(void)
{
	return setting.setting_struct.HighThreshold_COD;
}
void setting_SetHighThreshold_COD(value_type value)
{
	setting.setting_struct.HighThreshold_COD= value;
}
value_type setting_GetLowThreshold_COD(void)
{
	return setting.setting_struct.LowThreshold_COD;
}
void setting_SetLowThreshold_COD(value_type value)
{
	setting.setting_struct.LowThreshold_COD = value;
}

value_type setting_GetHighThreshold_MLSS(void)
{
	return setting.setting_struct.HighThreshold_Mlss;
}
void setting_SetHighThreshold_MLSS(value_type value)
{
	setting.setting_struct.HighThreshold_Mlss= value;
}

value_type setting_GetLowThreshold_MLSS(void)
{
	return setting.setting_struct.LowThreshold_Mlss;
}
void setting_SetLowThreshold_MLSS(value_type value)
{
	setting.setting_struct.LowThreshold_Mlss = value;
}

value_type setting_GetHighThreshold_OIW(void)
{
	return setting.setting_struct.HighThreshold_OIW;
}
void setting_SetHighThreshold_OIW(value_type value)
{
	setting.setting_struct.HighThreshold_OIW= value;
}

value_type setting_GetLowThreshold_OIW(void)
{
	return setting.setting_struct.LowThreshold_OIW;
}
void setting_SetLowThreshold_OIW(value_type value)
{
	setting.setting_struct.LowThreshold_OIW = value;
}



uint8_t setting_GetSlideAvgTimes_DO(void)
{
	return setting.setting_struct.SlideAvgTimes_DO;
}
void setting_SetSlideAvgTimes_DO(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_DO = times;
}

uint8_t setting_GetSlideAvgTimes_pH(void)
{
	return setting.setting_struct.SlideAvgTimes_pH;
}
void setting_SetSlideAvgTimes_pH(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_pH = times;
}

uint8_t setting_GetSlideAvgTimes_FCL(void)
{
	// return setting.setting_struct.SlideAvgTimes_FCL;
}
void setting_SetSlideAvgTimes_FCL(uint8_t times)
{
	// setting.setting_struct.SlideAvgTimes_FCL = times;
}

uint8_t setting_GetSlideAvgTimes_EC(void)
{
	return setting.setting_struct.SlideAvgTimes_EC;
}
void setting_SetSlideAvgTimes_EC(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_EC = times;
}

uint8_t setting_GetSlideAvgTimes_Tur(void)
{
	return setting.setting_struct.SlideAvgTimes_Tur;
}
void setting_SetSlideAvgTimes_Tur(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_Tur = times;
}

uint8_t setting_GetSlideAvgTimes_ORP(void)
{
	return setting.setting_struct.SlideAvgTimes_ORP;
}
void setting_SetSlideAvgTimes_ORP(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_ORP = times;
}

uint8_t setting_GetSlideAvgTimes_NH4(void)
{
	return setting.setting_struct.SlideAvgTimes_NH4;
}
void setting_SetSlideAvgTimes_NH4(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_NH4 = times;
}

uint8_t setting_GetSlideAvgTimes_F(void)
{
	// return setting.setting_struct.SlideAvgTimes_F;
}
void setting_SetSlideAvgTimes_F(uint8_t times)
{
	// setting.setting_struct.SlideAvgTimes_F = times;
}

uint8_t setting_GetSlideAvgTimes_CL(void)
{
	// return setting.setting_struct.SlideAvgTimes_CL;
}
void setting_SetSlideAvgTimes_CL(uint8_t times)
{
	// setting.setting_struct.SlideAvgTimes_CL = times;
}


uint8_t setting_GetSlideAvgTimes_Chl(void)
{
	return setting.setting_struct.SlideAvgTimes_Chl;
}
void setting_SetSlideAvgTimes_Chl(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_Chl = times;
}


uint8_t setting_GetSlideAvgTimes_Bga(void)
{
	return setting.setting_struct.SlideAvgTimes_Bga;
}
void setting_SetSlideAvgTimes_Bga(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_Bga = times;
}

uint8_t setting_GetSlideAvgTimes_COD(void)
{
	return setting.setting_struct.SlideAvgTimes_COD;
}
void setting_SetSlideAvgTimes_COD(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_COD = times;
}

uint8_t setting_GetSlideAvgTimes_MLSS(void)
{
	return setting.setting_struct.SlideAvgTimes_MLSS;
}
void setting_SetSlideAvgTimes_MLSS(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_MLSS = times;
}

uint8_t setting_GetSlideAvgTimes_OIW(void)
{
	return setting.setting_struct.SlideAvgTimes_OIW;
}
void setting_SetSlideAvgTimes_OIW(uint8_t times)
{
	setting.setting_struct.SlideAvgTimes_OIW = times;
}





uint8_t setting_GetOrp_low_Threshold_pn(void)
{
	return setting.setting_struct.Orp_low_Threshold_pn;
}
void setting_SetOrp_low_Threshold_pn(uint8_t value)
{
	setting.setting_struct.Orp_low_Threshold_pn = value;
}

uint8_t setting_GetOrp_high_Threshold_pn(void)
{
	return setting.setting_struct.Orp_high_Threshold_pn;
}
void setting_SetOrp_high_Threshold_pn(uint8_t value)
{
	setting.setting_struct.Orp_high_Threshold_pn = value;
}


uint8_t setting_GetAutoLock_DO(void)
{
	return setting.setting_struct.AutoLock_DO;
}
void setting_SetAutoLock_DO(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_DO = AutoLock;
}

uint8_t setting_GetAutoLock_pH(void)
{
	return setting.setting_struct.AutoLock_pH;
}
void setting_SetAutoLock_pH(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_pH = AutoLock;
}

uint8_t setting_GetAutoLock_Tur(void)
{
	return setting.setting_struct.AutoLock_Tur;
}
void setting_SetAutoLock_Tur(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_Tur = AutoLock;
}

uint8_t setting_GetAutoLock_FCL(void)
{
	// return setting.setting_struct.AutoLock_FCL;
	return 1;
}
void setting_SetAutoLock_FCL(uint8_t AutoLock)
{
	// setting.setting_struct.AutoLock_FCL = AutoLock;
}

uint8_t setting_GetAutoLock_EC(void)
{
	return setting.setting_struct.AutoLock_EC;
}
void setting_SetAutoLock_EC(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_EC = AutoLock;
}

uint8_t setting_GetAutoLock_ORP(void)
{
	return setting.setting_struct.AutoLock_ORP;
}
void setting_SetAutoLock_ORP(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_ORP = AutoLock;
}

uint8_t setting_GetAutoLock_NH4(void)
{
	return setting.setting_struct.AutoLock_NH4;
}
void setting_SetAutoLock_NH4(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_NH4 = AutoLock;
}

uint8_t setting_GetAutoLock_F(void)
{
	// return setting.setting_struct.AutoLock_F;
}
void setting_SetAutoLock_F(uint8_t AutoLock)
{
	// setting.setting_struct.AutoLock_F = AutoLock;
}

uint8_t setting_GetAutoLock_CL(void)
{
	// return setting.setting_struct.AutoLock_CL;
}
void setting_SetAutoLock_CL(uint8_t AutoLock)
{
	// setting.setting_struct.AutoLock_CL = AutoLock;
}

uint8_t setting_GetAutoLock_Chl(void)
{
	return setting.setting_struct.AutoLock_Chl;
}
void setting_SetAutoLock_Chl(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_Chl = AutoLock;
}

uint8_t setting_GetAutoLock_Bga(void)
{
	return setting.setting_struct.AutoLock_Bga;
}
void setting_SetAutoLock_Bga(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_Bga = AutoLock;
}

uint8_t setting_GetAutoLock_COD(void)
{
	return setting.setting_struct.AutoLock_COD;
}
void setting_SetAutoLock_COD(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_COD = AutoLock;
}

uint8_t setting_GetAutoLock_MLSS(void)
{
	return setting.setting_struct.AutoLock_MLSS;
}
void setting_SetAutoLock_MLSS(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_MLSS = AutoLock;
}

uint8_t setting_GetAutoLock_OIW(void)
{
	return setting.setting_struct.AutoLock_OIW;
}
void setting_SetAutoLock_OIW(uint8_t AutoLock)
{
	setting.setting_struct.AutoLock_OIW = AutoLock;
}


uint8_t setting_GetAutoLockLevel_DO(void)
{
	return setting.setting_struct.AutoLock_level_DO;
}
void setting_SetAutoLockLevel_DO(uint8_t level)
{
	setting.setting_struct.AutoLock_level_DO = level;
}

uint8_t setting_GetAutoLockLevel_pH(void)
{
	return setting.setting_struct.AutoLock_level_pH;
}
void setting_SetAutoLockLevel_pH(uint8_t level)
{
	setting.setting_struct.AutoLock_level_pH = level;
}

uint8_t setting_GetAutoLockLevel_Tur(void)
{
	return setting.setting_struct.AutoLock_level_Tur;
}
void setting_SetAutoLockLevel_Tur(uint8_t level)
{
	setting.setting_struct.AutoLock_level_Tur = level;
}

uint8_t setting_GetAutoLockLevel_FCL(void)
{
	// return setting.setting_struct.AutoLock_level_FCL;
}
void setting_SetAutoLockLevel_FCL(uint8_t level)
{
	// setting.setting_struct.AutoLock_level_FCL = level;
}

uint8_t setting_GetAutoLockLevel_EC(void)
{
	return setting.setting_struct.AutoLock_level_EC;
}
void setting_SetAutoLockLevel_EC(uint8_t level)
{
	setting.setting_struct.AutoLock_level_EC = level;
}

uint8_t setting_GetAutoLockLevel_ORP(void)
{
	return setting.setting_struct.AutoLock_level_ORP;
}
void setting_SetAutoLockLevel_ORP(uint8_t level)
{
	setting.setting_struct.AutoLock_level_ORP = level;
}

uint8_t setting_GetAutoLockLevel_NH4(void)
{
	return setting.setting_struct.AutoLock_level_NH4;
}
void setting_SetAutoLockLevel_NH4(uint8_t level)
{
	setting.setting_struct.AutoLock_level_NH4 = level;
}

uint8_t setting_GetAutoLockLevel_F(void)
{
	// return setting.setting_struct.AutoLock_level_F;
}
void setting_SetAutoLockLevel_F(uint8_t level)
{
	// setting.setting_struct.AutoLock_level_F = level;
}

uint8_t setting_GetAutoLockLevel_CL(void)
{
	// return setting.setting_struct.AutoLock_level_CL;
}
void setting_SetAutoLockLevel_CL(uint8_t level)
{
	// setting.setting_struct.AutoLock_level_CL = level;
}

uint8_t setting_GetAutoLockLevel_Chl(void)
{
	return setting.setting_struct.AutoLock_level_Chl;
}
void setting_SetAutoLockLevel_Chl(uint8_t level)
{
	setting.setting_struct.AutoLock_level_Chl = level;
}


uint8_t setting_GetAutoLockLevel_Bga(void)
{
	return setting.setting_struct.AutoLock_level_Bga;
}
void setting_SetAutoLockLevel_Bga(uint8_t level)
{
	setting.setting_struct.AutoLock_level_Bga= level;
}

uint8_t setting_GetAutoLockLevel_COD(void)
{
	return setting.setting_struct.AutoLock_level_COD;
}
void setting_SetAutoLockLevel_COD(uint8_t level)
{
	setting.setting_struct.AutoLock_level_COD = level;
}

uint8_t setting_GetAutoLockLevel_MLSS(void)
{
	return setting.setting_struct.AutoLock_level_MLSS;
}
void setting_SetAutoLockLevel_MLSS(uint8_t level)
{
	setting.setting_struct.AutoLock_level_MLSS = level;
}

uint8_t setting_GetAutoLockLevel_OIW(void)
{
	return setting.setting_struct.AutoLock_level_OIW;
}
void setting_SetAutoLockLevel_OIW(uint8_t level)
{
	setting.setting_struct.AutoLock_level_OIW = level;
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

uint8_t setting_GetMesUnit(void)
{
	return setting.setting_struct.Mes_Unit;
}
void setting_SetMesUnit(uint8_t value)
{
	setting.setting_struct.Mes_Unit = value;
}






