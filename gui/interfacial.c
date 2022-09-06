#include "interfacial.h"
#include <stdio.h>
#include <math.h>

#include "lcd_drive.h"
#include "loadbit.h"
#include "rtc.h"

#include "setting.h"

#include "main.h"

#include "key.h"
#include "bee.h"

#include "generate_page.h"

#include "bmp280.h"

#include "dissolved_oxygen.h"

#include "heart.h"

#include "log.h"

#include "spi_flash.h"
/***************************************************************任何指针操作记得加安全性判断是否为空指针！！！！！！！！！！！！！！！！！！！！！！！！！！！*/

typedef struct{
	uint8_t* content_cn;
	uint8_t* content_en;
	uint8_t content_cn_len;
}MsgContent_t;

MsgContent_t msg_content = {
	.content_cn = (uint8_t *)msg_save_cn,
	.content_en = (uint8_t *)msg_save_en,
	.content_cn_len = sizeof(msg_save_cn)
};


STATIC PtrToOptionNode cur_option = NULL;         //当前指向的选项
STATIC PtrToNanoOptionNode cur_NanoOption = NULL; //当前指向的小选项

STATIC PAGE_NUM cur_PageNum = PAGE_0_START;//当前刷新的界面

STATIC char time_arr[6]  = {0};  //左上角时间的数组
STATIC char press_arr[7] = {0};  //大气压通用数组
STATIC char sal_arr[6]   = {0};  //盐度通用数组
STATIC char cal_arr[8]   = {0};  //校准完成后显示用的校准通用数组

STATIC uint8_t flag_show_MSG = 0;           //是否显示弹窗
STATIC uint8_t flag_is_successful = 1;      //是否操作成功标志
STATIC uint8_t flag_NeedDestroyMSG = 0;     //是否需要销毁msgbox
STATIC uint8_t flage_FirstOKLong = 0;       //是不是第一次长按ok键

STATIC uint8_t* option_sensor_name;         //传感器管理里面设备显示名称的指针
STATIC PAGE_NUM temp_FatherPage;            //临时的父界面
STATIC float STD_value = 0.0;               //校准时候要用到的标准值

STATIC SENSOR_TYPE temp_SensorType;         //当前需要修改的设备类型
STATIC SENSOR_TYPE alarm_SensorType;        //报警设置选择的设备类型

MESSAGE_TYPE cur_MsgType = MESSAGE_SETTING; //弹窗的类型

STATIC uint16_t cur_LogIndex = 0;           //当前选择的数据日志的下标
STATIC uint8_t cur_LogIndex_arr[6] = {0};   //数组 65535

STATIC uint8_t FatherPage_OptionIndex = 0;  //父界面所选的标签下标
STATIC uint8_t GrandpaPage_OptionIndex = 0; //爷爷界面所选的标签下标
STATIC uint8_t AncestorPage_OptionIndex = 0;//阿太界面所选的标签下标

STATIC uint8_t flag_NeedWarning = 0;

HARDWARE_VERSION hardware_version; //硬件版本
const uint8_t software_version[] = "v1.0";  //软件版本

void interfacial_SetPage(PAGE_NUM page_num, uint8_t IsBack);//通过枚举变量设置显示的界面

uint8_t days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

STATIC Interfacial_t cur_interfacial ={
	.page_father = PAGE_0_START,
	.label_head = NULL,
	.option_head = NULL,
	.content_chn = NULL,
	.content_eng = NULL
}; 


static void (*btnUpDownFunc)(void) = NULL;   //上下按钮逻辑函数指针
static void (*btnOkEscFunc)(void) = NULL;    //ok esc按钮逻辑函数指针

//这里定义一个气压显示的buf



//声明在前方便后面直接注册调用
void btn_OkEscMode_NULL(void);
void btn_UpDownMode_NULL(void);
void btn_OkEscMode_ChangeOption(void);
void btn_UpDownMode_ChangeValue(void);
void btn_OkEscMode_ChangePage(void);
void btn_UpDownMode_ChangeOption(void);
void btn_OkEscMode_MsgBox(void);
void btn_UpDownMode_MsgBox(void);

void save_setting(void);

//注册按键处理逻辑
void btnUpDownFunc_register(void (*func)(void))
{
	btnUpDownFunc = func;
}
void btnOkEscFunc_register(void (*func)(void))
{
	btnOkEscFunc = func;
}

void interfacial_SetMessageBoxFlag(void)
{
	flag_show_MSG = 1;
}
uint8_t interfacial_GetMessageBoxFlag(void)
{
	return flag_show_MSG;
}
void interfacial_ClearMessageBoxFlag(void)
{
	flag_show_MSG = 0;
}

void interfacial_SetNeedDestroyMSG(void)
{
	flag_NeedDestroyMSG = 1;
}
void interfacial_ClearNeedDestroyMSG(void)
{
	flag_NeedDestroyMSG = 0;
}
uint8_t interfacial_GetNeedDestroyMSG(void)
{
	return flag_NeedDestroyMSG;
}

void interfacial_SetCurMsgType(MESSAGE_TYPE msg_type)
{
	cur_MsgType = msg_type;
}
MESSAGE_TYPE interfacial_GetCurMsgType(void)
{
	return cur_MsgType;
}
uint16_t interfacial_GetCurLogIndex(void)
{
	return cur_LogIndex;
}
void interfacial_SetCurLogIndex(uint16_t dat)
{
	cur_LogIndex = dat;
}
uint8_t *interfacial_GetCurLogIndexArr(void)
{
	return cur_LogIndex_arr;
}

//HARDWARE_VERSION interfacial_GetHWVersion(void);
//void interfacial_SetHWVersion(HARDWARE_VERSION hwv);
//uint8_t interfacial_GetSWVersion(void);

HARDWARE_VERSION interfacial_GetHWVersion(void)
{
	return hardware_version;
}
void interfacial_SetHWVersion(HARDWARE_VERSION hwv)
{
	hardware_version = hwv;
}
uint8_t* interfacial_GetSWVersion(void)
{
	return (uint8_t *)software_version;
}

SENSOR_TYPE interfacial_GetAlarmSensorType(void)
{
	return alarm_SensorType;
}

uint8_t interfacial_GetNeedWarning(void)
{
	return flag_NeedWarning;
}
void interfacial_ClearNeedWarning(void)
{
	flag_NeedWarning = 0;
}
void interfacial_UpdateCurLogIndexArr(uint16_t data_index)
{
	snprintf((char *)cur_LogIndex_arr, 6, "%5d", data_index);
	//return cur_LogIndex_arr;
}

PAGE_NUM interfacial_GetTempFatherPage(void)
{
	return temp_FatherPage;
}

void generate_MessageBox(MESSAGE_TYPE msg_type, uint8_t is_successful)
{
	interfacial_SetCurMsgType(msg_type);
	interfacial_SetMessageBoxFlag();//设置显示flag
	
	flag_is_successful = is_successful;
	
	switch(interfacial_GetCurMsgType())
	{
		case MESSAGE_SETTING:
			cur_NanoOption = cur_interfacial.NanoOption_msg->next_option;
			cur_NanoOption->IsSelected = 1;
			
			btnUpDownFunc_register(btn_UpDownMode_MsgBox);//注册新的按键逻辑
			btnOkEscFunc_register(btn_OkEscMode_MsgBox);//注册新的按键逻辑
			break;
		case MESSAGE_SAVELOG:
			set_SuccessfulTimStartFlag();
			if(interfacial_GetCurPage() != PAGE_0_START)
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			}
			break;
		case MESSAGE_SUCCESSFUL://成功的话要显示一段时间的
			set_SuccessfulTimStartFlag();
			if(interfacial_GetCurPage() != PAGE_0_START)
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			}
			break;
		
		case MESSAGE_DELETE:
			break;
	}
}

///*在初始界面打印溶解氧单位*/
//void StartPage_PrintUint_DO(void)
//{
//	GUI_PutCharArr(120,  50, (uint8_t *)BAIFENGHAO, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);     //%
//	GUI_PutEngStr (120,  86, (uint8_t *)DO_uint[1], MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);      //mg/L
//	GUI_PutCharArr(120, 114, (uint8_t *)&(FONT_ENG_MENU[71]), 16, 16, LOADBIT_NORMAL);                            //℃
//}

void destory_MessageBox(void)
{
	interfacial_ClearMessageBoxFlag();
	
	gui_ClearMessageBox();
}
void show_MessageBox(void)
{
	gui_DrawMessageBox();
	switch(interfacial_GetCurMsgType())
	{
		case MESSAGE_SETTING:
			if(setting_GetIsChn())                                                                  //状态栏文字标签
			{
				GUI_PutChnStr(24, 64, msg_content.content_cn, msg_content.content_cn_len, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
			}
			else
			{
				GUI_PutEngStr(20, 64, msg_content.content_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
			}
			NanoOptionList_Print(cur_interfacial.NanoOption_msg, setting_GetIsChn());
			break;
		case MESSAGE_SUCCESSFUL:
			if(flag_is_successful)
			{
				if(setting_GetIsChn())                                                                  //状态栏文字标签
				{
					GUI_PutChnStr(24, 80, (uint8_t *)msg_successful_cn, sizeof(msg_successful_cn), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
				}
				else
				{
					GUI_PutEngStr(24, 80, (uint8_t *)chenggong_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
				}
			}
			else
			{
				if(setting_GetIsChn())                                                                  //状态栏文字标签
				{
					GUI_PutChnStr(24, 80, (uint8_t *)msg_fail_cn, sizeof(msg_fail_cn), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);//这里只能显示 恢复出厂 因为指针大小为4只能显示4个字
				}
				else
				{
					GUI_PutEngStr(24, 80, (uint8_t *)shibai_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
				}
			}
			break;
		case MESSAGE_SAVELOG:
			if(flag_is_successful)
			{
				if(setting_GetIsChn())                                                                  //状态栏文字标签
				{
					GUI_PutChnStr(24, 80, (uint8_t *)msg_SaveSuccessful_cn, sizeof(msg_SaveSuccessful_cn), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);//这里只能显示 恢复出厂 因为指针大小为4只能显示4个字
				}
				else
				{
					GUI_PutEngStr(24, 72, (uint8_t *)datasaved, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
					GUI_PutEngStr(24, 88, (uint8_t *)successfully, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
				}
			}
			else
			{
				if(setting_GetIsChn())                                                                  //状态栏文字标签
				{
					GUI_PutChnStr(24, 80, (uint8_t *)msg_SaveFail_cn, sizeof(msg_SaveFail_cn), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);//这里只能显示 恢复出厂 因为指针大小为4只能显示4个字
				}
				else
				{
					GUI_PutEngStr(24, 80, (uint8_t *)shibai_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
				}
			}
			break;
		
		case MESSAGE_DELETE://数据删除成功
			break;
	}
	
}


void interfacial_SetCurPage(PAGE_NUM temp_page)
{
	cur_PageNum = temp_page;
}
PAGE_NUM interfacial_GetCurPage(void)
{
	return cur_PageNum;
}

//全局按键逻辑 比如菜单和校准按键触发事件
void global_key(void)
{
	if(get_KeyMenuFlag())//菜单键
	{
		clear_KeyMenuFlag();
		interfacial_SetPage(PAGE_1_MENU, PAGE_NOT_BACK);
	}
	if(get_KeyBluFlag())//背光键
	{
		clear_KeyBluFlag();
		HAL_GPIO_TogglePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin);
		
		//log_PrintBytes(0);
//		log_PrintfLogOffset();
		
		//log_SetLogCount(0);/////////////////////////////////////////////////////////////////////////////////////////////这里记得改
	}
	if(get_KeyCalFlag())//校准键
	{
		clear_KeyCalFlag();
		if(interfacial_GetCurPage() == PAGE_0_START && rs485_GetDeviceCount())
		{
			interfacial_SetPage(PAGE_4_CAL, PAGE_NOT_BACK);
		}
		
	}
	if(get_KeyCalLongFlag())//探头kb初始化界面
	{
		//HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
		if(interfacial_GetCurPage() == PAGE_0_START && rs485_GetDeviceCount())
		{
			interfacial_SetPage(PAGE_1_RESETCAL, PAGE_NOT_BACK);
		}
	}
	if(get_KeyOkLongFlag())//保存数据成功弹窗
	{
		if(interfacial_GetCurPage() == PAGE_0_START )//有设备的话 防止主界面是搜索设备
		{
			if(rs485_GetDeviceCount())
			{
				switch(rs485_GetSensorType())
				{
					case TYPE_DO://如果当前查询的设备是do的话
						//不用弹选择弹框 只要弹个成功弹框
						if(get_CurDo() !=NULL && DO_GetIsInit(get_CurDo()))//当前设备不为空的话
						{
							
							if(!flage_FirstOKLong)
							{
								flage_FirstOKLong = 1;
								
								generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(TYPE_DO));
							}
						}
						break;
			
						
						
					default:
						break;
				}
			}
		}
	}
	else
	{
		flage_FirstOKLong = 0;
	}
	
	/*如果有按键被按下的话*/
	if(get_KeyClickedFlag())
	{
		clear_KeyClickedFlag();
		set_BeeOn();
		RTC_UpdateShutDownTime(setting_GetAutoShut());
	}
}


//主循环调用按键轮询
void btn_func(void)
{
	global_key();//这里调用全局按键逻辑
	
	if(btnUpDownFunc != NULL)  //上下按键逻辑
	{
		btnUpDownFunc();
	}
	if(btnOkEscFunc != NULL)   //ok esc按键逻辑
	{
		btnOkEscFunc();
	}
	
	
}

/*选中指定序号的标签*/
void Option_InitByIndex(uint8_t index)
{
	cur_option = cur_interfacial.option_head;
	
	if(cur_option != NULL)//判断不是空链表
	{
		if(cur_option->option_index == index)
		{
			cur_option->IsSelected = SELECTED;
		}
		else
		{
			do
			{
				cur_option = cur_option->next_option;
				if(cur_option->option_index == index)
				{
					cur_option->IsSelected = SELECTED;
					return;
				}
				
			}while(cur_option != cur_interfacial.option_head);
		}
		
		
		
	}
}

/*遍历链表让第一个可选标签为选中状态*/
void CurOption_init(void)
{
	cur_option = cur_interfacial.option_head;
	
	
	if(cur_option != NULL)//判断不是空链表
	{
		if(cur_option->IsCanBeSelected)
		{
			cur_option->IsSelected = SELECTED;
		}
		else
		{
			do
			{
				cur_option = cur_option->next_option;
				if(cur_option->IsCanBeSelected)
				{
					cur_option->IsSelected = SELECTED;
					return;
				}
				
			}while(cur_option != cur_interfacial.option_head);
		}
	}
}
/*只有两个选项的初始化*/
void BinaryOption_init(uint8_t selected)
{
	cur_option = cur_interfacial.option_head;
	if(cur_option != NULL)
	{
		if(selected)
		{
			cur_option->IsSelected = SELECTED; //是被选中
		}
		else
		{
			cur_option = cur_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
		}
	}
}

/*选中第一个nano标签*/
void NanoOption_init(void)
{
	cur_option = cur_interfacial.option_head;
	if(cur_option != NULL)//判断不是空链表
	{
		if(cur_option->son_option != NULL)//第一个标签是可选的
		{			
			cur_NanoOption = cur_option->son_option; //当前小标签指针指向第一个小标签
			cur_NanoOption->IsSelected = SELECTED;
		}
		else//不然的话遍历整个链表找到第一个可选的
		{
			do
			{
				cur_option = cur_option->next_option;
				if(cur_option->son_option != NULL)
				{
					cur_NanoOption = cur_option->son_option; //当前小标签指针指向第一个小标签
					cur_NanoOption->IsSelected = SELECTED;
					return;
				}
				
			}while(cur_option != cur_interfacial.option_head);
		}
	}
}

/*自动关机选项初始化*/
void AutoShutOption_init(uint8_t selected)
{
	cur_option = cur_interfacial.option_head;
	switch (selected)
	{
		case 0:
			cur_option->IsSelected = SELECTED; //是被选中
			break;
		
		case 5:
			cur_option = cur_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
			break;
		
		case 10:
			cur_option = cur_option->next_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
			break;
		
		case 20:
			cur_option = cur_option->next_option->next_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
			break;
	}
}

void AutoLockOption_init(uint8_t selected)
{
	cur_option = cur_interfacial.option_head;
	for(uint8_t i = 0; i < selected; i++)
	{
		cur_option = cur_option->next_option;
	}
	cur_option->IsSelected = SELECTED;
}


/*销毁一个界面*/
void interfacial_Destory(PtrToInterfacial interfacial)
{
	OptionList_Destory(&(interfacial->option_head));  //销毁选项和标签链表
	LabelList_Destory(&(interfacial->label_head));
	
	interfacial->content_chn = NULL;
	interfacial->content_eng = NULL;
}


/*清除当前界面*/
void CurInterfacial_Destory(void)
{
	interfacial_Destory(&cur_interfacial);
}
/*清理当前界面上所有的label*/
void interfacial_ClearLabel(void)
{
	LabelList_Destory(&(cur_interfacial.label_head));
}

/*用来将值更新到气压buf里面*/
void set_PressArr(double press)
{
	snprintf(press_arr, 7, "%5.2f", press + setting_GetAirCompensate());
}
char* get_PressArr(void)
{
	return press_arr;
}

void set_SalArr(float sal)
{
	snprintf(sal_arr, 6, "%5.1f", sal);
}
char* get_SalArr(void)
{
	return sal_arr;
}

void StatusBar_Update(void)
{
	GUI_PutEngStr(0, 0, (uint8_t *)time_arr, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);     //把时间刷新到上面去
	snprintf(time_arr, 6, "%02d:%02d", RTC_GetHour(), RTC_GetMinute());                                     //刷新一下时间数组
	
	switch(rs485_GetSensorType())
	{
		case TYPE_NONE:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_NONE]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
			break;
		
		case TYPE_DO:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_DO]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		
		default:
			break;
	}
	
	gui_DrawStatusBarLine(); //画状态栏的线
}


void value_PlusPlus(uint8_t* data, uint8_t min, uint8_t max)
{
	int8_t temp = *data;
	if(++temp > max)
	{
		temp = min;
	}
	*data = temp;
}
void value_ReduceReduce(uint8_t* data, uint8_t min, uint8_t max)
{
	int8_t temp = *data;
	if(--temp < min)
	{
		temp = max;
	}
	*data = temp;
}
void value_PlusPlus_u16(uint16_t* data, uint16_t min, uint16_t max)
{
	int16_t temp = *data;
	if(++temp > max)
	{
		temp = min;
	}
	*data = temp;
}
void value_ReduceReduce_u16(uint16_t* data, uint16_t min, uint16_t max)
{
	int16_t temp = *data;
	if(--temp < min)
	{
		temp = max;
	}
	*data = temp;
}


//这里用来清标志的  否则你在空白界面上按的操作会直接作用在下个界面 标志没清
void btn_OkEscMode_NULL(void)
{
	if(get_KeyOkFlag())    
	{
		clear_KeyOkFlag();
		
		if(interfacial_GetCurPage() == PAGE_0_START)//如果在初始界面有开锁功能的话
		{
			switch(rs485_GetSensorType())
			{
				case TYPE_DO:
					if(get_CurDo() != NULL )//有溶解氧的话
					{
						if(setting_GetAutoLock() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
						{
							DO_SetValueLocked(get_CurDo());    //锁住
							break;
						}
						if(setting_GetAutoLock() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
						{
							clear_DOShakeCount();              //清除抖动计数
							DO_SetValueUnlocked(get_CurDo());  //解锁
						}
						
					}
					break;
				
				default:
					break;
			}
		}
	}
	if(get_KeyEscFlag())   clear_KeyEscFlag();
}
void btn_UpDownMode_NULL(void)
{
	if(get_KeyUpFlag())    clear_KeyUpFlag();
	if(get_KeyDownFlag())  clear_KeyDownFlag();
}


//上下键切换选项的逻辑
void btn_UpDownMode_ChangeOption(void)
{
	PtrToOptionNode p = cur_option;
	if(cur_option == NULL || cur_interfacial.option_head == NULL || cur_option == NULL || cur_option->IsSelected == 0)
	{
		clear_KeyUpFlag();
		clear_KeyDownFlag();
		return;
	}
	
	if(get_KeyUpFlag())
	{
		clear_KeyUpFlag();
		//遍历找到上一个可以被选中的标签如果找不到的话就保持不变
		do
		{
			p = p->prev_option;
		}while(p != cur_option && !(p->IsCanBeSelected));
		
		if(p != cur_option)
		{
			p->IsSelected = SELECTED;
			cur_option->IsSelected = DESELECTED;
			cur_option = p;
		}
	}
	if(get_KeyDownFlag())
	{
		clear_KeyDownFlag();
		do
		{
			p = p->next_option;
		}while(p != cur_option && !(p->IsCanBeSelected));
		
		if(p != cur_option)
		{
			p->IsSelected = SELECTED;
			cur_option->IsSelected = DESELECTED;
			cur_option = p;
		}
	}
	window_change(cur_option, cur_interfacial.option_head);  //滑动一下窗口
}

//ok esc切换界面的逻辑
void btn_OkEscMode_ChangePage(void)
{
	uint32_t STD_temp = 0;
	if(get_KeyEscFlag())    //取消就要回到父界面
	{
		clear_KeyEscFlag();
		if(cur_interfacial.page_father != NULL)
		{
			//特殊的界面添加的操作
			if(cur_PageNum == PAGE_3_SENSORSSEARCH)
			{
				rs485_ClearCircularSentStatus();
			}
			
			/*跳转到父界面*/
			interfacial_SetPage(cur_interfacial.page_father, PAGE_IS_BACK);
			
		}
	}
	
	/*安全检测，如果当前选中的标签是空的话就直接退出*/
	if(cur_interfacial.option_head == NULL || cur_option == NULL || cur_option->IsSelected == 0)
	{
		clear_KeyOkFlag();
		return;
	}
	
	if(get_KeyOkFlag())    //确定之后就要跳转到p的nextpage
	{
		clear_KeyOkFlag();
		
		if(cur_option->next_page != NULL)
		{
			
			/*特殊界面添加特殊逻辑*/
			if(interfacial_GetCurPage() == PAGE_2_SENSORMANAGE)//传感器管理界面保存设备名字指针
			{
				if(cur_option->content_eng != NULL)
				{
					option_sensor_name = cur_option->content_eng;
				}
			}

			if(interfacial_GetCurPage() == PAGE_3_ALARM_TYPE)
			{
				switch(cur_option->content_eng[0])
				{
					case 'B':
						alarm_SensorType = TYPE_Bga;
						break;
					
					case 'C':
						switch(cur_option->content_eng[1])
						{
							case 'L':
								alarm_SensorType = TYPE_CL;
								break;
							
							case 'h':
								alarm_SensorType = TYPE_Chl;
								break;
							
							case 'O':
								alarm_SensorType = TYPE_CODuv;
								break;
						}
						
						break;
					
					case 'D':
						alarm_SensorType = TYPE_DO;
						break;
					
					case 'E':
						alarm_SensorType = TYPE_EC;
						break;
					
					case 'F':
						if(cur_option->content_eng[1] == 'C')
						{
							alarm_SensorType = TYPE_FCL;
						}
						else
						{
							alarm_SensorType = TYPE_F;
						}
						break;
					
					case 'N':
						alarm_SensorType = TYPE_NH4;
						break;
					
					case 'O':
						alarm_SensorType = TYPE_ORP;
						break;
					
					case 'p':
						alarm_SensorType = TYPE_pH;
						break;
					
					case 'T':
						alarm_SensorType = TYPE_Tur;
						break;
				}
			}
			
			/*保存标签下标方便返回界面时候能够跳到哪个选项*/
			if(interfacial_GetCurPage() == PAGE_1_MENU)
			{
				AncestorPage_OptionIndex = interfacial_GetCurrentOption()->option_index;//爷爷界面的
			}
			if(interfacial_GetCurPage() >= PAGE_2_SENSORMANAGE && interfacial_GetCurPage() <= PAGE_2_HISTORY)
			{
				GrandpaPage_OptionIndex = interfacial_GetCurrentOption()->option_index;
			}
			if(interfacial_GetCurPage() == PAGE_3_SENSORS)
			{
				FatherPage_OptionIndex = interfacial_GetCurrentOption()->option_index;
			}
			
			/*切换界面*/
			interfacial_SetPage(cur_option->next_page, PAGE_NOT_BACK);
		}
		else//如果没有需要转跳的界面的话
		{
			if(cur_option->son_option != NULL)//如果有子选项的话
			{
				if(interfacial_GetCurPage() == PAGE_5_ONE || interfacial_GetCurPage() == PAGE_5_TWOFIRST || interfacial_GetCurPage() == PAGE_5_TWOSECOND)//单点校准中确定键是进入校准模式
				{//校准模式逻辑
					switch(temp_SensorType)
					{
						case TYPE_DO:
							
							STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);
						
							snprintf(cal_arr, 8, "%05.01f %%", STD_temp / 10.0);//将校准值写入校准文字buff
						
							STD_value = (STD_temp == 0 ? CAL_ZERO_VALUE : STD_temp / 10.0);//计算校准的值
						
							OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表

							gui_ClearLines(75, 93, 0);//清开始校准的选项
						
							LabelList_Add(52, 56, NULL, 0, (uint8_t *)cal_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							LabelList_Add( 0, 76, (uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
							LabelList_Add( 0, 96, (uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。
							LabelList_Add( 56,  136, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,   LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));  //do %
							
							set_RowSpacing(84);
							OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
							CurOption_init();
						break;
						
						
						default:
							break;
					}
					
					
					
				}
				else
				{
					cur_option->IsSelected = DESELECTED;//大标签选中取消				
					cur_NanoOption = cur_option->son_option; //当前小标签指针指向第一个小标签
					cur_NanoOption->IsSelected = SELECTED;
					
					btnUpDownFunc_register(btn_UpDownMode_ChangeValue);//注册新的按键逻辑
					btnOkEscFunc_register(btn_OkEscMode_ChangeOption);//注册新的按键逻辑
				}
			}
			else//就是save的情况得弹窗
			{
				switch (interfacial_GetCurPage())
				{
					case PAGE_3_DATASAVE:
						if(rs485_GetDeviceCount())//有设备的话 防止主界面是搜索设备
						{
							switch(rs485_GetSensorType())
							{
								case TYPE_DO://如果当前查询的设备是do的话
									//不用弹选择弹框 只要弹个成功弹框
									if(get_CurDo() !=NULL && DO_GetIsInit(get_CurDo()))//当前设备不为空的话
									{
										generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(TYPE_DO));
									}
									break;
						
									
									
								default:
									break;
							}
						}
						break;
					
					case PAGE_3_DATADELETE:
						msg_content.content_cn = (uint8_t *)msg_DelData_cn;
						msg_content.content_en = (uint8_t *)msg_DelData_en;
						msg_content.content_cn_len = sizeof(msg_DelData_cn);
						generate_MessageBox(MESSAGE_SETTING, 1);//显示弹窗
						break;
					
					case PAGE_3_RESERT:
						msg_content.content_cn = (uint8_t *)msg_reset_cn;
						msg_content.content_en = (uint8_t *)msg_reset_en;
						msg_content.content_cn_len = sizeof(msg_reset_cn);
						generate_MessageBox(MESSAGE_SETTING, 1);//显示弹窗
						break;
					
					case PAGE_5_ZERO:
					case PAGE_5_ONE:
					case PAGE_5_TWOFIRST:
					case PAGE_5_TWOSECOND:
						msg_content.content_cn = (uint8_t *)msg_ConfirmCal_cn;
						msg_content.content_en = (uint8_t *)msg_ConfirmCal_en;
						msg_content.content_cn_len = sizeof(msg_ConfirmCal_cn);
						generate_MessageBox(MESSAGE_SETTING, 1);//显示弹窗
						break;
					
					default:
						msg_content.content_cn = (uint8_t *)msg_save_cn;
						msg_content.content_en = (uint8_t *)msg_save_en;
						msg_content.content_cn_len = sizeof(msg_save_cn);
						generate_MessageBox(MESSAGE_SETTING, 1);//显示弹窗
						break;
				}
				
			}
		}
	}
}

void btn_UpDownMode_ChangePage(void)//主要用于帽膜和数据记录翻页
{
	if(get_KeyUpFlag())
	{
		clear_KeyUpFlag();
		switch(interfacial_GetCurPage())
		{
			case PAGE_4_DATALOG:
				cur_PageNum = PAGE_4_DATALOG_GPS;
				generate_DataLogGPS(&cur_interfacial);
				gui_ClearLines(32, 160, 0);
				break;
			case PAGE_4_DATALOG_GPS:
				cur_PageNum = PAGE_4_DATALOG;
				generate_DataLog(&cur_interfacial);
				gui_ClearLines(32, 160, 0);
				//interfacial_SetPage(interfacial_GetCurPage() == PAGE_4_DATALOG ? PAGE_4_DATALOG_GPS : PAGE_4_DATALOG);
				break;
			
			default:
				break;
		}
	}
	if(get_KeyDownFlag())
	{
		clear_KeyDownFlag();
		switch(interfacial_GetCurPage())
		{
			case PAGE_4_DATALOG:
				cur_PageNum = PAGE_4_DATALOG_GPS;
				generate_DataLogGPS(&cur_interfacial);
				gui_ClearLines(32, 160, 0);
				break;
			case PAGE_4_DATALOG_GPS:
				cur_PageNum = PAGE_4_DATALOG;
				generate_DataLog(&cur_interfacial);
				gui_ClearLines(32, 160, 0);
				//interfacial_SetPage(interfacial_GetCurPage() == PAGE_4_DATALOG ? PAGE_4_DATALOG_GPS : PAGE_4_DATALOG);
				break;
			
			default:
				break;
		}
	}
}

void update_LogData(void)
{
	log_ReadData(generate_GetLogUnion(), cur_LogIndex - 1);
	
	update_LogTitle(generate_GetLogUnion(), cur_LogIndex);
//	snprintf((char *)generate_GetLogTimeArr(), 19, "%05d %02d%02d%02d %02d:%02d", cur_LogIndex,
//																																								generate_GetLogUnion()->log.time.years,
//																																								generate_GetLogUnion()->log.time.month,
//																																								generate_GetLogUnion()->log.time.day,
//																																								generate_GetLogUnion()->log.time.hour,
//																																								generate_GetLogUnion()->log.time.minute);
	interfacial_UpdateCurLogIndexArr(cur_LogIndex);
}

void check_TimeValue(void)
{
	if(cur_option == cur_interfacial.option_head && cur_NanoOption != cur_interfacial.option_head->son_option->next_option->next_option)//不为日标签时
	{
		if(cur_interfacial.option_head->son_option->next_option->next_option->value > cur_interfacial.option_head->son_option->next_option->next_option->max_value)
		{
			cur_interfacial.option_head->son_option->next_option->next_option->value = cur_interfacial.option_head->son_option->next_option->next_option->max_value;
		}
	}
}
void update_TimeValue(void)
{
	if(interfacial_GetCurPage() == PAGE_3_TIME)
	{
		if(cur_NanoOption == cur_interfacial.option_head->son_option)//年
		{
			uint16_t year = 2000 + cur_NanoOption->value;
			if(year % 4)//平年
			{
				days[1] = 28;
			}
			else//闰年
			{
				days[1] = 29;
			}
			cur_interfacial.option_head->son_option->next_option->next_option->max_value = days[cur_interfacial.option_head->son_option->next_option->value - 1];
		}
		if(cur_NanoOption == cur_interfacial.option_head->son_option->next_option)//月
		{
			cur_NanoOption->next_option->max_value = days[cur_NanoOption->value - 1];
		}
		check_TimeValue();
	}
}

//上下键切换数值的逻辑
void btn_UpDownMode_ChangeValue(void)
{
	if(get_KeyUpFlag())
	{
		clear_KeyUpFlag();
		
		switch(interfacial_GetCurPage())
		{
			case PAGE_3_DATASHOW:
				if(log_GetLogCount())
				{
					value_PlusPlus_u16(&cur_LogIndex, 1, log_GetLogCount());
					update_LogData();
				}
				break;
			case PAGE_3_TIME:
				value_PlusPlus(&(cur_NanoOption->value), cur_NanoOption->min_value, cur_NanoOption->max_value);
				update_TimeValue();
				break;
			

			default:
				value_PlusPlus(&(cur_NanoOption->value), cur_NanoOption->min_value, cur_NanoOption->max_value);
				break;
		}
		
	}
	if(get_KeyDownFlag())
	{
		clear_KeyDownFlag();
		switch(interfacial_GetCurPage())
		{
			case PAGE_3_DATASHOW:
				if(log_GetLogCount())
				{
					value_ReduceReduce_u16(&cur_LogIndex, 1, log_GetLogCount());
					update_LogData();
					
				}
				break;
			case PAGE_3_TIME:
				value_ReduceReduce(&(cur_NanoOption->value), cur_NanoOption->min_value, cur_NanoOption->max_value);
				update_TimeValue();
				break;
				
			default:
				value_ReduceReduce(&(cur_NanoOption->value), cur_NanoOption->min_value, cur_NanoOption->max_value);
				break;
		}
		
	}
}

//ok esc选择修改项的逻辑
void btn_OkEscMode_ChangeOption(void)
{
	//当前子标签转跳到下一级 如果到队尾了直接转跳到父列表
	if(get_KeyOkFlag())
	{
		clear_KeyOkFlag();
		
		if(interfacial_GetCurPage() == PAGE_3_TIME && cur_NanoOption == cur_interfacial.option_head->son_option)//年
		{
			uint16_t year = 2000 + cur_NanoOption->value;
			if(year % 4)//平年
			{
				days[1] = 28;
			}
			else//闰年
			{
				days[1] = 29;
			}
			cur_interfacial.option_head->son_option->next_option->next_option->max_value = days[cur_interfacial.option_head->son_option->next_option->value - 1];
		}
		
		cur_NanoOption->IsSelected = DESELECTED;
		cur_NanoOption = cur_NanoOption->next_option;
		cur_NanoOption->IsSelected = SELECTED;
		
		if(cur_NanoOption == cur_option->son_option)//到头了的话
		{
			cur_NanoOption->IsSelected = DESELECTED;
			cur_option->IsSelected = SELECTED;
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
		}
		
	}
	//当前子标签指针转跳到上一级 如果到对头了转跳到父列表
	if(get_KeyEscFlag())
	{
		clear_KeyEscFlag();
		
		cur_NanoOption->IsSelected = DESELECTED;
		cur_NanoOption = cur_NanoOption->prev_option;
		cur_NanoOption->IsSelected = SELECTED;
		
		if(cur_NanoOption == cur_option->son_option->prev_option)//就是到nano option的头的话返回要返回大链表
		{
			cur_NanoOption->IsSelected = DESELECTED;
			cur_option->IsSelected = SELECTED;
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
		}
	}
}
uint8_t checked_AlarmValueLegal(void)
{
	uint16_t temp_high;
	uint16_t temp_low;
	
	
	temp_low = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);
	temp_high = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);



	if(temp_high != 0 && temp_low != 0)
	{
		if(temp_low >= temp_high)
		{
			return 0;
		}
	}
	return 1;
}



/*在msgbox中ok 和 esc按键的逻辑*/
void btn_OkEscMode_MsgBox(void)
{
	if(get_KeyOkFlag())/******************************************************************************保存设置*/
	{
		clear_KeyOkFlag();
		
		cur_NanoOption->IsSelected = DESELECTED;//取消选中
		//destory_MessageBox();
		if(cur_NanoOption == cur_interfacial.NanoOption_msg)//如果是是的话
		{
			save_setting();
			
			switch(interfacial_GetCurPage())
			{
				case PAGE_5_ALARMVALUE://这里的话要判断是否合法
					generate_MessageBox(MESSAGE_SUCCESSFUL, checked_AlarmValueLegal());
					break;
				
				case PAGE_1_RESETCAL:
				case PAGE_4_SENSORINFO: //传感器信息用来修改传感器modbus id
				case PAGE_5_ONE:        //先不用弹窗通过串口数据去弹窗
				case PAGE_5_SENSORCAP:
				case PAGE_5_THREE:
				case PAGE_5_TWOFIRST:
				case PAGE_5_TWOSECOND:
				case PAGE_5_ZERO:
					break;
				
				default:
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);//显示成功界面
					break;
			}
			
			
		}
		else
		{
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			destory_MessageBox();
		}
	}
	if(get_KeyEscFlag())
	{
		clear_KeyEscFlag();
		cur_NanoOption->IsSelected = DESELECTED;//取消选中
		btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
		btnOkEscFunc_register(btn_OkEscMode_ChangePage);
		destory_MessageBox();
	}
}
void btn_UpDownMode_MsgBox(void)
{
	if(get_KeyUpFlag())
	{
		clear_KeyUpFlag();
		cur_NanoOption->IsSelected = DESELECTED;
		cur_NanoOption = cur_NanoOption->prev_option;
		cur_NanoOption->IsSelected = SELECTED;
	}
	if(get_KeyDownFlag())
	{
		clear_KeyDownFlag();
		cur_NanoOption->IsSelected = DESELECTED;
		cur_NanoOption = cur_NanoOption->next_option;
		cur_NanoOption->IsSelected = SELECTED;
	}
}



/*点击保存后的操作*/
/*根据界面来进行相应操作*/
void save_setting(void)
{
	PtrToDOProbe p;
	uint32_t temp_value = 0;
	datetime_t temp_time;
	
  static	float compensate_k_temp = 1.0;
  static	float compensate_b_temp = 0.0;
	
	static	float value0 = 0.0;//校准值
	static	float value1 = 0.0;
	
	static	float real0 = 0.0;//实际值
	static	float real1 = 0.0;
	
	
	double double_value;
	
	switch(cur_PageNum)
	{
		case PAGE_1_RESETCAL:
			destory_MessageBox();
			switch(rs485_GetSensorType())
			{
				case TYPE_DO:
					DO_rs485_SetKB(get_CurDo(), 1.0, 0.0); //串口设置kb 1 0 
					break;
				
				default:
					break;
			}
			break;
		
		case PAGE_3_TIME://设置时间界面
		
			temp_time.week = 1;
		
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 100);
			temp_time.years = temp_value / 10000;
			temp_time.month = (temp_value % 10000) / 100;
			temp_time.day = temp_value % 100;
		
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 100);
			temp_time.hour = temp_value / 10000;
			temp_time.minute = (temp_value % 10000) / 100;
			temp_time.seconds = temp_value % 100;
		
			HYM8563_SetTime(&temp_time);
			
			break;
		
		case PAGE_3_LANGUAGE://语言设置界面
			setting_SetIsChn(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;
		
		case PAGE_3_AUTOSHUT:
			
			if(cur_option == cur_interfacial.option_head)
			{
				setting_SetAutoShut(0);
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				setting_SetAutoShut(5);
			}
			else if(cur_option == cur_interfacial.option_head->next_option->next_option)
			{
				setting_SetAutoShut(10);
			}
			else if(cur_option == cur_interfacial.option_head->prev_option)
			{
				setting_SetAutoShut(20);
			}
			
			SettingToFlash();//保存一波设置
			break;
		
		
		
		case PAGE_3_PRESSURE://设置当前气压值
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->next_option->son_option, 10);
			double_value = ((double)temp_value/100.0);
		
			setting_SetAirCompensate(double_value - bmp280_GetPress());
			SettingToFlash();
		
			DO_rs485_SetPressure(get_CurDo(), double_value);//更新do设备的气压值

			
			break;
		
		case PAGE_3_SALT://设置盐度值
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->next_option->son_option, 10);
			double_value = ((double)temp_value/10.0);
		
			setting_SetSalinity(double_value);  //更新设置中的盐度
			SettingToFlash();                   //将设置写入flash
		
			set_SalArr(setting_GetSalinity()); //刷新盐度显示的buf
		  
			DO_rs485_SetSalinity(get_CurDo(), double_value);
		  
			break;
		
		case PAGE_3_AUTOLOCK:
			if(cur_option == cur_interfacial.option_head)
			{
				setting_SetAutoLock(AUTOLOCK_OFF);
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				setting_SetAutoLock(AUTOLOCK_AUTO);
			}
			else if(cur_option == cur_interfacial.option_head->prev_option)
			{
				setting_SetAutoLock(AUTOLOCK_MANUAL);
			}
			
			SettingToFlash();//保存一波设置
			
			if(get_CurDo() != NULL )//恢复出厂设置的时候如果有设备被锁的话解锁
			{
				if(DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
				{
					clear_DOShakeCount();
					DO_SetValueUnlocked(get_CurDo());
				}
			}
			break;
		
		case PAGE_3_RESERT:
			if(cur_option == cur_interfacial.option_head)
			{
				clear_KeyAllFlag();
				if(setting_GetIsChn())//如果是中文的话得清一下上面的标签显示的文字
				{
					gui_ClearChuchangshezhi();
				}
				setting_reset();
				
				if(get_CurDo() != NULL )//恢复出厂设置的时候如果有设备被锁的话解锁
				{
					if(DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
					{
						clear_DOShakeCount();              //清除抖动计数
						DO_SetValueUnlocked(get_CurDo());  //解锁
					}
					
				}
			}
			break;
			
		case PAGE_3_DATADELETE:
			log_SetLogCount(0);
			break;
		
		case PAGE_4_KEYPADTONE://设置按键音
			setting_SetKeyPadTone(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;
		
		case PAGE_4_ALARMTONE://设置报警音
			setting_SetAlarmTone(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;
		
		case PAGE_4_ALARM:
			switch(alarm_SensorType)
			{
				case TYPE_DO:
					setting_SetIsAlarm_DO(0);
					SettingToFlash();//保存一波设置
					break;
				
				case TYPE_Bga:
					break;
				
				case TYPE_Chl:
					break;
				
				case TYPE_CL:
					break;
				
				case TYPE_CODuv:
					break;
				
				case TYPE_EC:
					break;
				
				case TYPE_F:
					break;
				
				case TYPE_FCL:
					break;
				
				case TYPE_NH4:
					break;
				
				case TYPE_ORP:
					break;
				
				case TYPE_pH:
					break;
				
				case TYPE_Tur:
					break;
				
				default:
					break;
			}
			
			break;
		

		
		case PAGE_4_SENSORINFO://设备信息  设置modbus id
			destory_MessageBox();
			p = DO_FindByName(interfacial_GetOptionSensorName(), rs485_GetDoList());//获取当前设备指针
		
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->next_option->next_option->son_option, 100);
			temp_value = ((temp_value/100)<<4)|(temp_value % 100); // 获取要设置的地址
				
			DO_rs485_SetAddr(p, temp_value);
			
			break;
		
		case PAGE_5_ALARMVALUE://设置是否报警 高低门限阈值 这里要判断一波
			switch(alarm_SensorType)
			{
				case TYPE_DO:
					setting_SetIsAlarm_DO(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_DO((value_type)temp_value/100.0);

						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_DO((value_type)temp_value/100.0);
						SettingToFlash();//保存一波设置
					}
					break;
				
				case TYPE_Bga:
					break;
				
				case TYPE_Chl:
					break;
				
				case TYPE_CL:
					break;
				
				case TYPE_CODuv:
					break;
				
				case TYPE_EC:
					break;
				
				case TYPE_F:
					break;
				
				case TYPE_FCL:
					break;
				
				case TYPE_NH4:
					break;
				
				case TYPE_ORP:
					break;
				
				case TYPE_pH:
					break;
				
				case TYPE_Tur:
					break;
				
				default:
					break;
			}
			

			break;
		
		case PAGE_5_ONE:
			//不同界面进来的可能设备也不一样区别一下
			switch(temp_SensorType)
			{
				case TYPE_DO:
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
					{
						compensate_k_temp = DO_GetKFloat(p) * (STD_value / 100.0 / DO_GetDOPercent(p));//计算值写值
						
						if(get_CurDo() != NULL && fabs(compensate_k_temp - 1.0) <= MAX_EPS_K)
						{
							DO_rs485_SetK(get_CurDo(), compensate_k_temp);
						}
						else
						{
							interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn;//校准失败
							interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;
							interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
							
							generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
							
							cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);
						}
					}
					break;
					
				
				
				default:
					break;
			}
			break;
		
		case PAGE_5_TWOFIRST:
			switch(temp_SensorType)
			{
				case TYPE_DO:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
					{
						value0 = STD_value / 100.0;
						real0 = (DO_GetDOPercent(p) - DO_GetBFloat(p)) / DO_GetKFloat(p);
						
						interfacial_GetCurrentOption()->content_chn = (uint8_t *)xiayidian_cn;//显示下一页
						interfacial_GetCurrentOption()->ChnContent_size = sizeof(xiayidian_cn);
						interfacial_GetCurrentOption()->content_eng = (uint8_t *)xiayidian_en;
						
						interfacial_GetCurrentOption()->next_page = PAGE_5_TWOSECOND;
						
						btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
						btnOkEscFunc_register(btn_OkEscMode_ChangePage);
					}
					break;
					
				
				
				default:
					break;
			}
			break;
			
		case PAGE_5_TWOSECOND:
			switch(temp_SensorType)
			{
				case TYPE_DO:
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
					gui_ClearLines(115, 133, 0);//清除选项
				
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //DO_ValueCheckSecond(p, STD_value)
					{
						value1 = STD_value / 100.0;
						real1 = (DO_GetDOPercent(p) - DO_GetBFloat(p)) / DO_GetKFloat(p);
						
						compensate_k_temp = (value0 - value1)/(real0 - real1);
						compensate_b_temp = value1 - compensate_k_temp * real1;
						
						
						
						if(get_CurDo() != NULL && fabs(compensate_k_temp - 1.0) <= MAX_EPS_K && fabs(compensate_b_temp) <= MAX_EPS_B)
						{
							DO_rs485_SetKB(get_CurDo(), compensate_k_temp, compensate_b_temp); //设置kb值
						}
						else//如果这个时候断开设备的话
						{
							interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
							interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
							interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
				
							generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
							cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
						}
					}
					break;
					
				
				
				default:
					break;
			}
			break;
			
			
			
			
			
			
			
			
			
			
		
		default:
			break;
	}
}


//这里这是初始化界面 按键逻辑只是一开始的逻辑
void interfacial_SetPage(PAGE_NUM page_num, uint8_t IsBack)
{
	
	
	set_OptionTop(NULL);   //清空窗口
	set_OptionTail(NULL);
	
//	btnUpDownFunc_register(NULL);//清空绑定防止没绑定调用函数溢出    这个得最后再加不然有界面没写按回车直接拉闸就无法操作
//	btnOkEscFunc_register(NULL);
	
	
	
	switch(page_num)                     //判断传入页面来初始化一些东西
	{
		case PAGE_0_START://初始界面
		  generate_StartPage(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);//这里要改成切换传感器
			btnOkEscFunc_register(btn_OkEscMode_NULL);//清标志
			rs485_SetIsChangeSenesor(); //让它重新生成一下节点
			DO_SetTempZero();
			break;
		
		case PAGE_1_MENU://菜单界面
			generate_MenuPage(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(IsBack)
			{
				Option_InitByIndex(AncestorPage_OptionIndex);
				
			}
			else
			{
				CurOption_init();
			}
			break;
		
		case PAGE_1_RESETCAL: //初始化校准界面
			generate_ResetCal(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_2_SYSTEM://系统设置界面
			generate_SystemPage(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(IsBack)
			{
				Option_InitByIndex(GrandpaPage_OptionIndex);
				refresh_window(cur_option, cur_interfacial.option_head);
			}
			else
			{
				CurOption_init();
			}
			break;
		
		case PAGE_2_SENSORMANAGE://传感器管理界面    列举已连接的传感器
			generate_SensorManage(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(IsBack)
			{
				Option_InitByIndex(GrandpaPage_OptionIndex);
			}
			else
			{
				CurOption_init();
			}
			
			break;
		
		case PAGE_2_HISTORY:
			generate_Histor(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(IsBack)
			{
				Option_InitByIndex(GrandpaPage_OptionIndex);
			}
			else
			{
				CurOption_init();
			}
			break;
		
		case PAGE_3_SENSORS://具体传感器界面 
			generate_Sensors(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(IsBack)
			{
				Option_InitByIndex(FatherPage_OptionIndex);
			}
			else
			{
				CurOption_init();
			}
			break;
		
		case PAGE_3_SENSORSSEARCH://搜索传感器界面
			generate_SensorSearch(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			//这里应该要升级一下就是要搜索不同种设备，没种查modbus 发5秒这样
			DO_rs485_GetModbusId();              //将获取modbus id的指令写入到写buf
			rs485_SetCircularSentStatus(); //循环发送
			break;
		
		case PAGE_3_TIME://时间设置界面
			generate_SetTimePage(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_BEEP://提示音
			generate_BeepSeting(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_LANGUAGE://语言设置界面
			generate_Language(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(setting_GetIsChn());
			break;
		
		case PAGE_3_AUTOSHUT://自动关机
			generate_AutoShut(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoShutOption_init(setting_GetAutoShut());
			break;
		
		case PAGE_3_ALARM_TYPE:
			generate_AlarmType(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_PRESSURE://气压设置
			generate_SetPressure(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		case PAGE_3_SALT://盐度设置
			generate_SetSalinity(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_GPS://gps查询
			generate_GpsInfo(&cur_interfacial);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			CurOption_init();
			break;
		
		case PAGE_3_INFO:
			generate_MeterInfo(&cur_interfacial);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			CurOption_init();
			break;
		
		case PAGE_3_RESERT://恢复出厂设置
			generate_reset(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_AUTOLOCK://自动锁定
			generate_AutoLock(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoLockOption_init(setting_GetAutoLock());
			break;
		
		case PAGE_3_DATASAVE:
			generate_DataStore(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_DATASHOW://数据记录显示
			if(interfacial_GetCurPage() == PAGE_2_HISTORY)//从历史数据界面进去默认指向最后一条
			{
				cur_LogIndex = log_GetLogCount();
			}
			
			generate_DataView(&cur_interfacial, cur_LogIndex);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_DATADELETE:
			generate_DataDelete(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_4_ALARMTONE://报警提示音
			generate_AlarmTone(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(setting_GetAlarmTone());
			break;
		
		case PAGE_4_KEYPADTONE://按键音
			generate_KeypadTone(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(setting_GetKeyPadTone());
			break;
		
		case PAGE_4_ALARM://报警设置
			generate_AlarmSetting(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			switch(alarm_SensorType)
			{
				case TYPE_DO:
					BinaryOption_init(setting_GetIsAlarm_DO());
					break;
				case TYPE_pH:
					break;
				case TYPE_Tur:
					break;
				case TYPE_FCL:
					break;
				case TYPE_EC:
					break;
				case TYPE_ORP:
					break;
				case TYPE_NH4:
					break;
				case TYPE_F:
					break;
				case TYPE_CL:
					break;
				case TYPE_Chl:
					break;
				case TYPE_Bga:
					break;
				case TYPE_CODuv:
					break;
				
				default:
					break;
				
			}
		
			//
			break;
		
		case PAGE_4_CAL://校准界面
			if(interfacial_GetCurPage() == PAGE_0_START || interfacial_GetCurPage() == PAGE_3_SENSORS)
			{
				temp_FatherPage = interfacial_GetCurPage();
			}
			generate_Cal(&cur_interfacial,temp_FatherPage);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_4_PARASET:
			generate_ParameterSetting(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_4_SENSORINFO://传感器信息
			generate_SensorInfo(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_4_DATALOG:
			generate_DataLog(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangePage);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		case PAGE_4_DATALOG_GPS:
			generate_DataLogGPS(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangePage);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		
		case PAGE_5_ALARMVALUE://高低门限报警值
			generate_AlarmValueSetting(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_5_ONE://单点校准
			if(temp_FatherPage == PAGE_3_SENSORS)
			{
				if(interfacial_GetOptionSensorName()[0] == 'D')
				{
					temp_SensorType = TYPE_DO;
				}
			}
			else if(temp_FatherPage == PAGE_0_START)
			{
				temp_SensorType = rs485_GetSensorType();
			}
			generate_OnePoint(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;
		
		case PAGE_5_TWOFIRST://两点校准的第一个点
			if(temp_FatherPage == PAGE_3_SENSORS)
			{
				if(interfacial_GetOptionSensorName()[0] == 'D')
				{
					temp_SensorType = TYPE_DO;
				}
			}
			else if(temp_FatherPage == PAGE_0_START)
			{
				temp_SensorType = rs485_GetSensorType();
			}
			generate_TwoPointFirst(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;
			
		case PAGE_5_TWOSECOND://两点校准的第二个点
			if(temp_FatherPage == PAGE_3_SENSORS)
			{
				if(interfacial_GetOptionSensorName()[0] == 'D')
				{
					temp_SensorType = TYPE_DO;
				}
			}
			else if(temp_FatherPage == PAGE_0_START)
			{
				temp_SensorType = rs485_GetSensorType();
			}
			generate_TwoPointSecond(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
		default:
			break;
	}
	gui_ClearLines(16, 160, 0);//除状态栏外都清一遍
	gui_DrawStatusBarLine();
	set_StartPoint(16);//设置从状态栏那边开始刷取得最快的menu显示
	
	interfacial_SetCurPage(page_num);
}


void interfacial_InitMsg(void)//生成是否两个按键,这个是界面里一直有的
{
	list_NanoOption IsOrNo = NULL;   //是否标签
	
	NanoOptionList_Add(32, 96, (uint8_t *)&shi_cn, 1, (uint8_t *)shi_en, NANOOPTION_BUTTON,  0, 0, 9, IS_SINGLE,  &IsOrNo);     //是
	NanoOptionList_Add(104, 96, (uint8_t *)&fou_cn, 1, (uint8_t *)fou_en, NANOOPTION_BUTTON,  0, 0, 9, IS_SINGLE,  &IsOrNo);    //否
	
	cur_interfacial.NanoOption_msg = IsOrNo;
}

Interfacial_t* interfacial_GetCurrentInterfacial(void)
{
	return &cur_interfacial;
}

PtrToOptionNode interfacial_GetCurrentOption(void)
{
	return cur_option;
}

uint8_t* interfacial_GetOptionSensorName(void)
{
	return option_sensor_name;
}



void DO_SearchValue(void)
{
	if(!rs485_GetCircularSentStatus())//确保没有别的东西下发或者在查询发送查询温度和溶解氧的值
	{
//		if(temp_FatherPage == PAGE_0_START)
//		{
//			DO_rs485_GetTempTwoDO(get_CurDo());
//		}
//		else if(temp_FatherPage == PAGE_3_SENSORS)
//		{
//			DO_rs485_GetTempTwoDO(DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList()));
//		}
		if(temp_FatherPage == PAGE_3_SENSORS)
		{
			DO_rs485_GetTempTwoDO(DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList()));
		}
		else
		{
			DO_rs485_GetTempTwoDO(get_CurDo());
		}
	}
}

static uint8_t location = 0;
void clear_NoSignal(void)
{
	gui_ClearNoSignal(setting_GetIsChn(), 32+location*8);
}


void no_signal(void)
{
	if(cur_interfacial.label_head == NULL)
	{
		location = 0;
		LabelList_Add((setting_GetIsChn() ? 48 : 44), 32, (uint8_t *)wuxinghao_cn, sizeof(wuxinghao_cn), (uint8_t *)wuxinghao_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(cur_interfacial.label_head));
	}
	else
	{
		clear_NoSignal();
		value_PlusPlus(&location, 0, 14);
		cur_interfacial.label_head->y = 32+location*8;
	}
	set_StartPoint(0);
}


/*通过一个全局的界面指针变量来设置刷新界面*/
void interfacial_refresh(void)                                                            //刷新界面 包括生成状态栏和设置界面
{
	uint8_t temp_warning = 0;
	StatusBar_Update();
	
	if(cur_PageNum != PAGE_3_SENSORS) //传感器界面文字只有英文
	{
		if(setting_GetIsChn())                                                                  //状态栏文字标签
		{
			GUI_PutChnStr(8, 16, cur_interfacial.content_chn, cur_interfacial.ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
		}
		else
		{
			GUI_PutEngStr(8, 16, cur_interfacial.content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
		}
	}
	else
	{
		GUI_PutEngStr(8, 16, cur_interfacial.content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
	}

	if(rs485_GetDeviceCount())
	{
		switch(rs485_GetSensorType())
		{
			case TYPE_DO:
				DO_SearchValue();
			
				switch(interfacial_GetCurPage())
				{
					case PAGE_0_START:
					if(get_CurDo() != NULL && DO_GetIsInit(get_CurDo()))
					{
//						if(!rs485_GetCircularSentStatus())//确保没有别的东西下发或者在查询发送查询温度和溶解氧的值
//						{
//							DO_rs485_GetTempTwoDO(get_CurDo());
//						}
						
						if(rs485_GetIsChangeSenesor())//第一次是溶解氧的话添加一下三个参数标签
						{
							rs485_ClearIsChangeSenesor();
							
							list_label label_head = NULL; //溶解氧数据label列表
							LabelList_Add( 16,  36, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,   LABEL_LARGE,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //do %
							LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //do mg/L
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							LabelList_Add( 120, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//%
							LabelList_Add( 120, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L
							LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃
							
							
							cur_interfacial.label_head = label_head;
							
						}
						
						if(setting_GetIsAlarm_DO())//如果有设置报警的话
						{
							if(DO_GetIsGetedValue(get_CurDo()))//如果有获取到数值的话
							{
								float high,low;
								high = float_format(setting_GetHighThreshold_DO());
								low = float_format(setting_GetLowThreshold_DO());
								
								temp_warning = flag_NeedWarning;
								
								
								flag_NeedWarning = 0;//先初始化为0
								if(high!=0 && DO_GetDOmgL(get_CurDo())>=high)
								{
									flag_NeedWarning = 1;//比高阈值高的话
								}
								if(low!=0 && DO_GetDOmgL(get_CurDo())<=low)
								{
									flag_NeedWarning = 1;//比低阈值低的话
								}
								if(temp_warning == 0 && flag_NeedWarning)//第一次报警响蜂鸣器
								{
									set_BeeAlarm();
								}
								if(temp_warning == 1 && !flag_NeedWarning)//第一次没有报警清一下图标
								{
									gui_ClearWarining();
								}
							}
						}
						else
						{
							flag_NeedWarning = 0;
						}
						
						
						if(setting_GetAutoShut() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo()))
						{
							gui_DrawLock((uint8_t *)icon_lock);//画锁
						}
						else
						{
							gui_ClearLock();//删锁
						}
						
						
						
						
					}
					break;
					
					case PAGE_5_TWOFIRST:
					case PAGE_5_TWOSECOND:
					case PAGE_5_ONE:
						
						break;
					
					
					default:
						break;
				}
				break;
			
			default:
				break;
		}
	}
	
	
	if(interfacial_GetCurPage() == PAGE_3_GPS )//添加正在搜索
	{
		static uint8_t gps_searching = 0;
		if(get_LatitudeArr()[0] == 0)
		{
			gps_searching = 1;
			if(setting_GetIsChn())                                                                  //状态栏文字标签
			{
				GUI_PutChnStr(16, 84, (uint8_t *)zhengzaisousuo_cn, sizeof(zhengzaisousuo_cn), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
			}
			else
			{
				GUI_PutEngStr(16, 84, (uint8_t *)zhengzaisousuo_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
			}
		}
		else
		{
			if(gps_searching)
			{
				gps_searching=0;
				gui_ClearLines(84, 100, 0);
				LabelList_Add( 0,  96, (uint8_t *)jingdu_cn, sizeof(jingdu_cn), (uint8_t *)jingdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//经度
				LabelList_Add( 0,  48, (uint8_t *)weidu_cn, sizeof(weidu_cn), (uint8_t *)weidu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//纬度
			}
		}
	}
	
	
	LabelList_Print(cur_interfacial.label_head, setting_GetIsChn());                        //遍历label链表刷新界面
	OptionList_Print(cur_interfacial.option_head, setting_GetIsChn(), get_RowSpacing());    //遍历选项链表刷新界面

	if(interfacial_GetMessageBoxFlag())
	{
		show_MessageBox();
	}
	
	
	
}



