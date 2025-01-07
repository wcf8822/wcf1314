#include "heart.h"
#include "key.h"
#include "iwdg.h"
#include "bee.h"
#include "rs485.h"

#include "lcd_drive.h"
#include "setting.h"
#include "interfacial.h"

///////////////////////////////////////////////////////////得整一个时间冒号闪烁的功能
///////////////////////////////////////////////////////////得整一个2s弹窗显示保存成功


//////////////////////////////////////////////////////////结构体声明

//所有main里要用到的flag结构体声明
typedef struct{
	uint8_t flag_RefreshLcd : 1; //将缓存刷到屏幕上去
	uint8_t flag_RefreshRtc : 1; //rtc采集
	uint8_t flag_RefreshBmp : 1; //采集气压
	uint8_t flag_RefreshBat : 1; //采集电池ad
	uint8_t flag_RefreshGPS : 1; //更新gps信息
	uint8_t flag_interfacial: 1; //将界面刷到缓存中
	uint8_t flag_warning    : 1; //报警图标闪烁
	uint8_t flag_Test       : 1; //测试用定时器
	uint8_t flag_SinMes     : 1; //单次测试用定时器
	
}flag_t;

//按键按下时长的结构体变量声明
typedef struct{//按键消抖计时器
	uint8_t count_up : 4;
	uint8_t count_down : 4;
	uint8_t count_esc : 4;
	uint8_t count_blu : 4;
	uint8_t count_menu : 4;
	
	uint16_t count_cal;
	uint16_t count_ok;
	uint16_t count_off;
}KeyCount_t;

typedef struct{
	uint8_t flag_SuccessfulTimStart : 1;
	uint16_t count_SuccessfulTim;
}Function_t;

///////////////////////////////////////////////////////////结构体实例化

//main中要用到的标志
static volatile flag_t MainFlags={
	.flag_RefreshLcd = 0,
	.flag_RefreshRtc = 0,
	.flag_RefreshBmp = 0,
	.flag_RefreshBat = 0,
	.flag_RefreshGPS = 0,
	.flag_interfacial = 0,
	.flag_Test = 0,
};

static volatile Function_t functions={
	.flag_SuccessfulTimStart = 0,
	.count_SuccessfulTim =0
};

//按键事件
static volatile KeyCount_t key_count={
	.count_up = 0,
	.count_down = 0,
	.count_ok = 0,
	.count_esc = 0,
	.count_menu = 0,
	.count_cal = 0,
	.count_blu = 0,
	.count_off = 0,
};

//按键开始计数标志
static volatile KeyFlag_t key_CountFlag={
	.flag_KeyUp = 0,
	.flag_KeyDown = 0,
	.flag_KeyOk = 0,
	.flag_KeyEsc = 0,
	.flag_KeyMenu = 0,
	.flag_KeyCal = 0,
	.flag_KeyBlu = 0,
	.flag_KeyOFF = 0,
	.flag_KeyOFFLong = 0,	
};

//按键是否被按过标志
static volatile KeyFlag_t key_ClickedFlag={
	.flag_KeyUp = 0,
	.flag_KeyDown = 0,
	.flag_KeyOk = 0,
	.flag_KeyEsc = 0,
	.flag_KeyMenu = 0,
	.flag_KeyCal = 0,
	.flag_KeyBlu = 0,
	.flag_KeyOFF = 0,
	.flag_KeyOFFLong = 0,	
};


//	.flag_RefreshLcd = 0,
//	.flag_RefreshRtc = 0,
//	.flag_RefreshBmp = 0,
//	.flag_RefreshBat = 0,
//	.flag_RefreshGPS = 0,
//	.flag_interfacial = 0,
//	.flag_Test = 0,

#define SET_LCDFLAG()           MainFlags.flag_RefreshLcd = 1
#define SET_RTCFLAG()           MainFlags.flag_RefreshRtc = 1
#define SET_BMPFLAG()           MainFlags.flag_RefreshBmp = 1
#define SET_BATFLAG()           MainFlags.flag_RefreshBat = 1
#define SET_GPSFLAG()           MainFlags.flag_RefreshGPS = 1
#define SET_INTERFACIALFLAG()   MainFlags.flag_interfacial = 1
#define SET_TESTFLAG()          MainFlags.flag_Test = 1
#define SET_WARNINGFLAG()       MainFlags.flag_warning = 1
// #define SET_SinMesFLAG()        MainFlags.flag_SinMes = 1


#define CLEAR_LCDFLAG()         MainFlags.flag_RefreshLcd = 0
#define CLEAR_RTCFLAG()         MainFlags.flag_RefreshRtc = 0
#define CLEAR_BMPFLAG()         MainFlags.flag_RefreshBmp = 0
#define CLEAR_BATFLAG()         MainFlags.flag_RefreshBat = 0
#define CLEAR_GPSFLAG()         MainFlags.flag_RefreshGPS = 0
#define CLEAR_INTERFACIALFLAG() MainFlags.flag_interfacial = 0
#define CLEAR_TESTFLAG()        MainFlags.flag_Test = 0
#define CLEAR_WARNINGFLAG()     MainFlags.flag_warning = 0
// #define CLEAR_SinMesFLAG()      MainFlags.flag_SinMes = 0


uint16_t TimeValue = 0;
uint8_t MesRun = 0;

//main接口
uint8_t get_RtcFlag(void)
{
	return MainFlags.flag_RefreshRtc;
}
uint8_t get_BmpFlag(void)
{
	return MainFlags.flag_RefreshBmp;
}
uint8_t get_LcdFlag(void)
{
	return MainFlags.flag_RefreshLcd;
}
uint8_t get_BatFlag(void)
{
	return MainFlags.flag_RefreshBat;
}
uint8_t get_GPSFlag(void)
{
	return MainFlags.flag_RefreshGPS;
}
uint8_t get_InterfacialFlag(void)
{
	return MainFlags.flag_interfacial;
}
uint8_t get_TestFlag(void)
{
	return MainFlags.flag_Test;
}
uint8_t get_WarningFlag(void)
{
	return MainFlags.flag_warning;
}

uint16_t get_SinMesFlag(void)
{
	return TimeValue;
}


void clear_RtcFlag(void)
{
	CLEAR_RTCFLAG();
}
void clear_BmpFlag(void)
{
	CLEAR_BMPFLAG();
}
void clear_LcdFlag(void)
{
	CLEAR_LCDFLAG();
}
void clear_TestFlag(void)
{
	CLEAR_TESTFLAG();
}
void clear_BatFlag(void)
{
	CLEAR_BATFLAG();
}
void clear_InterfacialFlag(void)
{
	CLEAR_INTERFACIALFLAG();
}
void clear_GPSFlag(void)
{
	CLEAR_GPSFLAG();
}
void clear_WarningFlag(void)
{
	CLEAR_WARNINGFLAG();
}
void clear_SinMesTimeFlag(void)
{
	// CLEAR_SinMesFLAG();
}

//按键计时开始结束读取状态
void set_KeyUpCountFlag(void)//开始计时
{
	key_CountFlag.flag_KeyUp = 1;
}
void set_KeyDownCountFlag(void)
{
	key_CountFlag.flag_KeyDown = 1;
}
void set_KeyOkCountFlag(void)
{
	key_CountFlag.flag_KeyOk = 1;
}
void set_KeyEscCountFlag(void)
{
	key_CountFlag.flag_KeyEsc = 1;
}
void set_KeyCalCountFlag(void)
{
	key_CountFlag.flag_KeyCal = 1;
}
void set_KeyMenuCountFlag(void)
{
	key_CountFlag.flag_KeyMenu = 1;
}
void set_KeyBluCountFlag(void)
{
	key_CountFlag.flag_KeyBlu = 1;
}	
void set_KeyOffCountFlag(void)
{
	key_CountFlag.flag_KeyOFF = 1;
}
void set_KeyOffLongCountFlag(void)
{
	key_CountFlag.flag_KeyOFFLong = 1;
}	



uint8_t get_KeyUpCountFlag(void)//获取是否在计时
{
	return key_CountFlag.flag_KeyUp;
}
uint8_t get_KeyDownCountFlag(void)
{
	return key_CountFlag.flag_KeyDown;
}
uint8_t get_KeyOkCountFlag(void)
{
	return key_CountFlag.flag_KeyOk;
}
uint8_t get_KeyEscCountFlag(void)
{
	return key_CountFlag.flag_KeyEsc;
}
uint8_t get_KeyCalCountFlag(void)
{
	return key_CountFlag.flag_KeyCal;
}
uint8_t get_KeyMenuCountFlag(void)
{
	return key_CountFlag.flag_KeyMenu;
}
uint8_t get_KeyBluCountFlag(void)
{
	return key_CountFlag.flag_KeyBlu;
}	
uint8_t get_KeyOffCountFlag(void)
{
	return key_CountFlag.flag_KeyOFF;
}
uint8_t get_KeyOffLongCountFlag(void)
{
	return key_CountFlag.flag_KeyOFFLong;
}	



void clear_KeyUpCountFlag(void)//清除计时标志
{
	key_CountFlag.flag_KeyUp = 0;
}
void clear_KeyDownCountFlag(void)
{
	key_CountFlag.flag_KeyDown = 0;
}
void clear_KeyOkCountFlag(void)
{
	key_CountFlag.flag_KeyOk = 0;
}
void clear_KeyEscCountFlag(void)
{
	key_CountFlag.flag_KeyEsc = 0;
}
void clear_KeyCalCountFlag(void)
{
	key_CountFlag.flag_KeyCal = 0;
}
void clear_KeyMenuCountFlag(void)
{
	key_CountFlag.flag_KeyMenu = 0;
}
void clear_KeyBluCountFlag(void)
{
	key_CountFlag.flag_KeyBlu = 0;
}	
void clear_KeyOffCountFlag(void)
{
	key_CountFlag.flag_KeyOFF = 0;
}
void clear_KeyOffLongCountFlag(void)
{
	key_CountFlag.flag_KeyOFFLong = 0;
}	


//清除按键定时器计数
void clear_KeyUpCount(void)
{
	key_count.count_up = 0;
}
void clear_KeyDownCount(void)
{
	key_count.count_down = 0;
}
void clear_KeyEscCount(void)
{
	key_count.count_esc = 0;
}
void clear_KeyOkCount(void)
{
	key_count.count_ok = 0;
}
void clear_KeyCalCount(void)
{
	key_count.count_cal = 0;
}
void clear_KeyMenuCount(void)
{
	key_count.count_menu = 0;
}
void clear_KeyBluCount(void)
{
	key_count.count_blu = 0;
}
void clear_KeyOffCount(void)
{
	key_count.count_off = 0;
}


//用于cal长按判断
uint16_t get_KeyCalCount(void)
{
	return key_count.count_cal;
}
uint16_t get_KeyOkCount(void)
{
	return key_count.count_ok;
}
uint16_t get_KeyOffCount(void)
{
	return key_count.count_off;
}


//获取按键是否被按过标志
uint8_t get_KeyUpClickedFlag(void)//获取
{
	return key_ClickedFlag.flag_KeyUp;
}
uint8_t get_KeyDownClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyDown;
}
uint8_t get_KeyOkClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyOk;
}
uint8_t get_KeyEscClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyEsc;
}
uint8_t get_KeyCalClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyCal;
}
uint8_t get_KeyMenuClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyMenu;
}
uint8_t get_KeyBluClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyBlu;
}	
uint8_t get_KeyOffClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyOFF;
}
uint8_t get_KeyOffLongClickedFlag(void)
{
	return key_ClickedFlag.flag_KeyOFFLong;
}	


void clear_KeyUpClickedFlag(void)//清除
{
	key_ClickedFlag.flag_KeyUp = 0;
}
void clear_KeyDownClickedFlag(void)
{
	key_ClickedFlag.flag_KeyDown = 0;
}
void clear_KeyOkClickedFlag(void)
{
	key_ClickedFlag.flag_KeyOk = 0;
}
void clear_KeyEscClickedFlag(void)
{
	key_ClickedFlag.flag_KeyEsc = 0;
}
void clear_KeyCalClickedFlag(void)
{
	key_ClickedFlag.flag_KeyCal = 0;
}
void clear_KeyMenuClickedFlag(void)
{
	key_ClickedFlag.flag_KeyMenu = 0;
}
void clear_KeyBluClickedFlag(void)
{
	key_ClickedFlag.flag_KeyBlu = 0;
}

void clear_KeyOffClickedFlag(void)
{
	key_ClickedFlag.flag_KeyOFF = 0;
}
void clear_KeyOffLongClickedFlag(void)
{
	key_ClickedFlag.flag_KeyOFFLong = 0;
}


void set_SuccessfulTimStartFlag(void)
{
	functions.flag_SuccessfulTimStart = 1;
}
uint8_t get_SuccessfulTimStartFlag(void)
{
	return functions.flag_SuccessfulTimStart;
}
void clear_SuccessfulTimStartFlag(void)
{
	functions.flag_SuccessfulTimStart = 0;
}
void clear_SuccessfulTimCount(void)
{
	functions.count_SuccessfulTim = 0;
}
static uint16_t count_warning = 0;

void clear_WarningCount(void)
{
	count_warning = 0;
}


static uint16_t count_data = 0;

void clear_DATACount(void)
{
	count_data = 0;
}





//#define TIM_REFRESHBAT  3000  //电池电量更新时间
//#define TIM_REFRESHGPS  5000  //gps更新时间
//#define TIM_INTERFACIAL 200   //显存更新时间
void main_tim(void)
{
	static uint8_t count_lcd = 0;
	static uint16_t count_rct = 0;
	static uint16_t count_bmp = 0;
	static uint16_t count_test = 0;
	static uint16_t count_gps = 0;
	static uint16_t count_bat = 0;
	static uint16_t count_interfacial = 0;
	static uint16_t count_danciceliang = 0;

	
	if(++count_lcd >= TIM_REFRESHLCD)
	{
		count_lcd = 0;
		SET_LCDFLAG();
	}
	if(++count_rct >= TIM_REFRESHRTC)
	{
		count_rct = 0;
		SET_RTCFLAG();
	}
	if(++count_bmp >= TIM_REFRESHBMP)
	{
		count_bmp = 0;
		SET_BMPFLAG();
	}
	if(++count_test >= TIM_TEST)
	{
		count_test = 0;
		SET_TESTFLAG();
	}
	
	if(++count_gps >= TIM_REFRESHGPS)
	{
		count_gps = 0;
		SET_GPSFLAG();
	}
	if(++count_bat >= TIM_REFRESHBAT)
	{
		count_bat = 0;
		SET_BATFLAG();
	}
	if(++count_interfacial >= TIM_INTERFACIAL)
	{
		count_interfacial = 0;
		SET_INTERFACIALFLAG();
	}
	if(++count_warning >= TIM_WARNING)
	{
		count_warning = 0;
		SET_WARNINGFLAG();
		if(Auto_Search_Count <= Auto_Search_Time) Auto_Search_Count++;
	}	
	if (++count_danciceliang >= 1000 )
	{
		count_danciceliang = 0;
		TimeValue++;
		if(TimeValue == 256)
		{
			TimeValue = setting_Getdanciceliangtime() + 5;
		}
	}
	

}


/*
void set_KeyUpFlag(void);
void set_KeyDownFlag(void);
void set_KeyOkFlag(void);
void set_KeyEscFlag(void);
void set_KeyMenuFlag(void);
void set_KeyCalFlag(void);
void set_KeyBluFlag(void);

*/


void key_tim(void)
{
	if((key_CountFlag.flag_KeyUp) && ++key_count.count_up >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyUp = 1;
	}
	
	
	if((key_CountFlag.flag_KeyDown) && ++key_count.count_down >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyDown = 1;
	}
	
	
	if((key_CountFlag.flag_KeyOk) && ++key_count.count_ok >= KEY_TIM )
	{
		key_ClickedFlag.flag_KeyOk = 1;
		if(key_count.count_ok >= KEY_TIM_LONG)
		{
			set_KeyOkLongFlag();              //设置长按标志
		}
	}
	
	
	if((key_CountFlag.flag_KeyEsc) && ++key_count.count_esc >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyEsc = 1;
	}
	
	
	if((key_CountFlag.flag_KeyCal) && ++key_count.count_cal >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyCal = 1;
		if(key_count.count_cal >= KEY_TIM_LONG_LONG)
		{
			set_KeyCalLongLongFlag();
		}
		else if(key_count.count_cal >= KEY_TIM_LONG)
		{
			set_KeyCalLongFlag();              //设置长按标志
		}
	}
	
	
	if((key_CountFlag.flag_KeyMenu) && ++key_count.count_menu >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyMenu = 1;
	}
	
	
	if((key_CountFlag.flag_KeyBlu) && ++key_count.count_blu >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyBlu = 1;
	}
	
	if((key_CountFlag.flag_KeyOFF) && ++key_count.count_off >= KEY_TIM)
	{
		key_ClickedFlag.flag_KeyOFF = 1;
		if(key_count.count_off >= KEY_TIM_LONG)
		{
			set_KeyOffLongFlag();              //设置长按标志
		}
	}
}

void functions_tim(void)
{
	if(get_SuccessfulTimStartFlag() && ++functions.count_SuccessfulTim >= TIM_SUCCEFFUL)
	{
		if(!(get_KeyOkCountFlag()))
		{
			clear_SuccessfulTimCount();
			clear_SuccessfulTimStartFlag();
			interfacial_SetNeedDestroyMSG();
			//destory_MessageBox();//这个放到main里面清
		}
	}
}

void logo_tim(void)
{
	static uint16_t logo_count = 0;
	if(!gui_GetRefreshStatus() && ++logo_count >= LOGO_TIM)
	{
		logo_count = 0;
		gui_SetRefreshON();
	}
}


/*系统心跳定时器中断*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim->Instance == TIM2)
	{
		feed_dog();
		main_tim();
		key_tim();
		bee_handle();
		functions_tim();
		logo_tim();
		
		rs485_TimHandle();
	}
}




