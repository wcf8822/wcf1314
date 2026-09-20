#include "interfacial.h"
#include <stdio.h>
#include <math.h>
#include "logic.h"
#include "lcd_drive.h"
#include "loadbit.h"
#include "rtc.h"

#include "setting.h"

#include "main.h"

#include "key.h"
#include "bee.h"

#include "generate_page.h"

#include "bmp280.h"

#include "heart.h"

#include "log.h"

#include "spi_flash.h"
#include "dissolved_oxygen.h"
#include "NH3N_DN02.h"
#include "COD_DC18.h"
#include "DO_shenghui.h"
#include "NH3N_shenghui.h"
#include "COD_shenghui.h"
#include "pH_shenghui.h"
#include "Tur_shenghui.h"
#include "EC_shenghui.h"
#include "EC_DE26.h"
#include "pH_Dph07.h"
#include "Chl_shenghui.h"
#include "ORP_DR31.h"
#include "BGA_shenghui.h"
#include "MLSS_Tianjian.h"
#include "OiW_Guohong.h"
#include "OiW_yushan.h"
#include "DO_HaiFa_DY12.h"
#include "DO_HaiFa_DY56.h"
#include "DX01.h"
#include "MLSS_lanchang.h"
#include "EC_DE40.h"
#include "DZ09.h"
#include "TDS_DT49.h"
#include "DL312.h"
#include "DY05.h"
#include "DO59.h"

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
STATIC char cal_arr[10]   = {0};  //校准完成后显示用的校准通用数组
STATIC char cal_BGA_arr[9]   = {0};  //校准完成后显示用的校准通用数组

STATIC uint8_t flag_show_MSG = 0;           //是否显示弹窗
STATIC uint8_t flag_is_successful = 1;      //是否操作成功标志
STATIC uint8_t flag_NeedDestroyMSG = 0;     //是否需要销毁msgbox
STATIC uint8_t flage_FirstOKLong = 0;       //是不是第一次长按ok键

STATIC uint8_t* option_sensor_name;         //传感器管理里面设备显示名称的指针
STATIC PAGE_NUM temp_FatherPage;            //临时的父界面
STATIC float STD_value = 0.0;               //校准时候要用到的标准值
STATIC int32_t ORP_CAL_value = 0;           //校准时候要用到的标准值

STATIC SENSOR_TYPE temp_SensorType;         //当前需要修改的设备类型



SENSOR_TYPE datashow_SensorType=TYPE_NONE;  //当前需要修改的设备类型
MESSAGE_TYPE cur_MsgType = MESSAGE_SETTING; //弹窗的类型

STATIC uint16_t cur_LogIndex = 0;           //当前选择的数据日志的下标
STATIC uint8_t cur_LogIndex_arr[5] = {0};   //数组 0-9999

STATIC uint8_t FatherPage_OptionIndex = 0;  //父界面所选的标签下标
STATIC uint8_t GrandpaPage_OptionIndex = 0; //爷爷界面所选的标签下标
STATIC uint8_t AncestorPage_OptionIndex = 0;//阿太界面所选的标签下标

STATIC uint8_t flag_NeedWarning = 0;

HARDWARE_VERSION hardware_version; //硬件版本
const uint8_t software_version[] = "V2.2.9";  //软件版本

//SETTING_FIRSTRUN_JUDGE 维护这个变量,清除历史记录和恢复初始化数值

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
	snprintf((char *)cur_LogIndex_arr, 5, "%4d", data_index);
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
		
		case MESSAGE_tixing:
			set_SuccessfulTimStartFlag();
			if(interfacial_GetCurPage() != PAGE_0_START)
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			}
			break;

		case MESSAGE_Close:
			msg_content.content_cn = (uint8_t *)dianliangdizidongguanji_cn;
			msg_content.content_en = (uint8_t *)dianliangdizidongguanji_en;
			msg_content.content_cn_len = sizeof(dianliangdizidongguanji_cn);

			set_SuccessfulTimStartFlag();
			if(interfacial_GetCurPage() != PAGE_0_START)
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			}
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

		case MESSAGE_tixing:
		case MESSAGE_Close:
			if(setting_GetIsChn())                                                                  //状态栏文字标签
			{
				GUI_PutChnStr(24, 80, msg_content.content_cn, msg_content.content_cn_len, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
			}
			else
			{
				GUI_PutEngStr(24, 80, msg_content.content_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
			}
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
	if(get_KeyOffFlag())//短按关机键
	{
		clear_KeyOffFlag();		
	}
	if(get_KeyOffLongFlag())//长按关机键
	{
		clear_KeyOffLongFlag();
    	HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_RESET);  //关闭背光
		GUI_ClearSCR(0x00);       // 初始化缓冲区为0x00，并输出屏幕(清屏)
		for(uint8_t i=0; i<200;i++)GUI_UpdateDisplay();//刷新下屏幕
		
		if(!HAL_GPIO_ReadPin(USB_JOIN_GPIO_Port, USB_JOIN_Pin)) //充电过程中 关机了 软件复位再次进入充电显示状态
    	{ 
			if(KEY_OFF_STATUS != KEY_BURSTMODE)
			{
			  HAL_NVIC_SystemReset();
			}
		}
		else
		{
			SHUTDOWN();		//关闭电源
			while(1)
			{
			} 		
		}		 
	}

	if(get_KeyOkFlag())//按下对号键,并且当前cod不是连续测量模式
	{
		if(interfacial_GetCurPage() == PAGE_0_START && get_CurDo()->DC17_Mes_Para.value_f == 2 && get_CurDo()->modbus_id == COD_DC17_ModbusID)
		{
			TimeValue = 0;
			MesRun = 1;
			COD_DC17_rs485_Set_Mes(get_CurDo(),0);
		}
	}

	if(get_KeyMenuFlag())//菜单键
	{
    	destory_MessageBox();
		clear_KeyMenuFlag();
		interfacial_SetPage(PAGE_1_MENU, PAGE_NOT_BACK);
	}
	if(get_KeyBluFlag())//背光键
	{
		clear_KeyBluFlag();
		HAL_GPIO_TogglePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin);
		
	}
	if(get_KeyCalFlag())//校准键
	{
		clear_KeyCalFlag();
		if(interfacial_GetCurPage() == PAGE_0_START && rs485_GetDeviceCount())//所有传感器
		{
			if(get_CurDo()->DC17_Mes_Para.value_f == 2 && get_CurDo()->modbus_id == COD_DC17_ModbusID)//如果是COD dc18 显示弹窗不进入校准
			{
				msg_content.content_cn = (uint8_t *)qingqiehuanzhilianxucel_cn;
				msg_content.content_en = (uint8_t *)qingqiehuanzhilianxucel_en;
				msg_content.content_cn_len = sizeof(qingqiehuanzhilianxucel_cn);
				generate_MessageBox(MESSAGE_tixing, 1);//显示弹窗
			}
			else
			{
				interfacial_SetPage(PAGE_4_CAL, PAGE_NOT_BACK);
			}
		}
		
	}
	if(get_KeyCalLongFlag())//探头kb初始化界面
	{

		if(interfacial_GetCurPage() == PAGE_0_START && rs485_GetDeviceCount() && get_CurDo()->modbus_id != Cl_DL312_ModbusID )
		{
			interfacial_SetPage(PAGE_1_RESETCAL, PAGE_NOT_BACK);
		}
		if(interfacial_GetCurPage() == PAGE_4_CAL)
		{
			if(get_CurDo()->modbus_id == COD_DC18_ModbusID || get_CurDo()->modbus_id == COD_DC17_ModbusID)
			{
				interfacial_SetPage(PAGE_4_COD_SET, PAGE_NOT_BACK);
			}
		}	
		
	}

	if(get_KeyCalLongLongFlag())//系统设置下长按6秒进入隐藏菜单
	{
		if(interfacial_GetCurPage() == PAGE_2_SYSTEM)
		{
			interfacial_SetPage(PAGE_4_FACTORY,PAGE_NOT_BACK);
		}
	}

	if(get_KeyOkLongFlag())//保存数据成功弹窗
	{
		if(interfacial_GetCurPage() == PAGE_0_START )//有设备的话 防止主界面是搜索设备
		{
			if(get_CurDo() !=NULL )//当前设备不为空的话
			{							
				if(!flage_FirstOKLong)
				{
					flage_FirstOKLong = 1;						
					generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
				}
			}else{
					generate_MessageBox(MESSAGE_SAVELOG, 0);						
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
	
	if(interfacial_GetCurPage() == PAGE_0_START){
	  if(get_KeyUpFlag()){
		  clear_KeyUpFlag();
			if(cur_DO.DO_list != comA_DO.DO_list){	 
				rs485_SetIsChangeSenesor();
				interfacial_ClearLabel();                                  //清所有的数据标签  不加的话当显示数据（PAGE_0_START）的两个界面切换 cur_interfacial.label_head会一直添加到链表中导致溢出，无法显示数据内容
				cur_DO.DO_list = comA_DO.DO_list; 
				cur_DO.current_sensor_type=comA_DO.current_sensor_type;
			}
		}
		
	  if(get_KeyDownFlag()){
		  clear_KeyDownFlag();	
			if(comB_DO.DO_list != NULL){			
			 if(cur_DO.DO_list != comB_DO.DO_list){	 
				 rs485_SetIsChangeSenesor();
				 interfacial_ClearLabel();                                  //清所有的数据标签
				 cur_DO.DO_list = comB_DO.DO_list; 
				 cur_DO.current_sensor_type=comB_DO.current_sensor_type;	
			 }
			}
		}			
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
		case 1:
			cur_option->IsSelected = SELECTED; //是被选中
			break;
		
		case 5:
		case 2:
			cur_option = cur_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
			break;
		
		case 10:
		case 3:
			cur_option = cur_option->next_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
			break;
		
		case 20:
			cur_option = cur_option->next_option->next_option->next_option;
			cur_option->IsSelected = SELECTED; //否被选中
			break;
	}
}

void SAL_UNIT_init(uint8_t selected)
{
	cur_option = cur_interfacial.option_head;
	switch (selected)
	{
		case 0:
			cur_option->IsSelected = SELECTED; //是被选中
			break;
		
		case 1:
			cur_option = cur_option->next_option;
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

		case TYPE_pH:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_pH]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_Tur:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_Tur]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);				
			break;
		case TYPE_FCL:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_FCL]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
		break;
		case TYPE_EC:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_EC]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);			
			break;
		case TYPE_ORP:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_ORP]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);			
			break;
		case TYPE_NH4:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_NH4]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_F:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_F]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_CL:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_CL]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_Chl:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_Chl]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_Bga:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_Bga]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_CODuv:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_CODuv]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_MLSS:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_MLSS]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_Oiw:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_Oiw]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_TSS:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_TSS]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_SAL:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_SAL]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
			break;
		case TYPE_TDS:
			GUI_PutEngStr(60, 0, (uint8_t *)&(sensor_type_str[TYPE_TDS]), MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_REVERSE);
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
					case TYPE_DO://如果当前查询的设备是do的话
								if(setting_GetAutoLock_DO() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
								  break;
								}							
								if(setting_GetAutoLock_DO() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
									setting_SetAutoLock_num(0);
								}
							break;
						
						case TYPE_pH:
								if(setting_GetAutoLock_pH() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_pH() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}							
							break;
								
						case TYPE_Tur:
								if(setting_GetAutoLock_Tur() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_Tur() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_FCL:
								if(setting_GetAutoLock_FCL() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_FCL() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_EC:
								if(setting_GetAutoLock_EC() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_EC() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_ORP:
								if(setting_GetAutoLock_ORP() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_ORP() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_NH4:
								if(setting_GetAutoLock_NH4() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_NH4() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_F:
								if(setting_GetAutoLock_F() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_F() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_CL:
								if(setting_GetAutoLock_CL() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_CL() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_Chl:
								if(setting_GetAutoLock_Chl() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_Chl() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_Bga:
								if(setting_GetAutoLock_Bga() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_Bga() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
								
						case TYPE_CODuv:
								if(setting_GetAutoLock_COD() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									if(get_CurDo()->DC17_Mes_Para.value_f == 2 && get_CurDo()->modbus_id == COD_DC17_ModbusID)
									{
										//不锁定
									}
									else
									{
										DO_SetValueLocked(get_CurDo());    //锁住
									}
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_COD() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}		
							break;
						case TYPE_MLSS:
							if(setting_GetAutoLock_MLSS() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
							{
								DO_SetValueLocked(get_CurDo());    //锁住
								if(setting_GetLockSave())
								{
										generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
								}
								break;
							}							
							if(setting_GetAutoLock_MLSS() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
							{
								clear_DOShakeCount();              //清除抖动计数
								DO_SetValueUnlocked(get_CurDo());  //解锁
							}	
							break;
						case TYPE_Oiw:
							if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
							{
								if(setting_GetAutoLock_OIW() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_OIW() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}	
							}
							else
							{
								if(setting_GetAutoLock_OIW_ppm() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
								{
									DO_SetValueLocked(get_CurDo());    //锁住
									if(setting_GetLockSave())
									{
											generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
									}
									break;
								}							
								if(setting_GetAutoLock_OIW_ppm() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
								{
									clear_DOShakeCount();              //清除抖动计数
									DO_SetValueUnlocked(get_CurDo());  //解锁
								}
							}
							
							break;

						case TYPE_TSS:
							if(setting_GetAutoLock_TSS() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
							{
								DO_SetValueLocked(get_CurDo());    //锁住
								if(setting_GetLockSave())
								{
										generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
								}
								break;
							}							
							if(setting_GetAutoLock_TSS() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
							{
								clear_DOShakeCount();              //清除抖动计数
								DO_SetValueUnlocked(get_CurDo());  //解锁
							}	
							break;
								
						case TYPE_SAL:
							if(setting_GetAutoLock_SAL() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
							{
								DO_SetValueLocked(get_CurDo());    //锁住
								if(setting_GetLockSave())
								{
										generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
								}
								break;
							}							
							if(setting_GetAutoLock_SAL() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
							{
								clear_DOShakeCount();              //清除抖动计数
								DO_SetValueUnlocked(get_CurDo());  //解锁
							}	
							break;

						case TYPE_TDS:
							if(setting_GetAutoLock_TDS() == AUTOLOCK_MANUAL && !DO_GetValueLocked(get_CurDo()))//如果是手动锁定模式的话 值没被锁的话
							{
								DO_SetValueLocked(get_CurDo());    //锁住
								if(setting_GetLockSave())
								{
										generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
								}
								break;
							}							
							if(setting_GetAutoLock_TDS() != AUTOLOCK_OFF && DO_GetValueLocked(get_CurDo())) //如果是有锁定功能无论自动还是手动就给它开锁
							{
								clear_DOShakeCount();              //清除抖动计数
								DO_SetValueUnlocked(get_CurDo());  //解锁
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
	if(interfacial_GetCurPage() != PAGE_0_START){
		if(get_KeyUpFlag())    clear_KeyUpFlag();
		if(get_KeyDownFlag())  clear_KeyDownFlag();
	}
}


void Search_History_Type_KeyUpFlag(void){
	uint8_t Get_Search_Flag=0;
	if(datashow_SensorType != TYPE_NONE){
		while(!Get_Search_Flag)
		{
			 if(datashow_SensorType == 0)
			 {
				 if(log_GetLogCount(17)	!= 0)
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=17;
           return;					 
				 }else{
					 datashow_SensorType=17;
				 }
			 }

			 
			 if(datashow_SensorType == 1 )
			 {
				 if(log_GetLogCount(0) != 0)
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=0;		
           return;					 
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }						 
			 }
			 
			 if(datashow_SensorType == 2 )
			 {
				 if(log_GetLogCount(1) != 0)	
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=1;		
           return;					 
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }							 
			 }
	
			 if(datashow_SensorType == 3)
			 {	
				 if(log_GetLogCount(2)	!= 0)	
				 {								 
					 Get_Search_Flag=1;
					 datashow_SensorType=2;
					 return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }

			 if(datashow_SensorType == 4)
			 {	
				 if(log_GetLogCount(3)	!= 0)				
				 {								 
					 Get_Search_Flag=1;
					 datashow_SensorType=3;
					 return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }

			 if(datashow_SensorType == 5)
			 {	
				 if(log_GetLogCount(4) != 0)
				 {								 
					 Get_Search_Flag=1;
					 datashow_SensorType=4;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
		
			 if(datashow_SensorType == 6)
			 {		
				 if(log_GetLogCount(5)	!= 0)							 
				 {	 
					 Get_Search_Flag=1;
					 datashow_SensorType=5;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
		
			 if(datashow_SensorType == 7)
			 {	
				 if(log_GetLogCount(6)	!= 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=6;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 } 						 

			 if(datashow_SensorType == 8)
			 {	
				 if(log_GetLogCount(7)	!= 0)
				 {							 
					 Get_Search_Flag=1;
					 datashow_SensorType=7;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }	
			 }
			 
			 if(datashow_SensorType == 9)
			 {
				 if(log_GetLogCount(8) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=8;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }						 
					
			 if(datashow_SensorType == 10)
			 {
				 if(log_GetLogCount(9) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=9;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
			 
			 if(datashow_SensorType == 11)
			 {	
				 if(log_GetLogCount(10) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=10;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }				 
			 if(datashow_SensorType == 12)
			 {	
				 if(log_GetLogCount(11) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=11;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
			 if(datashow_SensorType == 13)
			 {	
				 if(log_GetLogCount(12) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=12;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }

			 if(datashow_SensorType == 14)
			 {	
				 if(log_GetLogCount(13) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=13;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
			 if(datashow_SensorType == 15)
			 {	
				 if(log_GetLogCount(14) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=14;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
			 if(datashow_SensorType == 16)
			 {	
				 if(log_GetLogCount(15) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=15;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
			 if(datashow_SensorType == 17)
			 {	
				 if(log_GetLogCount(16) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=16;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType-1);	
				 }
			 }
		}
					
	}
}

void Search_History_Type_KeyDownFlag(void){
	uint8_t Get_Search_Flag=0;
	if(datashow_SensorType != TYPE_NONE){
		while(!Get_Search_Flag)
		{
			 if(datashow_SensorType == 0)
			 {
				 if(log_GetLogCount(1) != 0)
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=1;	
           return;					 
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);								 
				 }	 					 
			 }
			 
			 if(datashow_SensorType == 1 )
			 {
				 if(log_GetLogCount(2) != 0)
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=2;		
           return;					 
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }						 
			 }
			 
			 if(datashow_SensorType == 2 )
			 {
				 if(log_GetLogCount(3) != 0)	
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=3;		
           return;					 
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }							 
			 }
	
			 if(datashow_SensorType == 3)
			 {	
				 if(log_GetLogCount(4)	!= 0)	
				 {								 
					 Get_Search_Flag=1;
					 datashow_SensorType=4;
					 return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }

			 if(datashow_SensorType == 4)
			 {	
				 if(log_GetLogCount(5)	!= 0)				
				 {								 
					 Get_Search_Flag=1;
					 datashow_SensorType=5;
					 return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }

			 if(datashow_SensorType == 5)
			 {	
				 if(log_GetLogCount(6) != 0)
				 {								 
					 Get_Search_Flag=1;
					 datashow_SensorType=6;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }
		
			 if(datashow_SensorType == 6)
			 {		
				 if(log_GetLogCount(7)	!= 0)							 
				 {	 
					 Get_Search_Flag=1;
					 datashow_SensorType=7;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }
		
			 if(datashow_SensorType == 7)
			 {	
				 if(log_GetLogCount(8)	!= 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=8;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 } 						 

			 if(datashow_SensorType == 8)
			 {	
				 if(log_GetLogCount(9)	!= 0)
				 {							 
					 Get_Search_Flag=1;
					 datashow_SensorType=9;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }	
			 }
			 
			 if(datashow_SensorType == 9)
			 {
				 if(log_GetLogCount(10) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=10;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }						 
					
			 if(datashow_SensorType == 10)
			 {
				 if(log_GetLogCount(11) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=11;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }
			 
			 if(datashow_SensorType == 11)
			 {
				 if(log_GetLogCount(12) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=12;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }

			 if(datashow_SensorType == 12)
			 {
				 if(log_GetLogCount(13) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=13;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);						
				 }
			 }
			 
			 if(datashow_SensorType == 13)
			 {	
				 if(log_GetLogCount(14) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=14;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);
				 }
			 }	
			 if(datashow_SensorType == 14)
			 {	
				 if(log_GetLogCount(15) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=15;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);
				 }
			 }
			 if(datashow_SensorType == 15)
			 {	
				 if(log_GetLogCount(16) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=16;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);
				 }
			 }
			 if(datashow_SensorType == 16)
			 {	
				 if(log_GetLogCount(17) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=17;
           return;
				 }else{
					 datashow_SensorType=(SENSOR_TYPE)(datashow_SensorType+1);
				 }
			 }
			 if(datashow_SensorType == 17)
			 {	
				 if(log_GetLogCount(0) != 0)							 
				 {
					 Get_Search_Flag=1;
					 datashow_SensorType=0;
           return;
				 }else{
					 datashow_SensorType=0;	
				 }
			 }
		 
		}
					
	}
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
		if(interfacial_GetCurPage() == PAGE_2_HISTORY_ShowOPTION || interfacial_GetCurPage() == PAGE_2_HISTORY_DeleteOPTION)
		{
        	Search_History_Type_KeyUpFlag();				
		}		
		
		if(interfacial_GetCurPage() == PAGE_0_START)
		{
				if(cur_DO.DO_list == NULL){
				 cur_DO.DO_list = comA_DO.DO_list; 
				 cur_DO.current_sensor_type=comA_DO.current_sensor_type;		
				}
		}		
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
	
		if(interfacial_GetCurPage() == PAGE_2_HISTORY_ShowOPTION || interfacial_GetCurPage() == PAGE_2_HISTORY_DeleteOPTION)
		{
      		Search_History_Type_KeyDownFlag();
		}			
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

SENSOR_TYPE get_OptionSelectedSensorType(void)
{
	SENSOR_TYPE temp_type = TYPE_NONE;
//	switch(cur_option->content_eng[0])
//	{
//		case 'B':
//			temp_type = TYPE_Bga;
//			break;
//		
//		case 'C':
//			switch(cur_option->content_eng[1])
//			{
//				case 'L':
//					temp_type = TYPE_CL;
//					break;
//				
//				case 'h':
//					temp_type = TYPE_Chl;
//					break;
//				
//				case 'O':
//					temp_type = TYPE_CODuv;
//					break;
//			}
//			
//			break;
//		
//		case 'D':
//			temp_type = TYPE_DO;
//			break;
//		
//		case 'E':
//			temp_type = TYPE_EC;
//			break;
//		
//		case 'F':
//			if(cur_option->content_eng[1] == 'C')
//			{
//				temp_type = TYPE_FCL;
//			}
//			else
//			{
//				temp_type = TYPE_F;
//			}
//			break;
//		
//		case 'N':
//			temp_type = TYPE_NH4;
//			break;
//		
//		case 'O':
//			temp_type = TYPE_ORP;
//			break;
//		
//		case 'p':
//			temp_type = TYPE_pH;
//			break;
//		
//		case 'T':
//			temp_type = TYPE_Tur;
//			break;
//	}
  temp_type=cur_DO.current_sensor_type;
	return temp_type;
}


//ok esc切换界面的逻辑
void btn_OkEscMode_ChangePage(void)
{
	uint32_t STD_temp = 0;
	uint32_t STD_TUR = 0;
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
									
			/*保存标签下标方便返回界面时候能够跳到哪个选项*/
			if(interfacial_GetCurPage() == PAGE_1_MENU||interfacial_GetCurPage() == PAGE_2_HISTORY)
			{
				AncestorPage_OptionIndex = interfacial_GetCurrentOption()->option_index;//爷爷界面的
			}

			if(interfacial_GetCurPage() >= PAGE_2_SENSORMANAGE && interfacial_GetCurPage() <= PAGE_2_HISTORY_DeleteOPTION)
			{
				GrandpaPage_OptionIndex = interfacial_GetCurrentOption()->option_index;
				if(interfacial_GetCurPage() == PAGE_2_SENSORMANAGE)
				{				
					if(cur_option == cur_interfacial.option_head)
					{
						cur_DO.DO_list = comA_DO.DO_list; 
				    cur_DO.current_sensor_type=comA_DO.current_sensor_type;
					}
					else if(cur_option == cur_interfacial.option_head->next_option)
					{
						cur_DO.DO_list = comB_DO.DO_list; 
				    cur_DO.current_sensor_type=comB_DO.current_sensor_type;
					}
				}				
			}
			if(interfacial_GetCurPage() == PAGE_3_SENSORS || interfacial_GetCurPage() == PAGE_4_CAL || interfacial_GetCurPage() == PAGE_4_PARASET)
			{
				FatherPage_OptionIndex = interfacial_GetCurrentOption()->option_index;		
			}
		
			/*切换界面*/
			interfacial_SetPage(cur_option->next_page, PAGE_NOT_BACK);
		}
		else//如果没有需要转跳的界面的话
		{
	
			if(cur_option->son_option != NULL)
			{	 			
				switch(interfacial_GetCurPage()){
					case PAGE_5_DO_ONE_First:
					case PAGE_5_DO_TWO_FIRST:
					case PAGE_5_DO_TWO_SECOND:
					case PAGE_5_MLSS_zero_signal:	

							STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);
						
							OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表

							gui_ClearLines(75, 93, 0);//清开始校准的选项
							if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
							{
								if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
								{
									STD_value =STD_temp;
									snprintf(cal_arr, 8, "%5d", STD_temp);
									LabelList_Add(60, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示

								}
								else if(setting_GetMesUnit() == 5)
								{
									STD_value =STD_temp;
									snprintf(cal_arr, 8, "%6.3f", STD_value / 1000);
									LabelList_Add(60, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示

								}
								else
								{
									STD_value =STD_temp;
									snprintf(cal_arr, 8, "%6.4f", STD_value / 10000);
									if(STD_value >= 100000)
									{
										LabelList_Add(60, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示

									}
									else
									{
										LabelList_Add(68, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示


									}
								}
							}
							else if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
							{
								STD_value =STD_temp;
								snprintf(cal_arr, 8, "%5.3f", STD_value / 1000);
								LabelList_Add(52, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							}
							else if(get_CurDo()->modbus_id == LH_DX01_ModbusID)
							{
								STD_value =STD_temp;
								snprintf(cal_arr, 8, "%6.2f", STD_value / 100);
								LabelList_Add(52, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							}
							else if(get_CurDo()->modbus_id == Cl_DL312_ModbusID)
							{
								STD_value =STD_temp;
								snprintf(cal_arr, 8, "%7.1f", STD_value / 10.0);
								LabelList_Add(60, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							}
							else
							{
								STD_value = (STD_temp == 0 ? CAL_ZERO_VALUE : STD_temp / 10.0);//计算校准的值
								snprintf(cal_arr, 8, "%05.01f %%", STD_temp / 10.0);//将校准值写入校准文字buff
								LabelList_Add(52, 56,  
														NULL, 0, (uint8_t *)cal_arr, 
														LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							}
						
							LabelList_Add(0, 76,
														(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
														LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
														
							LabelList_Add(0, 96,
														(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
														LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
														&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。
														
							if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
							{
								LabelList_Add(56, 136,
														NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,
														LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
														&(interfacial_GetCurrentInterfacial()->label_head)); 
							}
							else if(get_CurDo()->modbus_id == OiW_guohong_ModbusID
							|| get_CurDo()->modbus_id == LH_DX01_ModbusID || get_CurDo()->modbus_id == Cl_DL312_ModbusID )
							{
								LabelList_Add(56, 136,
														NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
														LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
														&(interfacial_GetCurrentInterfacial()->label_head));  //do %
							}
							else
							{
								LabelList_Add(56, 136,
														NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,
														LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
														&(interfacial_GetCurrentInterfacial()->label_head));  //do %
							}
							
							set_RowSpacing(84);
							OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
							CurOption_init();											
						break ;
														
					case PAGE_5_NH3N_ONE:
					case PAGE_5_NH3N_TWO:
								STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);													
							  STD_value = STD_temp / 10.0;//计算校准的值
								snprintf(cal_arr, 8, "%5.1f", STD_temp / 10.0);//将校准值写入校准文字buff
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
								gui_ClearLines(75, 93, 0);//清开始校准的选项
					
								LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							
								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。					

									LabelList_Add(0, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  // NH3N  mg/L
									
									LabelList_Add(70, 136,
																NULL, 0, (uint8_t *)mV_en,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //  NH3N 的 电压值单位 mV
									
									LabelList_Add(96, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_Vol_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //		NH3N 的 电压值			
								set_RowSpacing(84);
								OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
								CurOption_init();															

            break;					
					
					case PAGE_5_NH3N_pH_ONE:
					case PAGE_5_NH3N_pH_TWO:
          case PAGE_5_NH3N_pH_THREE:
								STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);													
							  STD_value = (STD_temp == 0 ? CAL_ZERO_VALUE : STD_temp / 100.0);//计算校准的值
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
								snprintf(cal_arr, 8, "%5.2f", STD_temp / 100.0);//将校准值写入校准文字buff
								gui_ClearLines(75, 93, 0);//清开始校准的选项
							
								LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							
								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。								
                  if(get_CurDo()->modbus_id == pH_DpH07_ModbusID  || get_CurDo()->modbus_id == pH_shenghui_ModbusID)	
                  {
										LabelList_Add(0, 136,
																	NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																	LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																	&(interfacial_GetCurrentInterfacial()->label_head));  //pH									
									}else{
										LabelList_Add(0, 136,
																	NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,
																	LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																	&(interfacial_GetCurrentInterfacial()->label_head));  //pH
									}
									LabelList_Add(70, 136,
																NULL, 0, (uint8_t *)mV_en,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //pH 电压单位mV
									
									LabelList_Add(96, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //pH 的 电压值																	
										
	
									set_RowSpacing(84);
									OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
									CurOption_init();												

            break;			
						
					case PAGE_5_COD_shenghui_ONE:
					case PAGE_5_COD_shenghui_TWO:
					case PAGE_5_COD_shenghui_THREE:
          case PAGE_5_COD_DC18_Zero:
		  case PAGE_5_COD_DC17_IN_Zero:
					case PAGE_5_COD_DC18_Slope:					
								STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);													
							  STD_value = STD_temp / 10.0;//计算校准的值
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
								snprintf(cal_arr, 8, "%5.1f", STD_temp / 10.0);//将校准值写入校准文字buff
								gui_ClearLines(75, 93, 0);//清开始校准的选项
							
								LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							
								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。					

									LabelList_Add(56, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //COD值
									
								set_RowSpacing(84);
								OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
								CurOption_init();						
            break;							
													
					
					case PAGE_5_COD_shenghui_Tur_ONE:
					case PAGE_5_COD_shenghui_Tur_TWO:
          case PAGE_5_COD_DC18_Tur_Zero:
		  case PAGE_5_COD_DC17_IN_Tur_Zero:
					case PAGE_5_COD_DC18_Tur_Slope:					
								STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);													
							  STD_value = STD_temp / 10.0;//计算校准的值
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
								snprintf(cal_arr, 8, "%5.1f", STD_temp / 10.0);//将校准值写入校准文字buff
								gui_ClearLines(75, 93, 0);//清开始校准的选项
							
								LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
							
								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。							
							LabelList_Add(56, 136,
														NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,
														LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
														&(interfacial_GetCurrentInterfacial()->label_head));  //浊度值

						set_RowSpacing(84);
						OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
						CurOption_init();					
            break;			


					case PAGE_5_shenghui_Tur_ONE:
					case PAGE_5_shenghui_Tur_TWO:
					case PAGE_5_shenghui_Tur_THREE:		
								STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);													
								STD_TUR = NanoOptionList_Get_np_Value(cur_interfacial.option_head->son_option, 10);	
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
								
								if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
								{
									STD_value =STD_temp;
									snprintf(cal_arr, 8, "%7.3f", STD_value / 1000);
								}
								else if(get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
								{
									STD_value =STD_TUR;
									snprintf(cal_arr, 8, "%6.1f", STD_value / 10);
								}
								else
								{
									STD_value = STD_temp / 10.0;//计算校准的值
									snprintf(cal_arr, 8, "%5.1f", STD_temp / 10.0);//将校准值写入校准文字buff
								}
								gui_ClearLines(75, 93, 0);//清开始校准的选项
								if(interfacial_GetCurPage() == PAGE_5_shenghui_Tur_ONE)
								{
									STD_value =STD_TUR;
									snprintf(cal_arr, 8, "%5.1f", STD_value / 10);
									if(get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
									{
										if(Get_uint_flag() == 68)
										{
											STD_value = STD_value;
											LabelList_Add(52, 56,   
															NULL, 0, (uint8_t *)jiahao_en, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(cur_interfacial.label_head));
															
											LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
										}
										else if(Get_uint_flag() == 67)
										{
											STD_value = -STD_value;
											LabelList_Add(52, 56,   
															NULL, 0, (uint8_t *)jianhao_en, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(cur_interfacial.label_head));
										}
											LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
									}
									else
									{
										LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
									}
								}
								else
								{
								LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
								}
								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。		
								if(get_CurDo()->modbus_id == OiW_guohong_ModbusID
								|| get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
								{
									LabelList_Add(56, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //do %
								}
								else
								{
									LabelList_Add(0, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //浊度
									LabelList_Add(70, 136,
																NULL, 0, (uint8_t *)mV_en,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //浊度 电压单位mV
									
									LabelList_Add(96, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //浊度 的 电压值		
								}
						set_RowSpacing(84);
						OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
						CurOption_init();	
            break;			

							case PAGE_5_DR31_ORP_ONE:
								ORP_CAL_value = NanoOptionList_Get_np_Value(cur_interfacial.option_head->son_option, 10);				
								STD_value = ORP_CAL_value;								
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表								
								gui_ClearLines(75, 93, 0);//清开始校准的选项
								if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
								{
									snprintf(cal_arr, 8, "%4.2f",STD_value/100.0f);//将校准值写入校准文字buff
								}
								else if(get_CurDo()->modbus_id == OiW_yushan_ModbusID
								|| get_CurDo()->modbus_id ==OiW_yushan_DA511_ModbusID)
								{
									STD_value = ORP_CAL_value;
									snprintf(cal_arr, 10, "%7.2f",fabsf((float)ORP_CAL_value/100.0f));//将校准值写入校准文字buff
								}
								if (orp_np == 68)
								{	
									ORP_CAL_value = ORP_CAL_value;
									if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
									{
										STD_value = ORP_CAL_value;
										snprintf(cal_arr, 10, "%8.3f",fabsf((float)ORP_CAL_value/1000.0f));//将校准值写入校准文字buff
									}
									else
									{
										snprintf(cal_arr, 8, "%5d",ORP_CAL_value);//将校准值写入校准文字buff
									}
									if(get_CurDo()->modbus_id != MLSS_Tianjian_ModbusID)
									{
										LabelList_Add(52, 56,   
															NULL, 0, (uint8_t *)jiahao_en, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(cur_interfacial.label_head));
									}
								}
								else if (orp_np == 67)
								{
									ORP_CAL_value = - ORP_CAL_value;
									if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
									{
										STD_value = ORP_CAL_value;
										snprintf(cal_arr, 10, "%8.3f",fabsf((float)ORP_CAL_value/1000.0f));//将校准值写入校准文字buff
									}
									else
									{
										snprintf(cal_arr, 8, "%5d",ORP_CAL_value);//将校准值写入校准文字buff
									}
									if(get_CurDo()->modbus_id != MLSS_Tianjian_ModbusID)
									{
									LabelList_Add(52, 56,   
															NULL, 0, (uint8_t *)jianhao_en, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(cur_interfacial.label_head));
									}
								}
								if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
								{
									LabelList_Add(68, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
								}
								else
								{
								LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
								}
							
								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。
								if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
								{
									LabelList_Add(56, 136,
															NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,
															LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
															&(interfacial_GetCurrentInterfacial()->label_head));  //ORP测量数值	
								}
								else
								{
										LabelList_Add(56, 136,
															NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
															LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
															&(interfacial_GetCurrentInterfacial()->label_head));  //ORP测量数值		
								}							 
								set_RowSpacing(84);
								OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
								CurOption_init();	
							break;
															
			    case PAGE_5_shenghui_EC_ONE:		
				case PAGE_5_DE26_EC_Zero:		
				case PAGE_5_shenghui_BGA_ONE:
				case PAGE_5_shenghui_BGA_TWO:	
				case PAGE_5_OIW_K_CAL:				
								if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
								{
									STD_value = 0;
									STD_value = NanoOptionList_Get_np_Value(cur_interfacial.option_head->son_option, 10);	
									if(orp_np == 67)
									{
										STD_value = -STD_value;
									}
									else
									{
										STD_value = STD_value;
									}

								}
								else
								{
									STD_temp = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);													
							 		STD_value = STD_temp;//计算校准的值
								}	
								OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
								if(get_CurDo()->modbus_id == Chl_shenghui_ModbusID)
								{
									STD_value = STD_temp / 10.0;//计算校准的值
									snprintf(cal_arr, 8, "%5.1f", STD_value);//将校准值写入校准文字buff
								}
								else if(get_CurDo()->modbus_id == Bga_shenghui_ModbusID)
								{
									STD_value = STD_temp / 10.0 *1000;
							   	  	snprintf(cal_BGA_arr, 6, "%5.1f", STD_temp/10.0f);//将校准值写入校准文字buff								
									
								}
								else if(get_CurDo()->modbus_id == OiW_guohong_ModbusID ||
								get_CurDo()->modbus_id == OiW_yushan_ModbusID ||
								get_CurDo()->modbus_id == OiW_yushan_DA511_ModbusID||
								get_CurDo()->modbus_id == EC_DE40_ModbusID ||
								get_CurDo()->modbus_id == EC_DS46_ModbusID ||
								get_CurDo()->modbus_id == TDS_DT49_Modbus ||
								get_CurDo()->modbus_id == LH_DX01_ModbusID)
								{
									STD_value = STD_temp / 100.0;//计算校准的值
							   	  	snprintf(cal_arr, 8, "%6.2f", STD_value);//将校准值写入校准文字buff			
								}
								else if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
								{
									if((setting_GetMesUnit() == 0) || (setting_GetMesUnit() == 10))
									{
										snprintf(cal_arr, 8, "%4d", (int)STD_value);												
									}
									else if((setting_GetMesUnit() == 5))
									{
										snprintf(cal_arr, 8, "%6.3f", STD_value/1000);
									}
									else
									{
										snprintf(cal_arr, 8, "%6.4f", STD_value/10000);
									}
								}
								else
								{
							   	  	snprintf(cal_arr, 8, "%d", (unsigned int)STD_value);//将校准值写入校准文字buff								
								}
								gui_ClearLines(75, 93, 0);//清开始校准的选项
								if(get_CurDo()->modbus_id == Bga_shenghui_ModbusID)
								{
									LabelList_Add(60, 56,  
															NULL, 0, (uint8_t *)cal_BGA_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
								}
								else if(get_CurDo()->modbus_id == EC_DE40_ModbusID 
								|| get_CurDo()->modbus_id == EC_DS46_ModbusID)
								{
									LabelList_Add(60, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示

								}
								else if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
								{
									LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
								}
								else
								{
									LabelList_Add(52, 56,  
															NULL, 0, (uint8_t *)cal_arr, 
															LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//将校准值变成label显示
								}

								LabelList_Add(0, 76,
															(uint8_t *)charubiaozhunrongye_cn, sizeof(charubiaozhunrongye_cn), (uint8_t *)charubiaozhunrongye_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//插入标准溶液
															
								LabelList_Add(0, 96,
															(uint8_t *)dengdaizhong_cn, sizeof(dengdaizhong_cn), (uint8_t *)dengdaizhong_en,
															LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, 
															&(interfacial_GetCurrentInterfacial()->label_head));//等待中。。。		
															
														
								if(get_CurDo()->modbus_id == Chl_shenghui_ModbusID)
								{
										LabelList_Add(0, 136,
																	NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																	LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																	&(interfacial_GetCurrentInterfacial()->label_head));  //叶绿素
									LabelList_Add(70, 136,
																NULL, 0, (uint8_t *)mV_en,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //电压单位mV
									
									LabelList_Add(96, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_Vol_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //叶绿素 电压									
								}
								else if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID || get_CurDo()->modbus_id == EC_DS46_ModbusID)
               					{
									LabelList_Add(56, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));							 
							 	}
							 	else
               					{
									LabelList_Add(56, 136,
																NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
																LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
																&(interfacial_GetCurrentInterfacial()->label_head));  //电导率							 
							 	}

						set_RowSpacing(84);
						OptionList_Add(0, (uint8_t *)querenxiaozhun_cn, sizeof(querenxiaozhun_cn), (uint8_t *)querenxiaozhun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(interfacial_GetCurrentInterfacial()->option_head));
						CurOption_init();	
            break;		
															
					default:
						cur_option->IsSelected = DESELECTED;//大标签选中取消				
						cur_NanoOption = cur_option->son_option; //当前小标签指针指向第一个小标签
						cur_NanoOption->IsSelected = SELECTED;					
						btnUpDownFunc_register(btn_UpDownMode_ChangeValue);//注册新的按键逻辑
						btnOkEscFunc_register(btn_OkEscMode_ChangeOption);//注册新的按键逻辑
						break;
					
				
				}			
			
			}
			else//就是save的情况得弹窗
			{
				switch (interfacial_GetCurPage())
				{
					case PAGE_3_DATASAVE:
						if(get_CurDo() !=NULL )//当前设备不为空的话
						{
							generate_MessageBox(MESSAGE_SAVELOG, log_SaveData(rs485_GetSensorType()));
						}else{
							generate_MessageBox(MESSAGE_SAVELOG, 0);		
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
					
	      	case PAGE_5_COD_Cleanse:					
						msg_content.content_cn = (uint8_t *)msg_Cleanse_cn;
						msg_content.content_en = (uint8_t *)msg_Cleanse_en;
						msg_content.content_cn_len = sizeof(msg_Cleanse_cn);
						generate_MessageBox(MESSAGE_SETTING, 1);//显示弹窗
						break;
					case PAGE_3_SENSORS:
					case PAGE_4_FACTORY:
						msg_content.content_cn = (uint8_t *)qingqiehuanzhilianxucel_cn;
						msg_content.content_en = (uint8_t *)qingqiehuanzhilianxucel_en;
						msg_content.content_cn_len = sizeof(qingqiehuanzhilianxucel_cn);
						generate_MessageBox(MESSAGE_tixing, 1);//显示弹窗	
					break;

					case PAGE_5_MLSS_zero_signal:
					case PAGE_5_DO_ONE_First:
					case PAGE_5_DO_TWO_FIRST:
					case PAGE_5_DO_TWO_SECOND:
					case PAGE_5_TEMP:
					case PAGE_5_NH3N_ONE:
					case PAGE_5_NH3N_TWO:
					case PAGE_5_NH3N_pH_ONE:
					case PAGE_5_NH3N_pH_TWO:
					case PAGE_5_NH3N_pH_THREE:
						
					case PAGE_5_COD_shenghui_Tur_ONE:
					case PAGE_5_COD_shenghui_Tur_TWO:
					case PAGE_5_COD_shenghui_ONE:
					case PAGE_5_COD_shenghui_TWO:
					case PAGE_5_COD_shenghui_THREE:
          			case PAGE_5_COD_shenghui_Zero:	 
						
					case PAGE_5_COD_DC18_Tur_Zero:
					case PAGE_5_COD_DC17_IN_Tur_Zero:
					case PAGE_5_COD_DC18_Tur_Slope:
					case PAGE_5_COD_DC18_Zero:
					case PAGE_5_COD_DC17_IN_Zero:
					case PAGE_5_COD_DC18_Slope:
					case PAGE_5_COD_DC18_Coefficient:
						
					case PAGE_5_shenghui_Tur_ONE:
					case PAGE_5_shenghui_Tur_TWO:
					case PAGE_5_shenghui_Tur_THREE:		

					case PAGE_5_shenghui_EC_ONE:
					case PAGE_5_shenghui_BGA_ONE:
					case PAGE_5_shenghui_BGA_TWO:			
					case PAGE_5_DR31_ORP_ONE:
					case PAGE_5_OIW_K_CAL:
					case PAGE_5_DE26_EC_Zero:		
					
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
	log_ReadData(generate_GetLogUnion(), cur_LogIndex - 1,datashow_SensorType);
	
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

/*滑动平均*/
void update_SlideValue(void)
{
	if(cur_NanoOption == cur_interfacial.option_head->son_option)//
	{
		switch(cur_NanoOption->value)
		{
			case 0:
				cur_interfacial.option_head->son_option->next_option->min_value = 2;
				cur_interfacial.option_head->son_option->next_option->max_value = 9;
				if(cur_NanoOption->next_option->value < 2)
				{
					cur_NanoOption->next_option->value = 2;
				}
				break;
			case 4:
				cur_interfacial.option_head->son_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->max_value = 0;
				if(cur_NanoOption->next_option->value != 0)
				{
					cur_NanoOption->next_option->value = 0;
				}
				break;
			
			default:
				cur_interfacial.option_head->son_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->max_value = 9;
				break;
		}
	}
}

void update_SlideValue_Sensor(void)
{
	if(cur_NanoOption == cur_interfacial.option_head->son_option)//
	{
		switch(cur_NanoOption->value)
		{
			case 0:
				cur_interfacial.option_head->son_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->max_value = 9;

				cur_interfacial.option_head->son_option->next_option->next_option->min_value = 1;
				cur_interfacial.option_head->son_option->next_option->next_option->max_value = 9;
				if(cur_NanoOption->next_option->next_option->value < 1)
				{
					cur_NanoOption->next_option->next_option->value = 1;
				}

				break;
			case 2:
				cur_interfacial.option_head->son_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->max_value = 0;
				if(cur_NanoOption->next_option->value != 0)
				{
					cur_NanoOption->next_option->value = 0;
				}

				cur_interfacial.option_head->son_option->next_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->next_option->max_value = 0;
				if(cur_NanoOption->next_option->next_option->value != 0)
				{
					cur_NanoOption->next_option->next_option->value = 0;
				}

				break;
			
			default:
				cur_interfacial.option_head->son_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->max_value = 9;
				cur_interfacial.option_head->son_option->next_option->next_option->min_value = 0;
				cur_interfacial.option_head->son_option->next_option->next_option->max_value = 9;
				break;
		}
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
			case PAGE_3_DATASHOW://显示记录界面
				value_PlusPlus_u16(&cur_LogIndex, 1, log_GetLogCount(datashow_SensorType));
				update_LogData();
				break;
			case PAGE_3_TIME://时间界面
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
				value_ReduceReduce_u16(&cur_LogIndex, 1, log_GetLogCount(datashow_SensorType));
				update_LogData();
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
		
		switch(interfacial_GetCurPage())
		{
			case PAGE_3_TIME:
				if(cur_NanoOption == cur_interfacial.option_head->son_option)
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
				break;
				
			case PAGE_5_SLIDEVALUE:
//				if(cur_NanoOption == cur_interfacial.option_head->son_option)//
//				{
//					switch(cur_NanoOption->value)
//					{
//						case 0:
//							cur_interfacial.option_head->son_option->next_option->min_value = 2;
//							cur_interfacial.option_head->son_option->next_option->max_value = 9;
//							if(cur_NanoOption->next_option->value < 2)
//							{
//								cur_NanoOption->next_option->value = 2;
//							}
//							break;
//						case 4:
//							cur_interfacial.option_head->son_option->next_option->min_value = 0;
//							cur_interfacial.option_head->son_option->next_option->max_value = 0;
//							if(cur_NanoOption->next_option->value != 0)
//							{
//								cur_NanoOption->next_option->value = 0;
//							}
//							break;
//						
//						default:
//							cur_interfacial.option_head->son_option->next_option->min_value = 0;
//							cur_interfacial.option_head->son_option->next_option->max_value = 9;
//							break;
//					}
//				}
				update_SlideValue();
				break;
			case PAGE_4_SENSOR_SLIDEAVG:
				update_SlideValue_Sensor();
				break;
			
			default:
				break;
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
		
		switch(interfacial_GetCurPage())
		{
			case PAGE_5_SLIDEVALUE:
//				if(cur_NanoOption == cur_interfacial.option_head->son_option)//
//				{
//					switch(cur_NanoOption->value)
//					{
//						case 0:
//							cur_interfacial.option_head->son_option->next_option->min_value = 2;
//							cur_interfacial.option_head->son_option->next_option->max_value = 9;
//							if(cur_NanoOption->next_option->value < 2)
//							{
//								cur_NanoOption->next_option->value = 2;
//							}
//							break;
//						case 4:
//							cur_interfacial.option_head->son_option->next_option->min_value = 0;
//							cur_interfacial.option_head->son_option->next_option->max_value = 0;
//							if(cur_NanoOption->next_option->value != 0)
//							{
//								cur_NanoOption->next_option->value = 0;
//							}
//							break;
//						
//						default:
//							cur_interfacial.option_head->son_option->next_option->min_value = 0;
//							cur_interfacial.option_head->son_option->next_option->max_value = 9;
//							break;
//					}
//				}
				update_SlideValue();
				break;
			case PAGE_4_SENSOR_SLIDEAVG:
				update_SlideValue_Sensor();
				break;
			
			default:
				break;
		}
		
		
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
	int32_t temp_high;
	int32_t temp_low;
	
	
	temp_low = NanoOptionList_Get_np_Value(cur_interfacial.option_head->son_option, 10);

	if(orp_np == 67)
	temp_low =  - temp_low;

	temp_high = NanoOptionList_Get_np_Value(cur_interfacial.option_head->next_option->son_option, 10);

	if(orp_np == 67)
	temp_high =  - temp_high;
	
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
				
				case PAGE_5_TEMP://这里先弹一下之后要通过串口来判断
				case PAGE_4_Temp_k:
				case PAGE_4_Clear_time:
				case PAGE_4_Mes_Mode:
				case PAGE_4_Set_Mode_Temp:
				case PAGE_4_Mes_Set:
				case PAGE_4_Mes_Time_Set:
				case PAGE_3_PRESSURE:
				case PAGE_3_SALT:				
				case PAGE_1_RESETCAL:
				case PAGE_4_SENSORINFO: //传感器信息用来修改传感器modbus id
				case PAGE_5_DO_ONE_First:        //先不用弹窗通过串口数据去弹窗
				case PAGE_5_MLSS_zero_signal:
				case PAGE_5_SENSORCAP:
				case PAGE_5_DO_TWO_FIRST:
				case PAGE_5_DO_TWO_SECOND:
				case PAGE_5_NH3N_ONE:
				case PAGE_5_NH3N_TWO:
				case PAGE_5_NH3N_pH_ONE:
				case PAGE_5_NH3N_pH_TWO:
				case PAGE_5_NH3N_pH_THREE:
					
				case PAGE_5_COD_shenghui_Tur_ONE:
				case PAGE_5_COD_shenghui_Tur_TWO:
				case PAGE_5_COD_shenghui_ONE:
				case PAGE_5_COD_shenghui_TWO:
				case PAGE_5_COD_shenghui_THREE:
        		case PAGE_5_COD_shenghui_Zero:	 					
				case PAGE_5_COD_DC17_IN_Tur_Zero:
				case PAGE_5_COD_DC18_Tur_Zero:
				case PAGE_5_COD_DC18_Tur_Slope:
				case PAGE_5_COD_DC18_Zero:
				case PAGE_5_COD_DC17_IN_Zero:
				case PAGE_5_COD_DC18_Slope:
				case PAGE_5_COD_DC18_Coefficient:				
	      		case PAGE_5_COD_Cleanse:			
					
				case PAGE_5_shenghui_Tur_ONE:
				case PAGE_5_shenghui_Tur_TWO:
				case PAGE_5_shenghui_Tur_THREE:		
					
			  	case PAGE_5_shenghui_EC_ONE:		
				case PAGE_5_shenghui_BGA_ONE:
				case PAGE_5_shenghui_BGA_TWO:
			  	case PAGE_5_DE26_EC_Zero:		
				case PAGE_4_Measure_Switch:
				case PAGE_5_DR31_ORP_ONE:
				case PAGE_5_OIW_K_CAL:
				case PAGE_3_Set_TDS_coefficient:
				case PAGE_3_Set_Temp_coefficient:
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
		if (interfacial_GetCurPage() == PAGE_5_shenghui_Tur_ONE || interfacial_GetCurPage() == PAGE_5_shenghui_Tur_TWO 
		|| interfacial_GetCurPage() == PAGE_5_shenghui_Tur_THREE || interfacial_GetCurPage() == PAGE_5_shenghui_BGA_ONE
		|| interfacial_GetCurPage() == PAGE_5_shenghui_BGA_TWO || interfacial_GetCurPage() == PAGE_5_DE26_EC_Zero
		|| interfacial_GetCurPage() == PAGE_5_shenghui_EC_ONE)
		{
			interfacial_SetPage(cur_interfacial.page_father, PAGE_IS_BACK);
		}
		else
		{
			cur_NanoOption->IsSelected = DESELECTED;//取消选中
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			destory_MessageBox();
		}
		// interfacial_SetPage(cur_interfacial.page_father, PAGE_IS_BACK);
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


double double_value;
/*点击保存后的操作*/
/*根据界面来进行相应操作*/
void save_setting(void)
{
	PtrToDOProbe p;//溶解氧指针
	uint32_t temp_value = 0;
	int16_t orp_alm_np_value = 0;
	int16_t temp_int_value=0;
  float  float_value=0.0;
	datetime_t temp_time;
	uint8_t data_702_580;

  	static	float compensate_k_temp = 1.0;
  	static	float compensate_b_temp = 0.0;
	
	static	float value0 = 0.0;//校准值
	static	float value1 = 0.0;
	
	static	float real0 = 0.0;//实际值
	static	float real1 = 0.0;
	static float MLSS_Cal_1 = 0.0;
	static float MLSS_Cal_2 = 0.0;
	
	static float MLSS_K = 0.0;
	static float MLSS_B = 0.0;
	
	
	// double double_value;
	
	switch(cur_PageNum)
	{
		case PAGE_1_RESETCAL:
			destory_MessageBox();
			switch(get_CurDo()->modbus_id)
			{
				
				case DO_HF1012_ModbusID:  
					DO_ClearCalPara(get_CurDo()); //清除溶解氧的校准参数					
					break;
				
				case NH3N_DN02_ModbusID:  
			    NH3N_DN02_rs485_ClearCal(get_CurDo());
					break;
				
				case COD_DC18_ModbusID:  
          		COD_DC18_rs485_ClearCal(get_CurDo(),COD_DC18_ModbusID);
					break;
				
				case COD_DC17_ModbusID:
          		COD_DC18_rs485_ClearCal(get_CurDo(),COD_DC17_ModbusID);
					break;

				case DO_shenghui_ModbusID:  
			    DO_shenghui_rs485_ClearCal(get_CurDo());
					break;
				
				case NH3N_shenghui_ModbusID:  
			    NH3N_shenghui_rs485_ClearCal(get_CurDo());
					break;
				
				case COD_shenghui_ModbusID:  
			    COD_shenghui_rs485_ClearCal(get_CurDo());
					break;	
				
				case pH_shenghui_ModbusID:  
			    pH_shenghui_rs485_ClearCal(get_CurDo());
					break;
				
				case Tur_shenghui_ModbusID:  
			    Tur_shenghui_rs485_ClearCal(get_CurDo());
					break;
				
				case EC_shenghui_ModbusID:  
			    EC_shenghui_rs485_ClearCal(get_CurDo());
					break;
				
				case EC_DE26_ModbusID:  
			    EC_DE26_rs485_SetMode(get_CurDo(),0x30);
					break;

				case EC_DE40_ModbusID:
				EC_DE40_rs485_SetMode(get_CurDo(),0x30,EC_DE40_ModbusID);
					break;

				case EC_DS46_ModbusID:
				EC_DE40_rs485_SetMode(get_CurDo(),0x30,EC_DS46_ModbusID);
					break;
				
				case TDS_DT49_Modbus:
				TDS_DT49_rs485_SetMode(get_CurDo(),0x30);
					break;

				case pH_DpH07_ModbusID:  
			    pH_DpH07_rs485_ClearCal(get_CurDo());
					break;

				case Chl_shenghui_ModbusID:  
			    Chl_shenghui_rs485_ClearCal(get_CurDo());
					break;
				
				case ORP_DR31_ModbusID:
				ORP_DR31_rs485_ClearCal(get_CurDo());
					break;
				
				case Bga_shenghui_ModbusID:
				BGA_shenghui_rs485_ClearCal(get_CurDo());
					break;
				case MLSS_Tianjian_ModbusID:
				// MLSS_Tianjian_rs485_Set_k_b(get_CurDo(),1.0f,0.0f);
				MLSS_Tianjian_Reset_Info(get_CurDo());
					break;

				case OiW_guohong_ModbusID:
				OiW_Guohong_rs485_Set_OIW_K_Reset(get_CurDo(),1);
					break;

				case OiW_yushan_ModbusID:
				OiW_yushan_rs485_Set_OIW_K_b_Reset(get_CurDo(),1,0,OiW_yushan_ModbusID);
					break;
					
				case OiW_yushan_DA511_ModbusID:
				OiW_yushan_rs485_Set_OIW_K_b_Reset(get_CurDo(),1,0,OiW_yushan_DA511_ModbusID);
					break;
					
				case DO_HF_DY12_ModbusID:
					DO_HaiFa_DY12_rs485_ClearCal(get_CurDo());
					break;

				case DO_HF_DY56_ModbusID:
					DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());
					Set_DY56_ClearCal_Flag(1);
					break;

				case LH_DX01_ModbusID:
					DX01_rs485_ClearCal(get_CurDo());
					break;
				case ZS_DZ09_ModbusID:
					DZ09_rs485_ClearCal(get_CurDo());
					break;
//				case Cl_DL312_ModbusID:
//					DL312_rs485_ClearCal(get_CurDo());
//					break;
				
				case DO_DY05_ModbusID:
					DY05_rs485_mode(get_CurDo());
					break;

				case DO_DO59_ModbusID:
					DO59_rs485_ClearCal(get_CurDo());
					break;
				
				default:
					break;

			}
			break;
			
		case PAGE_5_COD_shenghui_Zero:
			destory_MessageBox();
		  COD_shenghui_rs485_ZeroCal(get_CurDo()); 
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

		case PAGE_3_Set_Sal_uint://盐度单位设置
			uint8_t unit_value;
			if(cur_option == cur_interfacial.option_head)
			{
				// setting_SetSal_Unit(0);
				unit_value = 0;
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				unit_value = 1;
				// setting_SetSal_Unit(5);
			}
			destory_MessageBox();
			switch(get_CurDo()->modbus_id)
			{
				case EC_DS46_ModbusID:  
					EC_DE40_rs485_Set_sal_unit(get_CurDo(),unit_value,EC_DS46_ModbusID); //清除溶解氧的校准参数					
					break;
			}
			break;
	

		case PAGE_3_GPS:
			setting_SetKeyGPS(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
      if(setting_GetIsKeyGps())
			{
			    HAL_GPIO_WritePin(GPS_EN_PORT, GPS_EN_PIN, GPIO_PIN_SET);
			}
			else
			{
			    HAL_GPIO_WritePin(GPS_EN_PORT, GPS_EN_PIN, GPIO_PIN_RESET);			
			}
			interfacial_SetPage(PAGE_3_GPS, PAGE_NOT_BACK);
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

		case PAGE_3_Temp_jingdu:
			if(cur_option == cur_interfacial.option_head)
			{
				setting_SetTemp_jingdu(0);
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				setting_SetTemp_jingdu(5);
			}
			else if(cur_option == cur_interfacial.option_head->next_option->next_option)
			{
				setting_SetTemp_jingdu(10);
			}
			SettingToFlash();//保存一波设置
			break;

		case PAGE_4_Temp_xiaoshu:
			if(cur_option == cur_interfacial.option_head)
			{
				setting_SetTemp_jingdu(0);
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				setting_SetTemp_jingdu(5);
			}
			else if(cur_option == cur_interfacial.option_head->next_option->next_option)
			{
				setting_SetTemp_jingdu(10);
			}
			
			SettingToFlash();//保存一波设置
			break;
		
		
		case PAGE_3_PRESSURE://设置当前气压值
			destory_MessageBox();	
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->next_option->son_option, 10);
			double_value = ((double)temp_value/100.0);
			
			setting_SetAirCompensate(double_value - bmp280_GetPress());

			switch(get_CurDo()->modbus_id)
			{						
				case DO_HF1012_ModbusID:  
			    DO_rs485_SetPressure(get_CurDo(), double_value);//更新do设备的气压值
					break;
				
				case DO_shenghui_ModbusID:  
					DO_shenghui_rs485_SetPressure(get_CurDo(),double_value);	            
					break;	

				case DO_HF_DY12_ModbusID:  
					if(double_value >= 150)
					{
						double_value = 150;
					}
					setting_SetAirCompensate(double_value - bmp280_GetPress());
					DO_HaiFa_DY12_rs485_Set_Cmd_open(get_CurDo());	         
					Set_DY12_Press_Value((int16_t)(double_value * 100));
					Set_DY12_Press_Flag(1);
					break;	

				case DO_HF_DY56_ModbusID:  
					if(double_value >= 150)
					{
						double_value = 150;
					}
					setting_SetAirCompensate(double_value - bmp280_GetPress());
					DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());	         
					Set_DY56_Press_Value((int16_t)(double_value * 100));
					Set_DY56_Press_Flag(1);
					break;	
					
				case DO_DY05_ModbusID:  
					DY05_rs485_SetPressure(get_CurDo(),double_value);	            
					break;	

				case DO_DO59_ModbusID:  
					DO59_rs485_SetPressure(get_CurDo(),double_value);	            
					break;
				
				default:
					break;
			}
			SettingToFlash();
			
			break;
		
		case PAGE_3_SALT://设置盐度值
			destory_MessageBox();	
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->next_option->son_option, 10);
			double_value = ((double)temp_value/10.0);

			setting_SetSalinity(double_value);  //更新设置中的盐度
		
			set_SalArr(setting_GetSalinity()); //刷新盐度显示的buf
		  
			switch(get_CurDo()->modbus_id)
			{						
				case DO_HF1012_ModbusID:  
			    DO_rs485_SetSalinity(get_CurDo(), double_value);
					break;
				
				case DO_shenghui_ModbusID:  
					DO_shenghui_rs485_SetSalinity(get_CurDo(),double_value);	            
					break;	
				
				case DO_HF_DY12_ModbusID:  
					if(double_value >= 100)
					{
						double_value = 100;
					}
					setting_SetSalinity(double_value);  //更新设置中的盐度

					DO_HaiFa_DY12_rs485_Set_Cmd_open(get_CurDo());	         
					Set_DY12_Sal_Value((int16_t)(double_value * 100));
					Set_DY12_Sal_Flag(1);
					break;

				case DO_HF_DY56_ModbusID:  
					if(double_value >= 100)
					{
						double_value = 100;
					}
					setting_SetSalinity(double_value);  //更新设置中的盐度

					DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());	         
					Set_DY56_Sal_Value((int16_t)(double_value * 100));
					Set_DY56_Sal_Flag(1);
					break;
					
				case DO_DY05_ModbusID:  
					DY05_rs485_SetSalinity(get_CurDo(),double_value);	            
					break;	

				case DO_DO59_ModbusID:  
					DO59_rs485_SetSalinity(get_CurDo(),double_value);	            
					break;
				
				default:
					break;
			}
		  	SettingToFlash();                   //将设置写入flash
			break;


		case PAGE_3_UNIT://单位设置界面
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 0);
			setting_Set_Temp_Unit(temp_value);
			SettingToFlash();//保存一波设置
			break;

		
		case PAGE_3_RESERT:
			if(cur_option == cur_interfacial.option_head)
			{
				clear_KeyAllFlag();
				// if(setting_GetIsChn())//如果是中文的话得清一下上面的标签显示的文字
				// {
				// 	gui_ClearChuchangshezhi();
				// }
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
			log_SetLogCount(0,datashow_SensorType);
			break;
		

		case PAGE_3_Set_TDS_coefficient:
			destory_MessageBox();			
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);	
			double_value = ((double)temp_value/1000.0);
			if(get_CurDo() != NULL )
			{						
				switch (get_CurDo()->modbus_id)
				{
				case EC_DE26_ModbusID:
					EC_DE26_SetTds_value(get_CurDo(),double_value);
					break;
				case EC_DE40_ModbusID:
					EC_DE40_SetTds_value(get_CurDo(),double_value);
					break;
				
				default:
					break;
				}
			}
			else
		 	{
				generate_MessageBox(MESSAGE_SUCCESSFUL, 0);		
			}
			break;	

		case PAGE_3_Set_Temp_coefficient:
			destory_MessageBox();			
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);	
			double_value = ((double)temp_value/10000.0);
			if(get_CurDo() != NULL )
			{						
				switch (get_CurDo()->modbus_id)
				{
				case EC_DE26_ModbusID:
					EC_DE26_SetTemp_value(get_CurDo(),double_value);
					break;
				case EC_DE40_ModbusID:
					EC_DE40_SetTemp_value(get_CurDo(),double_value);
					break;
				default:
					break;
				}
			}
			else
		 	{
				generate_MessageBox(MESSAGE_SUCCESSFUL, 0);		
			}
			break;	

		case PAGE_4_KEYPADTONE://设置按键音
			setting_SetKeyPadTone(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;
                                                                                                                                                                
		case PAGE_4_ALARMTONE://设置报警音
			setting_SetAlarmTone(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;

		case PAGE_4_Mes_Set://测量模式设置
			destory_MessageBox();	
			COD_DC17_rs485_Set_Mes(get_CurDo(),((cur_option == cur_interfacial.option_head) ? 1 : 0));
			break;

		case PAGE_4_Mes_Time_Set://单次测量设置时间
			destory_MessageBox();			
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);		
			setting_Setdanciceliangtime(temp_value);
			if(temp_value >= 5 && temp_value <= 255)
			{
				COD_DC17_rs485_Set_Mes_Time(get_CurDo(),temp_value);
			}
			else
			{
				interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn;//校准失败
				interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;
				interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
				
				generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
				
				// cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);
			}
			break;
		
		case PAGE_4_ALARM:
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					setting_SetIsAlarm_DO(0);
					break;

				case TYPE_pH:
					setting_SetIsAlarm_pH(0);

					break;
				
				case TYPE_Tur:
					setting_SetIsAlarm_Tur(0);

					break;
				
				case TYPE_FCL:
					setting_SetIsAlarm_FCL(0);

					break;
				
				case TYPE_EC:
					setting_SetIsAlarm_EC(0);

					break;
				
				case TYPE_ORP:
					setting_SetIsAlarm_ORP(0);

					break;
				
				case TYPE_NH4:
					setting_SetIsAlarm_NH4(0);
	      			break;			

				case TYPE_F:
					setting_SetIsAlarm_F(0);

					break;
				
				case TYPE_CL:
					setting_SetIsAlarm_CL(0);

					break;
				
				case TYPE_Chl:
					setting_SetIsAlarm_Chl(0);

					break;
				
				case TYPE_Bga:
					setting_SetIsAlarm_Bga(0);

					break;
				
				case TYPE_CODuv:
					setting_SetIsAlarm_COD(0);

					break;		

				case TYPE_MLSS:
					setting_SetIsAlarm_MLSS(0);

					break;		

				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						setting_SetIsAlarm_OIW(0);
					}
					else
					{
						setting_SetIsAlarm_OIW_ppm(0);
					}

					break;			
				case TYPE_SAL:
					setting_SetIsAlarm_SAL(0);
					break;	
				
				case TYPE_TDS:
					setting_SetIsAlarm_TDS(0);
					break;
				default:
					break;
			}
			SettingToFlash();//保存一波设置
			break;
		
		case PAGE_4_SENSORINFO://设备信息  设置modbus id
			destory_MessageBox();
			p = DO_FindByName(interfacial_GetOptionSensorName(), rs485_GetDoList());//获取当前设备指针
		
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->next_option->next_option->son_option, 100);
			temp_value = ((temp_value/100)<<4)|(temp_value % 100); // 获取要设置的地址
				
			DO_rs485_SetAddr(p, temp_value);
			
			break;
		
		case PAGE_4_AUTOLOCK:
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_DO(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_DO(AUTOLOCK_MANUAL);
					}
					
					break;

				case TYPE_pH:	
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_pH(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_pH(AUTOLOCK_MANUAL);
					}
			
					break;
				case TYPE_Tur:	
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_Tur(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_Tur(AUTOLOCK_MANUAL);
					}
					break;
				case TYPE_FCL:		
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_CL(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_CL(AUTOLOCK_MANUAL);
					}
					break;
				case TYPE_EC:	
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_EC(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_EC(AUTOLOCK_MANUAL);
					}
					break;
				case TYPE_ORP:	
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_ORP(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_ORP(AUTOLOCK_MANUAL);
					}
					break;
				case TYPE_NH4:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_NH4(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_NH4(AUTOLOCK_MANUAL);
					}
					break;
				case TYPE_F:		
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_F(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_F(AUTOLOCK_MANUAL);
					}		
					break;
				case TYPE_CL:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_CL(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_CL(AUTOLOCK_MANUAL);
					}
					break;
				case TYPE_Chl:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_Chl(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_Chl(AUTOLOCK_MANUAL);
					}					
					break;
				case TYPE_Bga:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_Bga(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_Bga(AUTOLOCK_MANUAL);
					}				
					break;
				case TYPE_CODuv:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_COD(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_COD(AUTOLOCK_MANUAL);
					}			
					break;
				case TYPE_MLSS:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_MLSS(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_MLSS(AUTOLOCK_MANUAL);
					}			
					break;
				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						if(cur_option == cur_interfacial.option_head)
						{
							setting_SetAutoLock_OIW(AUTOLOCK_OFF);
						}
						else if(cur_option == cur_interfacial.option_head->prev_option)
						{
							setting_SetAutoLock_OIW(AUTOLOCK_MANUAL);
						}
					}
					else
					{
						if(cur_option == cur_interfacial.option_head)
						{
							setting_SetAutoLock_OIW_ppm(AUTOLOCK_OFF);
						}
						else if(cur_option == cur_interfacial.option_head->prev_option)
						{
							setting_SetAutoLock_OIW_ppm(AUTOLOCK_MANUAL);
						}
					}
					
					break;
				case TYPE_TSS:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_TSS(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_TSS(AUTOLOCK_MANUAL);
					}			
					break;
				case TYPE_SAL:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_SAL(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_SAL(AUTOLOCK_MANUAL);
					}		
					break;
				case TYPE_TDS:
					if(cur_option == cur_interfacial.option_head)
					{
						setting_SetAutoLock_TDS(AUTOLOCK_OFF);
					}
					else if(cur_option == cur_interfacial.option_head->prev_option)
					{
						setting_SetAutoLock_TDS(AUTOLOCK_MANUAL);
					}		
					break;
				default:
					break;
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
		
		case PAGE_5_AUTOLOCKVALUE:
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					setting_SetAutoLock_DO(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_DO(cur_option->option_index);
					setting_SetAutoLock_num(0);
					break;	
				case TYPE_pH:	
					setting_SetAutoLock_pH(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_pH(cur_option->option_index);
					break;
				case TYPE_Tur:	
					setting_SetAutoLock_Tur(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_Tur(cur_option->option_index);
					break;
				case TYPE_FCL:		
					setting_SetAutoLock_FCL(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_FCL(cur_option->option_index);
					break;
				case TYPE_EC:	
					setting_SetAutoLock_EC(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_EC(cur_option->option_index);			
					break;
				case TYPE_ORP:	
					setting_SetAutoLock_ORP(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_ORP(cur_option->option_index);
					break;
				case TYPE_NH4:
					setting_SetAutoLock_NH4(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_NH4(cur_option->option_index);
					break;
				case TYPE_F:		
					setting_SetAutoLock_F(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_F(cur_option->option_index);
					break;
				case TYPE_CL:
					setting_SetAutoLock_CL(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_CL(cur_option->option_index);
					break;
				case TYPE_Chl:
					setting_SetAutoLock_Chl(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_Chl(cur_option->option_index);				
					break;
				case TYPE_Bga:
					setting_SetAutoLock_Bga(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_Bga(cur_option->option_index);	
					break;
				case TYPE_CODuv:
					setting_SetAutoLock_COD(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_COD(cur_option->option_index);				
					break;
				case TYPE_MLSS:
					setting_SetAutoLock_MLSS(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_MLSS(cur_option->option_index);				
					break;
				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						setting_SetAutoLock_OIW(AUTOLOCK_AUTO);//设置成自动锁定
						setting_SetAutoLockLevel_OIW(cur_option->option_index);	
					}
					else
					{
						setting_SetAutoLock_OIW_ppm(AUTOLOCK_AUTO);//设置成自动锁定
						setting_SetAutoLockLevel_OIW_ppm(cur_option->option_index);	
					}			
					break;
				case TYPE_TSS:
					setting_SetAutoLock_TSS(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_TSS(cur_option->option_index);				
					break;
				case TYPE_SAL:
					setting_SetAutoLock_SAL(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_SAL(cur_option->option_index);				
					break;
				case TYPE_TDS:
					setting_SetAutoLock_TDS(AUTOLOCK_AUTO);//设置成自动锁定
					setting_SetAutoLockLevel_TDS(cur_option->option_index);				
					break;
				default:
					break;
			}
			
			SettingToFlash();//保存一波设置
			break;


		case PAGE_4_CAL_GROUP://标液值管理
			setting_SetIs_pH_Group(((cur_option == cur_interfacial.option_head) ? 0 : 1));
		  pH_DpH07_rs485_SetStander(get_CurDo(), setting_GetIs_pH_Group()+1);
			SettingToFlash();//保存一波设置
			break;
		
		
		case PAGE_4_INTERVALSAVE://间隔保存
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 100);
			temp_value =  ((temp_value / 100) * 3600)+			//取出设置的小时数并换算成秒(第一位数)
						  ((temp_value % 100) * 60);		//取出设置的分钟数并换算成秒(最后两位数)
			setting_SetAutoIntervalTime(temp_value);
			SettingToFlash();//保存一波设置
      		clear_SAVEDATACount();
			break;

		case PAGE_4_LOCKSAVE:////锁定保存
			setting_SetLockSave(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;
		
		case PAGE_4_COD_SET:////DC18的浊度显示
			setting_SetKey_DC18_Set(((cur_option == cur_interfacial.option_head) ? 1 : 0));
			SettingToFlash();//保存一波设置
			break;

		case PAGE_4_Measure_Switch://量程设置
					
			if(get_CurDo() != NULL )
			{						
				get_CurDo()->Measure_Range.value_f=((cur_option == cur_interfacial.option_head) ? 256 : 4095);
				Chl_shenghui_rs485_Measure_Switch(get_CurDo(),get_CurDo()->Measure_Range.value_f);
			}
			else
		 	{
			  generate_MessageBox(MESSAGE_SUCCESSFUL, 0);		
			}
			break;	
		
		case PAGE_4_MesUNIT:
			if(cur_option == cur_interfacial.option_head)
			{
				setting_SetMesUnit(0);
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				setting_SetMesUnit(5);
			}
			else if(cur_option == cur_interfacial.option_head->next_option->next_option)
			{
				setting_SetMesUnit(10);
			}
			else if(cur_option == cur_interfacial.option_head->prev_option)
			{
				setting_SetMesUnit(20);
			}
			
			SettingToFlash();//保存一波设置
			break;

		case PAGE_4_Cal_Num_Set:
			destory_MessageBox();	
			if(cur_option == cur_interfacial.option_head)
			{
				MLSS_Tianjian_rs485_Set_Cal_Num(get_CurDo(),2);
			}
			else if(cur_option == cur_interfacial.option_head->next_option)
			{
				MLSS_Tianjian_rs485_Set_Cal_Num(get_CurDo(),3);
			}
			break;

		case PAGE_4_Model_selection://型号选择
			setting_Set_Type(((cur_option == cur_interfacial.option_head) ? 1 : 0));//580写1,702写0
			if(cur_option == cur_interfacial.option_head)
			{
				data_702_580 = 1;//580
			}
			else
			{
				data_702_580 = 0;//702
			}
			W25QXX_Write(&data_702_580, B580_D702_TYPE_ADDR, 1);
			SettingToFlash();//保存一波设置
			break;

		case PAGE_5_ALARMVALUE://设置是否报警 高低门限阈值 这里要判断一波

			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					setting_SetIsAlarm_DO(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_DO((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_DO((value_type)temp_value/10.0);		
					}
					break;
				case TYPE_pH:
					setting_SetIsAlarm_pH(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_pH((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_pH((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_Tur:
					setting_SetIsAlarm_Tur(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_Tur((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_Tur((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_FCL:
					setting_SetIsAlarm_FCL(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_FCL((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_FCL((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_EC:
					setting_SetIsAlarm_EC(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_EC((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_EC((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_ORP:
					setting_SetIsAlarm_ORP(1);
					if(checked_AlarmValueLegal())
					{
						orp_alm_np_value = NanoOptionList_Get_np_Value(cur_interfacial.option_head->son_option, 10);//低门限
						if (orp_np == 68)
						{
							orp_alm_np_value = orp_alm_np_value;
							setting_SetLowThreshold_ORP((value_type)orp_alm_np_value);
						}
						else if(orp_np == 67)
						{
							orp_alm_np_value = - orp_alm_np_value;
							setting_SetLowThreshold_ORP((value_type)orp_alm_np_value);
						}
						setting_SetOrp_low_Threshold_pn(orp_np);

						orp_alm_np_value = NanoOptionList_Get_np_Value(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						
						if (orp_np == 68)
						{
							orp_alm_np_value = orp_alm_np_value;
							setting_SetHighThreshold_ORP((value_type)orp_alm_np_value);
						}
						else if(orp_np == 67)
						{
							orp_alm_np_value = - orp_alm_np_value;
							setting_SetHighThreshold_ORP((value_type)orp_alm_np_value);
						}
						setting_SetOrp_high_Threshold_pn(orp_np);
					}
					break;
				
				case TYPE_NH4:
					setting_SetIsAlarm_NH4(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_NH4((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_NH4((value_type)temp_value/10.0);		
					}
	      break;			
				case TYPE_F:
					setting_SetIsAlarm_F(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_F((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_F((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_CL:
					setting_SetIsAlarm_CL(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_CL((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_CL((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_Chl:
					setting_SetIsAlarm_Chl(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_Chl((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_Chl((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_Bga:
					setting_SetIsAlarm_Bga(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_Bga((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_Bga((value_type)temp_value/10.0);		
					}
					break;
				
				case TYPE_CODuv:
					setting_SetIsAlarm_COD(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_COD((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_COD((value_type)temp_value/10.0);		
					}
					break;

				case TYPE_MLSS:
					setting_SetIsAlarm_MLSS(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_MLSS((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_MLSS((value_type)temp_value/10.0);		
					}
					break;

				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						setting_SetIsAlarm_OIW(1);
						if(checked_AlarmValueLegal())
						{
							temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
							setting_SetLowThreshold_OIW((value_type)temp_value/10.0);
							temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
							setting_SetHighThreshold_OIW((value_type)temp_value/10.0);		
						}
					}
					else
					{
						setting_SetIsAlarm_OIW_ppm(1);
						if(checked_AlarmValueLegal())
						{
							temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
							setting_SetLowThreshold_OIW_ppm((value_type)temp_value/10.0);
							temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
							setting_SetHighThreshold_OIW_ppm((value_type)temp_value/10.0);		
						}
					}
					break;

				case TYPE_TSS:
					setting_SetIsAlarm_TSS(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_TSS((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_TSS((value_type)temp_value/10.0);		
					}
					break;

				case TYPE_SAL:
					setting_SetIsAlarm_SAL(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_SAL((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_SAL((value_type)temp_value/10.0);	
					}
					break;

				case TYPE_TDS:
					setting_SetIsAlarm_TDS(1);
					if(checked_AlarmValueLegal())
					{
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);//低门限
						setting_SetLowThreshold_TDS((value_type)temp_value/10.0);
						temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);//高门限制
						setting_SetHighThreshold_TDS((value_type)temp_value/10.0);	
					}
					break;
					
				default:
					break;
			}
			SettingToFlash();//保存一波设置
			break;
		case PAGE_4_Mes_Mode:
			destory_MessageBox();			
			switch(rs485_GetSensorType())
			{
				case TYPE_Oiw:
					switch(get_CurDo()->modbus_id)
            		{
						case OiW_guohong_ModbusID:
							if(cur_option == cur_interfacial.option_head)
							{
								OiW_Guohong_rs485_Set_Mes_Model(get_CurDo(),0);
							}
						    else if(cur_option == cur_interfacial.option_head->next_option)
							{
								OiW_Guohong_rs485_Set_Mes_Model(get_CurDo(),2);
							}
							else
							{
								OiW_Guohong_rs485_Set_Mes_Model(get_CurDo(),3);
							}
							break;
						default:
							break;
					}		
					break;
				default:
					break;				
			}			
			break;
		case PAGE_4_Set_Mode_Temp:
			destory_MessageBox();			
			switch(rs485_GetSensorType())
			{
				case TYPE_Oiw:
					switch(get_CurDo()->modbus_id)
            		{
						case OiW_guohong_ModbusID:
							if(cur_option == cur_interfacial.option_head)
							{
								OiW_Guohong_rs485_Set_Temp_Model(get_CurDo(),0);
							}
							else
							{
								OiW_Guohong_rs485_Set_Temp_Model(get_CurDo(),1);
							}
							break;
						default:
							break;
					}		
					break;
				default:
					break;				
			}			
			break;

		case PAGE_4_Temp_k:
			destory_MessageBox();
			temp_int_value = NanoOptionList_Get_np_Value(cur_interfacial.option_head->next_option->son_option, 10);		

			if(orp_np == 68)
			{
				temp_int_value = temp_int_value;
			}
			else if(orp_np == 67)
			{
				temp_int_value = -temp_int_value;
			}
			switch(rs485_GetSensorType())
			{
				case TYPE_Oiw:
					switch(get_CurDo()->modbus_id)
            		{
						case OiW_guohong_ModbusID:
							OiW_Guohong_rs485_Set_Temp_k_Model(get_CurDo(),temp_int_value /10.0);
							break;
						default:
							break;
					}		
					break;
				default:
					break;				
			}	

			break;
		case PAGE_4_Clear_time:
			destory_MessageBox();
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);		

			switch(rs485_GetSensorType())
			{
				case TYPE_Oiw:
					switch(get_CurDo()->modbus_id)
            		{
						case OiW_guohong_ModbusID:
							OiW_Guohong_rs485_Set_Clear_Model(get_CurDo(),temp_value);
							break;
						default:
							break;
					}		
					break;
				case TYPE_TSS:
					DX01_rs485_Set_Clear_time(get_CurDo(),temp_value);
					break;
				default:
					break;				
			}	
			break;


		case PAGE_5_TEMP:
			destory_MessageBox();			
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->next_option->son_option, 10);		
			
			if(get_CurDo()->modbus_id == LH_DX01_ModbusID
			|| get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
			{
				if(setting_GetTemp_jingdu() == 0)
				{
					temp_value *=100;
				}	
				else if(setting_GetTemp_jingdu() == 5)
				{
					temp_value *=10;
				}
			}
			
			if(setting_Get_Temp_Unit())
			{
				switch(get_CurDo()->modbus_id)
				{
					case DO_shenghui_ModbusID:
					case pH_shenghui_ModbusID:
					case EC_shenghui_ModbusID:
					case COD_shenghui_ModbusID:
					case Chl_shenghui_ModbusID:
					case NH3N_shenghui_ModbusID:
					case Tur_shenghui_ModbusID:		
					case LH_DX01_ModbusID:
					case ZS_DZ09_ModbusID:
            			temp_value=(temp_value-3200)/1.8;
						break;
					default:
            			temp_value=(temp_value-320)/1.8;			
						break;
				}	
			}
		
			switch(rs485_GetSensorType())
			{
				case TYPE_DO:
					switch(get_CurDo()->modbus_id)
					{
						case DO_HF1012_ModbusID:
							DO_rs485_SetTemp(get_CurDo(), temp_value / 10.0);	
							break;
						case DO_HF_DY56_ModbusID:
							float_value=temp_value;
							float_value=float_value/10.0;
							float_value=float_value-get_CurDo()->temperature.value_f+get_CurDo()->NH4_Vol.value_f;
							DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());
							Set_DY56_Temp_Cal_Value((int16_t)(float_value*100));
							Set_DY56_Temp_Cal_Flag(1);
							break;
						case DO_shenghui_ModbusID:
							DO_shenghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
							break;
						
						case DO_DY05_ModbusID:
							float_value=temp_value;
						  float_value=float_value/10.0;
						
							if(setting_Get_Temp_Unit())
							{
								float_value=float_value - ( (get_CurDo()->temperature.value_f-32)/1.8) + get_CurDo()->NH4_Vol.value_f;										
							}
							else
							{
								float_value=float_value-get_CurDo()->temperature.value_f + get_CurDo()->NH4_Vol.value_f;							
							}
							
							DY05_rs485_Set_Temp_Cal(get_CurDo(), float_value);
							break;
							
						case DO_DO59_ModbusID:
					
							DO59_rs485_Set_Temp_Cal(get_CurDo(), temp_value / 10.0);
							break;	
							
						default:
							break;
					}										
					break;
					
				case TYPE_pH:
					switch(get_CurDo()->modbus_id)
					{
						case pH_DpH07_ModbusID:
							pH_DpH07_rs485_SetTemp(get_CurDo(), temp_value );
							break;
						case pH_shenghui_ModbusID:
							pH_shenghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
							break;
						default:
							break;
					}			
					break;
					
				case TYPE_Tur:
					switch(get_CurDo()->modbus_id)
					{
						case ZS_DZ09_ModbusID:
							if(setting_Get_Temp_Unit())
							{
								DZ09_rs485_Set_Temp_Cal(get_CurDo(), (int16_t)(temp_value - ((uint32_t)(get_CurDo()->temperature.value_f*100)-3200)/1.8));
							}
							else
							{
								DZ09_rs485_Set_Temp_Cal(get_CurDo(), (int16_t)(temp_value - (uint32_t)(get_CurDo()->temperature.value_f*100)));
							}
							break;
						default:
							break;
					}
					break;
	
				case TYPE_FCL:

					break;
				
				case TYPE_EC:
					switch(get_CurDo()->modbus_id)
            		{
						case EC_shenghui_ModbusID:
							EC_shenghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
							break;
						case EC_DE26_ModbusID:
							EC_DE26_rs485_SetTemp(get_CurDo(), temp_value / 10.0);
							break;
						case EC_DE40_ModbusID:
							EC_DE40_rs485_SetTemp(get_CurDo(), temp_value / 10.0,EC_DE40_ModbusID);
							break;
						default:
							break;
					}		
					break;
				case TYPE_TDS:
					switch(get_CurDo()->modbus_id)
            		{
						case TDS_DT49_Modbus:
							TDS_DT49_rs485_SetTemp(get_CurDo(), temp_value / 10.0);
							break;
						default:
							break;
					}
					break;
				
				case TYPE_SAL:
					switch(get_CurDo()->modbus_id)
            		{
						case EC_DS46_ModbusID:
							EC_DE40_rs485_SetTemp(get_CurDo(), temp_value / 10.0,EC_DS46_ModbusID);
							break;
						default:
							break;
					}		
					break;

				case TYPE_ORP:

					break;
				
				case TYPE_NH4:
					switch(get_CurDo()->modbus_id)
          {
						case NH3N_shenghui_ModbusID:
							NH3N_shenghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
							break;
						case NH3N_DN02_ModbusID:
							NH3N_DN02_rs485_SetTemp(get_CurDo(), temp_value / 10.0);
							break;
						default:
							break;
					}			

					break;
					
				case TYPE_F:

					break;
				
				case TYPE_CL:
					switch(get_CurDo()->modbus_id)
          {
						case Cl_DL312_ModbusID:
							float_value=temp_value;
						  float_value=float_value/10.0;
						
							if(setting_Get_Temp_Unit())
							{
								float_value=float_value - ( (get_CurDo()->temperature.value_f-32)/1.8) + get_CurDo()->sal.value_f;										
							}
							else
							{
								float_value=float_value-get_CurDo()->temperature.value_f + get_CurDo()->sal.value_f;												
							}
							
							DL312_rs485_Set_Temp_Cal(get_CurDo(), float_value);
							break;
						default:
							break;
					}
					break;
				
				case TYPE_Chl:
					switch(get_CurDo()->modbus_id)
            		{
						case Chl_shenghui_ModbusID:
							Chl_shenghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
							break;
						
						default:
							break;
					}	
					break;
					
				case TYPE_Bga:
					switch(get_CurDo()->modbus_id)
            			{
							case Bga_shenghui_ModbusID:
								BGA_shesnghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
								break;
							default:
								break;
						}	
					break;
					
				case TYPE_CODuv:
					switch(get_CurDo()->modbus_id)
            		{
						case COD_shenghui_ModbusID:
							COD_shenghui_rs485_SetTemp(get_CurDo(), temp_value / 100.0);
							break;
						case COD_DC18_ModbusID:	
							if(setting_Get_Temp_Unit())
							{
								temp_int_value=temp_value-(int)( ( (get_CurDo()->temperature.value_f-32)/1.8)*10);												
							}
							else
							{
								temp_int_value=temp_value-(int)(get_CurDo()->temperature.value_f*10);								
							}

							temp_int_value=temp_int_value*10+get_CurDo()->sal.value_f;
							COD_DC18_rs485_SetTemp(get_CurDo(), temp_int_value ,COD_DC18_ModbusID);
							break;

						case COD_DC17_ModbusID:	
							if(setting_Get_Temp_Unit())
							{
								temp_int_value=temp_value-(int)( ( (get_CurDo()->temperature.value_f-32)/1.8)*10);												
							}
							else
							{
								temp_int_value=temp_value-(int)(get_CurDo()->temperature.value_f*10);								
							}

							temp_int_value=temp_int_value*10+get_CurDo()->sal.value_f;
							COD_DC18_rs485_SetTemp(get_CurDo(), temp_int_value ,COD_DC17_ModbusID);
							break;

						default:
							break;
					}	
					break;
				case TYPE_MLSS:
					if(setting_Get_Temp_Unit())
					{
						temp_int_value=temp_value-(int)(((get_CurDo()->temperature.value_f-32)/1.8)*10);												
					}
					else
					{
						temp_int_value=temp_value-(int)(get_CurDo()->temperature.value_f*10);								
					}
					Setting_MLSS_Temp_B((float)temp_int_value /10.0);
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					break;

				case TYPE_TSS:
					if(setting_Get_Temp_Unit())
					{
						DX01_rs485_Set_Temp_Cal(get_CurDo(), (int16_t)(temp_value - ((uint32_t)(get_CurDo()->temperature.value_f*100)-3200)/1.8));
					}
					else
					{
						DX01_rs485_Set_Temp_Cal(get_CurDo(), (int16_t)(temp_value - (uint32_t)(get_CurDo()->temperature.value_f*100)));
					}

					break;
				default:
					break;				
			}			
			break;
				

		case PAGE_5_COD_DC18_Coefficient:
			destory_MessageBox();				
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);	
			get_CurDo()->Measure_Range.value_f=temp_value;
			if(get_CurDo()->modbus_id == COD_DC18_ModbusID)
			{
				COD_DC18_rs485_Coefficient(get_CurDo(), temp_value , COD_DC18_ModbusID);	
			}
			if(get_CurDo()->modbus_id == COD_DC17_ModbusID)
			{
				COD_DC18_rs485_Coefficient(get_CurDo(), temp_value , COD_DC17_ModbusID);	
			}
		  
			break;
		
		case PAGE_5_MLSS_zero_signal:
			temp_SensorType=cur_DO.current_sensor_type;
			switch(temp_SensorType)
			{
				case TYPE_MLSS:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					if(get_CurDo() != NULL )
					{		
						if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
						{
							switch(p->modbus_id)
							{
								case MLSS_Tianjian_ModbusID:
									MLSS_Tianjian_rs485_Set_Signal_two(get_CurDo(),p->DOpercent.value_f,40212);
									break;
								default:
									break;
							}
						}
					}
					else//如果这个时候断开设备的话
					{
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
			
						generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
						cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
					}
					break;

				case TYPE_Oiw:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					if(get_CurDo() != NULL )
					{		
						if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
						{
							switch(p->modbus_id)
							{
								case OiW_guohong_ModbusID:
									OiW_Guohong_rs485_Set_Clear_move(get_CurDo(),1);
									break;
								default:
									break;
							}
						}
					}
					else//如果这个时候断开设备的话
					{
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
			
						generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
						cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
					}
					break;

				case TYPE_TSS:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					if(get_CurDo() != NULL )
					{		
						if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
						{
							switch(p->modbus_id)
							{
								case LH_DX01_ModbusID:
									DX01_rs485_Set_Zero_Cal(get_CurDo(),(int16_t)STD_value);
									break;
								default:
									break;
							}
						}
					}
					else//如果这个时候断开设备的话
					{
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
			
						generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
						cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
					}
					break;
				default:
					break;
			}
			break;
		
		case PAGE_5_DO_ONE_First:
			//不同界面进来的可能设备也不一样区别一下
		  temp_SensorType=cur_DO.current_sensor_type;
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
					{//单点校准校的是饱和氧,饱和氧校准的话校的是K值
						if(get_CurDo() != NULL )
						{						
							switch(p->modbus_id)
							{
								case DO_HF1012_ModbusID:									
									compensate_k_temp = DO_GetKFloat(p) * (STD_value / 100.0 / DO_GetDOPercent(p));//计算值写值
																	
									if(get_CurDo()->manufacturer == manufacturer_hyphive)
									{
										DO_rs485_SetK(get_CurDo(), compensate_k_temp);			
									}
									else 
									{										
										if(fabs(compensate_k_temp - 1.0) <= MAX_EPS_K)
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
									
								case DO_shenghui_ModbusID:
									DO_shenghui_rs485_FullCal(get_CurDo());
									break;
								
								case DO_HF_DY12_ModbusID:
									DO_HaiFa_DY12_rs485_Set_Cmd_open(get_CurDo());
									Set_DY12_Full_Flag(1);
									break;

								case DO_HF_DY56_ModbusID:
									DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());
									Set_DY56_Full_Flag(1);
									break;
								
								case DO_DY05_ModbusID:
									DY05_rs485_Set_Slp_Cal(get_CurDo());
									break;	

								case DO_DO59_ModbusID:
									DO59_rs485_Set_Slp_Cal(get_CurDo());
									break;
								
								default:
									break;
						  	}
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
					
				case TYPE_MLSS:
				case TYPE_CL:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					if(get_CurDo() != NULL )
					{		
						if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
						{
							switch(p->modbus_id)
							{
								case MLSS_Tianjian_ModbusID:
									if (setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
									{
										MLSS_Tianjian_rs485_Set_Signal(get_CurDo(),p->DOpercent.value_f,STD_value,40214);
									}
									else
									{
									MLSS_Tianjian_rs485_Set_Signal(get_CurDo(),p->DOpercent.value_f,STD_value,40214);
									}
									break;
									
								case Cl_DL312_ModbusID:
                  DL312_rs485_Set_Frist_Value(get_CurDo(),STD_value/10.0);
									break;
									
								default:
									break;
							}
						}
					}
					else//如果这个时候断开设备的话
					{
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
			
						generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
						cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
					}
					break;

				case TYPE_Oiw:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					if(get_CurDo() != NULL )
					{		
						if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
						{
							switch(p->modbus_id)
							{
								case OiW_guohong_ModbusID:
									MLSS_Tianjian_rs485_Set_Signal(get_CurDo(),p->DOpercent.value_f,STD_value,40214);
									break;
								default:
									break;
							}
						}
					}
					else//如果这个时候断开设备的话
					{
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
			
						generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
						cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
					}
					break;
				
				default:
					break;
			}
			break;
		
		case PAGE_5_DO_TWO_FIRST:
			temp_SensorType=cur_DO.current_sensor_type;
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

					if(get_CurDo()->modbus_id == DO_HF_DY12_ModbusID)
					{
						OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
   						gui_ClearLines(115, 133, 0);//清除选项
					}
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
					{
						if(get_CurDo() != NULL )
						{
							switch(p->modbus_id)
							{
								case DO_HF1012_ModbusID:
									value0 = STD_value / 100.0;
									real0 = (DO_GetDOPercent(p) - DO_GetBFloat(p)) / DO_GetKFloat(p);

									interfacial_GetCurrentOption()->content_chn = (uint8_t *)xiayidian_cn;
									interfacial_GetCurrentOption()->ChnContent_size = sizeof(xiayidian_cn);
									interfacial_GetCurrentOption()->content_eng = (uint8_t *)xiayidian_en;

									interfacial_GetCurrentOption()->next_page = PAGE_5_DO_TWO_SECOND;

									btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
									btnOkEscFunc_register(btn_OkEscMode_ChangePage);
									break;
								case DO_shenghui_ModbusID:
									if(STD_value >= 95.0)
									{
										DO_shenghui_rs485_FullCal(get_CurDo());		
									}
									else if(STD_value <= 5.0)
									{
										DO_shenghui_rs485_ZeroCal(get_CurDo());	
									}
									break;
									
								case DO_HF_DY12_ModbusID:
									DO_HaiFa_DY12_rs485_Set_Cmd_open(get_CurDo());
									Set_DY12_Zero_Flag(1);
									break;

								case DO_HF_DY56_ModbusID:
									DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());
									Set_DY56_Zero_Flag(1);
									break;

								case DO_DY05_ModbusID:
									DY05_rs485_Set_Zero_Cal(get_CurDo());
									break;
								
								case DO_DO59_ModbusID:
									DO59_rs485_Set_Zero_Cal(get_CurDo());
									break;	
								
								default:
									break;
							}
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
					
				case TYPE_MLSS:
				case TYPE_CL:
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
				
					gui_ClearLines(115, 133, 0);//清除选项
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					if(get_CurDo() != NULL )
					{		
						if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
						{
							switch(p->modbus_id)
							{
								case MLSS_Tianjian_ModbusID:
									if (setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
									{
										MLSS_Tianjian_rs485_Set_Signal(get_CurDo(),p->DOpercent.value_f,STD_value,40218);
									}
									else
									{
									MLSS_Tianjian_rs485_Set_Signal(get_CurDo(),p->DOpercent.value_f,STD_value,40218);
									}
									break;
								case Cl_DL312_ModbusID:
                  DL312_rs485_Set_Secend_Value(get_CurDo(),STD_value/10.0);
									break;
								default:
									break;
							}
						}
					}
					else//如果这个时候断开设备的话
					{
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
			
						generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
						cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//
					}
					break;
					
				
				
				default:
					break;
			}
			break;
			
		case PAGE_5_DO_TWO_SECOND:
			temp_SensorType=cur_DO.current_sensor_type;
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
						
						if(get_CurDo() != NULL){
							 switch(p->modbus_id){
									case DO_HF1012_ModbusID:
										
										if(get_CurDo()->manufacturer == manufacturer_hyphive)
										{
												DO_rs485_SetKB(get_CurDo(), compensate_k_temp, compensate_b_temp); //设置kb值		
										}
										else 
										{
											if(fabs(compensate_k_temp - 1.0) <= MAX_EPS_K && fabs(compensate_b_temp) <= MAX_EPS_B)
											{
											   DO_rs485_SetKB(get_CurDo(), compensate_k_temp, compensate_b_temp); //设置kb值
											}
											else
											{
												interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn; //提示校准失败
												interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;         //英文
												interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
									
												generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
												cur_interfacial.page_father = ((temp_FatherPage == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);//											
											}
											
										}
										break;
									case DO_shenghui_ModbusID:
										if(STD_value >= 95.0){
												DO_shenghui_rs485_FullCal(get_CurDo());		
										}else if(STD_value <= 5.0){
												DO_shenghui_rs485_ZeroCal(get_CurDo());	
										}
										break;
									default:
										break;
								}		
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
				case TYPE_MLSS:
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
					
					if(DO_ValueCheckFirst(p, STD_value))
					{
						MLSS_Cal_2 = get_CurDo()->DOmgl.value_f;
						MLSS_K = STD_value / (MLSS_Cal_2 - MLSS_Cal_1);//0点标液实际测量值和100的标液实际测量值(不为0)
						MLSS_B = - MLSS_K * MLSS_Cal_1;//可以为0
						if(get_CurDo() != NULL)
						{
							switch(p->modbus_id)
							{
								case MLSS_Tianjian_ModbusID:
									// MLSS_Tianjian_rs485_Set_k_b(get_CurDo(),MLSS_K,MLSS_B);
									break;
								default:
								break;
							}		
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
			/*滤波设置相关设置*/
		case PAGE_4_SLIDEAVG:
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					setting_SetIsOpen_SlideAvg_DO(0);	
				break;
				case TYPE_pH:
					setting_SetIsOpen_SlideAvg_pH(0);	
					break;
				case TYPE_Tur:
					setting_SetIsOpen_SlideAvg_Tur(0);	
					break;
				case TYPE_FCL:
					setting_SetIsOpen_SlideAvg_FCL(0);
					break;
				case TYPE_EC:
					setting_SetIsOpen_SlideAvg_EC(0);
					break;
				case TYPE_ORP:
					setting_SetIsOpen_SlideAvg_ORP(0);
					break;
				case TYPE_NH4:
					setting_SetIsOpen_SlideAvg_NH4(0);
					break;
				case TYPE_F:
					setting_SetIsOpen_SlideAvg_F(0);
					break;
				case TYPE_CL:
					setting_SetIsOpen_SlideAvg_CL(0);
					break;
				case TYPE_Chl:
					setting_SetIsOpen_SlideAvg_Chl(0);
					break;
				case TYPE_Bga:
					setting_SetIsOpen_SlideAvg_Bga(0);	
					break;
				case TYPE_CODuv:
					setting_SetIsOpen_SlideAvg_COD(0);
					break;
				case TYPE_MLSS:
					setting_SetIsOpen_SlideAvg_MLSS(0);
					break;
				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						setting_SetIsOpen_SlideAvg_OIW(0);
					}
					else
					{
						setting_SetIsOpen_SlideAvg_OIW_ppm(0);
					}
					break;
				case TYPE_TSS:
					setting_SetIsOpen_SlideAvg_TSS(0);
					break;
				case TYPE_SAL:
					setting_SetIsOpen_SlideAvg_SAL(0);
					break;
				case TYPE_TDS:
					setting_SetIsOpen_SlideAvg_TDS(0);
					break;
				default:
					break;
			}
							
			p = *(rs485_GetDoList());
			if(p == NULL) //安全性判断
			{
				return;
			}else	{
				filter_destroy(&(p->queue_domgl));
				filter_destroy(&(p->queue_dopercent));
				filter_destroy(&(p->queue_temp));
			}			
			SettingToFlash();//保存一波设置
			break;
		case PAGE_4_SENSOR_SLIDEAVG:
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);	
			p = *(rs485_GetDoList());
			if(p == NULL) //安全性判断
			{
				return;
			}		
			switch(cur_DO.DO_list->modbus_id)
			{
				case LH_DX01_ModbusID:
					DX01_rs485_Write_Avg_num(get_CurDo(),temp_value);	
					break;
				case OiW_yushan_ModbusID:
					break;
			}
			break;
		case PAGE_5_SLIDEVALUE:
			
			temp_value = NanoOptionList_GetValue(cur_interfacial.option_head->son_option, 10);	
			p = *(rs485_GetDoList());
			if(p == NULL) //安全性判断
			{
				return;
			}		
			switch(cur_DO.current_sensor_type)
			{
					case TYPE_DO:
						setting_SetSlideAvgTimes_DO(temp_value);
						setting_SetIsOpen_SlideAvg_DO(1);	
						filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_DO());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_DO());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_DO());       //初始化一下℃    数值指针
					break;
					case TYPE_pH:
						setting_SetSlideAvgTimes_pH(temp_value);
						setting_SetIsOpen_SlideAvg_pH(1);	
						filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_pH());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_pH());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_pH());       //初始化一下℃    数值指针
						break;
					case TYPE_Tur:
						setting_SetSlideAvgTimes_Tur(temp_value);
						setting_SetIsOpen_SlideAvg_Tur(1);	
						filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_Tur());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_Tur());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_Tur());       //初始化一下℃    数值指针
						break;
					case TYPE_FCL:
						setting_SetSlideAvgTimes_FCL(temp_value);
						setting_SetIsOpen_SlideAvg_FCL(1);
						filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_FCL());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_FCL());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_FCL());       //初始化一下℃    数值指针
						break;
					case TYPE_EC:
						setting_SetSlideAvgTimes_EC(temp_value);
						setting_SetIsOpen_SlideAvg_EC(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_EC());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_EC());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_EC());       //初始化一下℃    数值指针
						break;
					case TYPE_ORP:
						setting_SetSlideAvgTimes_ORP(temp_value);
						setting_SetIsOpen_SlideAvg_ORP(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_ORP());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_ORP());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_ORP());       //初始化一下℃    数值指针
						break;
					case TYPE_NH4:
						setting_SetSlideAvgTimes_NH4(temp_value);
						setting_SetIsOpen_SlideAvg_NH4(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_NH4());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_NH4());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_NH4());       //初始化一下℃    数值指针
						break;
					case TYPE_F:
						setting_SetSlideAvgTimes_F(temp_value);
						setting_SetIsOpen_SlideAvg_F(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_F());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_F());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_F());       //初始化一下℃    数值指针
						break;
					case TYPE_CL:
						setting_SetSlideAvgTimes_CL(temp_value);
						setting_SetIsOpen_SlideAvg_CL(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_CL());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_CL());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_CL());       //初始化一下℃    数值指针
						break;
					case TYPE_Chl:
						setting_SetSlideAvgTimes_Chl(temp_value);
						setting_SetIsOpen_SlideAvg_Chl(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_Chl());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_Chl());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_Chl());       //初始化一下℃    数值指针
						break;
					case TYPE_Bga:
						setting_SetSlideAvgTimes_Bga(temp_value);
						setting_SetIsOpen_SlideAvg_Bga(1);	
				   	filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_Bga());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_Bga());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_Bga());       //初始化一下℃    数值指针
						break;
					case TYPE_CODuv:
						setting_SetSlideAvgTimes_COD(temp_value);
						setting_SetIsOpen_SlideAvg_COD(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_COD());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_COD());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_COD());       //初始化一下℃    数值指针
						filter_init(&(p->queue_value), setting_GetSlideAvgTimes_COD());       //初始化一下℃    数值指针
						break;
					case TYPE_MLSS:
						setting_SetSlideAvgTimes_MLSS(temp_value);
						setting_SetIsOpen_SlideAvg_MLSS(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_MLSS());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_MLSS());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_MLSS());       //初始化一下℃    数值指针
						break;
					case TYPE_Oiw:
						if(cur_DO.DO_list->modbus_id == OiW_yushan_DA511_ModbusID)
						{
							setting_SetSlideAvgTimes_OIW_ppm(temp_value);
							setting_SetIsOpen_SlideAvg_OIW_ppm(1);
							filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_OIW_ppm());      //初始化一下mg/l 数值指针
							filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_OIW_ppm());  //初始化一下%    数值指针
							filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_OIW_ppm());       //初始化一下℃    数值指针

						}
						else
						{
							setting_SetSlideAvgTimes_OIW(temp_value);
							setting_SetIsOpen_SlideAvg_OIW(1);
							filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_OIW());      //初始化一下mg/l 数值指针
							filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_OIW());  //初始化一下%    数值指针
							filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_OIW());       //初始化一下℃    数值指针
						}
						break;
					case TYPE_TSS:
						setting_SetSlideAvgTimes_TSS(temp_value);
						setting_SetIsOpen_SlideAvg_TSS(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_TSS());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_TSS());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_TSS());       //初始化一下℃    数值指针
						break;

					case TYPE_SAL:
						setting_SetSlideAvgTimes_SAL(temp_value);
						setting_SetIsOpen_SlideAvg_SAL(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_SAL());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_SAL());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_SAL());       //初始化一下℃    数值指针
	
						break;
					
					case TYPE_TDS:
						setting_SetSlideAvgTimes_TDS(temp_value);
						setting_SetIsOpen_SlideAvg_TDS(1);
					  filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_TDS());      //初始化一下mg/l 数值指针
						filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_TDS());  //初始化一下%    数值指针
						filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_TDS());       //初始化一下℃    数值指针
	
						break;
					
					default:
						break;
			}		

			SettingToFlash();//保存一波设置
			break;
			
			
			
			case PAGE_5_NH3N_ONE:		
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
						if(get_CurDo() != NULL )
						{							
							switch(get_CurDo()->modbus_id)
							{						
								case NH3N_DN02_ModbusID:  
                  NH3N_DN02_rs485_Frist(get_CurDo(),STD_value);	
									break;
								
								case NH3N_shenghui_ModbusID:  
                  NH3N_shenghui_rs485_Frist (get_CurDo(),STD_value);	            
									break;	
								
								default:
									break;
							}
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
			
			case PAGE_5_NH3N_TWO:		
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
						if(get_CurDo() != NULL )
						{							
							switch(get_CurDo()->modbus_id)
							{						
								case NH3N_DN02_ModbusID:  
                  NH3N_DN02_rs485_Second(get_CurDo(),STD_value);	
									break;
								
								case NH3N_shenghui_ModbusID:  
                  NH3N_shenghui_rs485_Second(get_CurDo(),STD_value);	            
									break;	
								
								default:
									break;
							}
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
					
					
			case PAGE_5_NH3N_pH_ONE:		
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
						if(get_CurDo() != NULL )
						{							
							switch(get_CurDo()->modbus_id)
							{						
								case NH3N_DN02_ModbusID:  
                  NH3N_DN02_pH_rs485_Frist(get_CurDo(),STD_value);	
									break;
								
								case NH3N_shenghui_ModbusID:  
                  NH3N_shenghui_pH_rs485_Frist(get_CurDo(),STD_value);	            
									break;	
								
								case pH_shenghui_ModbusID:  
                  pH_shenghui_rs485_Frist(get_CurDo(),STD_value);	    							
									break;	

								case pH_DpH07_ModbusID:  
									if(!setting_GetIs_pH_Group()){
                     pH_DpH07_rs485_Cal_pH(get_CurDo(),0x12);										
									}else{
                     pH_DpH07_rs485_Cal_pH(get_CurDo(),0x22);										
									}	    							
									break;	
								
								default:
									break;
							}
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

			case PAGE_5_NH3N_pH_TWO:		
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
						if(get_CurDo() != NULL )
						{							
							switch(get_CurDo()->modbus_id)
							{						
								case NH3N_DN02_ModbusID:  
                  NH3N_DN02_pH_rs485_Second(get_CurDo(),STD_value);	
									break;
								
								case NH3N_shenghui_ModbusID:  
                  NH3N_shenghui_pH_rs485_Second(get_CurDo(),STD_value);	            
									break;	

								case pH_shenghui_ModbusID:  
                  pH_shenghui_rs485_Second(get_CurDo(),STD_value);	            
									break;	

								case pH_DpH07_ModbusID:  
									if(!setting_GetIs_pH_Group()){
                     pH_DpH07_rs485_Cal_pH(get_CurDo(),0x11);										
									}else{
                     pH_DpH07_rs485_Cal_pH(get_CurDo(),0x21);										
									}   		        
									break;	
								
								default:
									break;
							}
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
					
			case PAGE_5_NH3N_pH_THREE:		
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
						if(get_CurDo() != NULL )
						{							
							switch(get_CurDo()->modbus_id)
							{						
								case NH3N_DN02_ModbusID:  
                  NH3N_DN02_pH_rs485_Third(get_CurDo(),STD_value);	
									break;
								
								case NH3N_shenghui_ModbusID:  
                  NH3N_shenghui_pH_rs485_Third(get_CurDo(),STD_value);	            
									break;	

								case pH_shenghui_ModbusID:  
                  pH_shenghui_rs485_Third(get_CurDo(),STD_value);	            
									break;	

								case pH_DpH07_ModbusID:  
									if(!setting_GetIs_pH_Group()){
                     pH_DpH07_rs485_Cal_pH(get_CurDo(),0x13);										
									}else{
                     pH_DpH07_rs485_Cal_pH(get_CurDo(),0x23);										
									} 		          
									break;	
								
								default:
									break;
							}
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
					

			case PAGE_5_COD_shenghui_Tur_ONE:		
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
						if(get_CurDo() != NULL )
						{	
	              COD_shenghui_Tur_rs485_Frist(get_CurDo(),STD_value);									   							
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
					
			case PAGE_5_COD_shenghui_Tur_TWO:		
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
						if(get_CurDo() != NULL )
						{							
                COD_shenghui_Tur_rs485_Second(get_CurDo(),STD_value);	
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
					
			case PAGE_5_COD_shenghui_ONE:		
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
						if(get_CurDo() != NULL )
						{							
                COD_shenghui_rs485_Frist(get_CurDo(),STD_value);	
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
					
			case PAGE_5_COD_shenghui_TWO:		
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
						if(get_CurDo() != NULL )
						{							
                COD_shenghui_rs485_Second(get_CurDo(),STD_value);	
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
					
			case PAGE_5_COD_shenghui_THREE:		
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
						if(get_CurDo() != NULL )
						{							
                COD_shenghui_rs485_Third(get_CurDo(),STD_value);	
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
			case PAGE_5_COD_DC17_IN_Tur_Zero:
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
						if(get_CurDo() != NULL )
						{							
                		COD_DC17_rs485_IN_tur_ZeroCal(get_CurDo(),STD_value);	
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

			case PAGE_5_COD_DC18_Tur_Zero:		
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
						if(get_CurDo() != NULL )
						{				
							if(get_CurDo()->modbus_id == COD_DC18_ModbusID)
							{
                				COD_DC18_Tur_rs485_ZeroCal(get_CurDo(),STD_value,COD_DC18_ModbusID);
							}
							if(get_CurDo()->modbus_id == COD_DC17_ModbusID)
							{
                				COD_DC18_Tur_rs485_ZeroCal(get_CurDo(),STD_value,COD_DC17_ModbusID);
							}
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

			case PAGE_5_COD_DC18_Tur_Slope:		
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
						if(get_CurDo() != NULL )
						{		
							if(get_CurDo()->modbus_id == COD_DC18_ModbusID)
							{
                				COD_DC18_Tur_rs485_SlopeCal(get_CurDo(),STD_value,COD_DC18_ModbusID);
							}
							if(get_CurDo()->modbus_id == COD_DC17_ModbusID)
							{
                				COD_DC18_Tur_rs485_SlopeCal(get_CurDo(),STD_value,COD_DC17_ModbusID);
							}
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

			case PAGE_5_COD_DC18_Zero:		
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
						if(get_CurDo() != NULL )
						{							
							if(get_CurDo()->modbus_id == COD_DC18_ModbusID)
                			{
								COD_DC18_rs485_ZeroCal(get_CurDo(),STD_value,COD_DC18_ModbusID);	
							}
							if(get_CurDo()->modbus_id == COD_DC17_ModbusID)
                			{
								COD_DC18_rs485_ZeroCal(get_CurDo(),STD_value,COD_DC17_ModbusID);	
							}
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

			case PAGE_5_COD_DC17_IN_Zero:
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
						if(get_CurDo() != NULL )
						{							
                COD_DC17_rs485_INZeroCal(get_CurDo(),STD_value);	
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
					
	case PAGE_5_COD_DC18_Slope:		
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
						if(get_CurDo() != NULL )
						{				
							if(get_CurDo()->modbus_id == COD_DC18_ModbusID)			
               				{	
								COD_DC18_rs485_SlopeCal(get_CurDo(),STD_value,COD_DC18_ModbusID);
							}
							if(get_CurDo()->modbus_id == COD_DC17_ModbusID)			
               				{	
								COD_DC18_rs485_SlopeCal(get_CurDo(),STD_value,COD_DC17_ModbusID);
							}
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


					
	  case PAGE_5_COD_Cleanse:			
		   	destory_MessageBox();
		    switch(get_CurDo()->modbus_id)
			{			
				case COD_DC18_ModbusID:  
					COD_DC18_rs485_Cleanse(get_CurDo(),1,COD_DC18_ModbusID); //清洗
					break;

				case COD_DC17_ModbusID:  
					COD_DC18_rs485_Cleanse(get_CurDo(),1,COD_DC17_ModbusID); //清洗
					break;

				case COD_shenghui_ModbusID:  
					COD_shenghui_rs485_Cleanse(get_CurDo()); //清洗
					break;	
				
				default:
					break;				
			}				
				break;
				
			case PAGE_5_shenghui_Tur_ONE:		
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
			
					gui_ClearLines(115, 133, 0);//清除选项
					
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS || temp_FatherPage == NONE_PAGE)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
					{
						if(p != NULL )
						{
							switch(p->modbus_id)
							{
								case Tur_shenghui_ModbusID:
									Tur_shenghui_rs485_Frist(get_CurDo(),STD_value);
									break;
								case OiW_guohong_ModbusID:
									OiW_Guohong_rs485_Cal(get_CurDo(),STD_value /1000.0f,1);
									break;
								case ZS_DZ09_ModbusID:
									STD_value = STD_value *10;
									DZ09_rs485_Set_Zero_Cal(get_CurDo(),(int16_t)STD_value);
									break;
								default:
									break;
							}									   							
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

			case PAGE_5_shenghui_Tur_TWO:		
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
			
					gui_ClearLines(115, 133, 0);//清除选项
					
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS || temp_FatherPage == NONE_PAGE)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
					{
						if(p != NULL )
						{	
							switch(p->modbus_id)
							{
								case Tur_shenghui_ModbusID:
									Tur_shenghui_rs485_Second(get_CurDo(),STD_value);
									break;
								case OiW_guohong_ModbusID:
									OiW_Guohong_rs485_Cal(get_CurDo(),STD_value /1000.0f,2);
									break;
								case ZS_DZ09_ModbusID:
									DZ09_rs485_Set_Slp_Cal(get_CurDo(),(STD_value/10.0f));
									break;
								default:
									break;
							}								   							
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
					
			case PAGE_5_shenghui_Tur_THREE:		
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
			
					gui_ClearLines(115, 133, 0);//清除选项
					
					//这里的话是判断哪个do设备，可能多个do然后就在列表中校准的就不是当前主界面上的do设备
					if(temp_FatherPage == PAGE_0_START)
					{
						p = get_CurDo();
					}
					else if(temp_FatherPage == PAGE_3_SENSORS || temp_FatherPage == NONE_PAGE)
					{
						p = DO_FindByName(interfacial_GetOptionSensorName(),rs485_GetDoList());
					}
					
					destory_MessageBox();
					
					if(DO_ValueCheckFirst(p, STD_value)) //如果数据符合要求的话
					{
						if(p != NULL )
						{	
							switch(p->modbus_id)
							{
								case Tur_shenghui_ModbusID:
									Tur_shenghui_rs485_Third(get_CurDo(),STD_value);
									break;
								case OiW_guohong_ModbusID:
									OiW_Guohong_rs485_Cal(get_CurDo(),STD_value /1000.0f,3);
									break;
								default:
									break;
							}										   							
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

			case PAGE_5_shenghui_BGA_ONE:
			case PAGE_5_shenghui_BGA_TWO:
					OptionList_Destory(&(interfacial_GetCurrentInterfacial()->option_head));  //销毁选项链表
			
					gui_ClearLines(115, 133, 0);//清除选项
					destory_MessageBox();
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
						if(get_CurDo() != NULL )
						{	
							switch(get_CurDo()->modbus_id)
							{									
								case Bga_shenghui_ModbusID:  
									if(cur_PageNum == PAGE_5_shenghui_BGA_ONE)
									{
										BGA_shenghui_cal_rs485_Frist(get_CurDo(),STD_value);	
									}
									else if(cur_PageNum == PAGE_5_shenghui_BGA_TWO)
									{
										BGA_shenghui_cal_rs485_Second(get_CurDo(),STD_value);
									}
									break;	
																
								default:
									break;
							}
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
			case PAGE_5_shenghui_EC_ONE:	
			case PAGE_5_DR31_ORP_ONE:	
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
						if(get_CurDo() != NULL )
						{	
							
								switch(get_CurDo()->modbus_id)
								{									
									case EC_DE26_ModbusID:  
	              					    EC_DE26_rs485_Frist(get_CurDo(),STD_value,EC_DE26_ModbusID);		
										break;	

									case EC_DE40_ModbusID:
										EC_DE26_rs485_Frist(get_CurDo(),STD_value,EC_DE40_ModbusID);
										break;
									
									case TDS_DT49_Modbus:
										TDS_DT49_rs485_Standard(get_CurDo(),STD_value);
										break;

									case EC_shenghui_ModbusID:  
	                  					EC_shenghui_rs485_Frist(get_CurDo(),STD_value);	
										break;	
									
									case EC_DS46_ModbusID:
										EC_DE40_rs485_SAL_cal(get_CurDo(),STD_value,EC_DS46_ModbusID);
										break;

									case Chl_shenghui_ModbusID:  
										Chl_shenghui_rs485_Second(get_CurDo(),STD_value);	
										break;			

									case ORP_DR31_ModbusID:
										ORP_DR31_rs485_CAL(get_CurDo(),ORP_CAL_value);	
										break;
									case MLSS_Tianjian_ModbusID:
										MLSS_Tianjian_rs485_Set_K(get_CurDo(),STD_value/100);	
										break;
									case OiW_guohong_ModbusID:
										OiW_Guohong_rs485_Set_OIW_B(get_CurDo(),STD_value/1000);	
										break;
									case OiW_yushan_ModbusID:
										OiW_yushan_rs485_Set_OIW_K_b_Reset(get_CurDo(),get_CurDo()->sal.value_f,(STD_value/100)-(get_CurDo()->DOmgl.value_f -get_CurDo()->press.value_f ),OiW_yushan_ModbusID);	
										break;
									case OiW_yushan_DA511_ModbusID:
										OiW_yushan_rs485_Set_OIW_K_b_Reset(get_CurDo(),get_CurDo()->sal.value_f,(STD_value/100)-(get_CurDo()->DOmgl.value_f -get_CurDo()->press.value_f ),OiW_yushan_DA511_ModbusID);	
										break;
									default:
										break;
								}
							   							
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
			case PAGE_5_OIW_K_CAL:
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
						if(get_CurDo() != NULL )
						{	
							
								switch(get_CurDo()->modbus_id)
								{									
									case OiW_guohong_ModbusID:
										OiW_Guohong_rs485_Set_OIW_K(get_CurDo(),STD_value);	
										break;
									case OiW_yushan_ModbusID:
										OiW_yushan_rs485_Set_OIW_K_b_Reset(get_CurDo(),((STD_value - get_CurDo()->press.value_f )/((get_CurDo()->DOmgl.value_f - get_CurDo()->press.value_f)/ get_CurDo()->sal.value_f)),get_CurDo()->press.value_f,OiW_yushan_ModbusID);
										break;
									case OiW_yushan_DA511_ModbusID:
										OiW_yushan_rs485_Set_OIW_K_b_Reset(get_CurDo(),((STD_value - get_CurDo()->press.value_f )/((get_CurDo()->DOmgl.value_f - get_CurDo()->press.value_f)/ get_CurDo()->sal.value_f)),get_CurDo()->press.value_f,OiW_yushan_DA511_ModbusID);
										break;
									case LH_DX01_ModbusID:
										DX01_rs485_Set_Slp_Cal(get_CurDo(),STD_value);	
										break;
									default:
										break;
								}
							   							
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
			case PAGE_5_DE26_EC_Zero:		
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
						if(get_CurDo() != NULL )
						{					
							switch(get_CurDo()->modbus_id)
							{									
								case EC_DE26_ModbusID:  
                  					EC_DE26_rs485_Zero(get_CurDo(),STD_value);				
									break;	
								
								case EC_DE40_ModbusID:
									EC_DE40_rs485_Zero(get_CurDo(),STD_value);		
									break;
								
								case TDS_DT49_Modbus:
									TDS_DT49_rs485_Zero(get_CurDo(),STD_value);		
									break;

								case Chl_shenghui_ModbusID:  
									Chl_shenghui_rs485_Frist(get_CurDo(),STD_value);	
									break;	
								
								case MLSS_Tianjian_ModbusID:
									if(setting_GetMesUnit() == 0|| setting_GetMesUnit() == 10)
									{
										MLSS_Tianjian_rs485_SetZero(get_CurDo(),STD_value);
									}
									else
									{
										MLSS_Tianjian_rs485_SetZero(get_CurDo(),STD_value);
									}
									break;

								default:
									break;
							}													
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
		
		case PAGE_2_HISTORY://历史数据
			generate_Histor(&cur_interfacial);
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
	
		case PAGE_2_HISTORY_ShowOPTION://历史数据查看选择
			generate_Histor_ShowOption(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
//			if(IsBack)
//			{
//				Option_InitByIndex(GrandpaPage_OptionIndex);
//			}
//			else
//			{
//				CurOption_init();
//			}
		  CurOption_init();
			break;
		
		case PAGE_2_HISTORY_DeleteOPTION://历史数据删除选择
			generate_Histor_DeleteOption(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
//			if(IsBack)
//			{
//				Option_InitByIndex(GrandpaPage_OptionIndex);
//			}
//			else
//			{
//				CurOption_init();
//			}
			CurOption_init();
			break;
		
		case PAGE_3_SENSORS://具体传感器界面 
			generate_Sensors(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
//			if(IsBack)
//			{
//				Option_InitByIndex(FatherPage_OptionIndex);
//			}
//			else
//			{
//				CurOption_init();
//			}
			
				CurOption_init();
			break;
		
		case PAGE_3_SENSORSSEARCH://搜索传感器界面
			generate_SensorSearch(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
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

		case PAGE_3_UNIT://单位设置界面
			generate_Unit(&cur_interfacial, setting_Get_Temp_Unit());
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
//			BinaryOption_init(setting_GetUnit());
			break;
		
		case PAGE_3_AUTOSHUT://自动关机
			generate_AutoShut(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoShutOption_init(setting_GetAutoShut());
			break;

		case PAGE_3_Temp_jingdu://温度精度设置
			generate_DZ09_Temp_xiaoshu(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoShutOption_init(setting_GetTemp_jingdu());
			break;

		case PAGE_3_AUTOSAVE://自动保存
			generate_AutoSave(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
//			if(IsBack)
//			{
//				Option_InitByIndex(Page_3_OptionIndex);
//			}
//			else
//			{
//				CurOption_init();
//			}
			break;
			
//		case PAGE_3_ALARM_TYPE://报警设置
//			generate_AlarmType(&cur_interfacial);
//			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
//			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
//			CurOption_init();
//			break;
		
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
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			BinaryOption_init(setting_GetIsKeyGps());
			break;
		
		case PAGE_3_INFO://仪表信息
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
			
		case PAGE_3_DATASAVE:
			generate_DataStore(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_NULL);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_3_DATASHOW://数据记录显示
			if(interfacial_GetCurPage() == PAGE_2_HISTORY_ShowOPTION)//从历史数据界面进去默认指向最后一条
			{
			   cur_LogIndex = log_GetLogCount(datashow_SensorType);   
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
		
		case PAGE_3_Set_TDS_coefficient:
			generate_DE26_EC_tds_value(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();
			break;

		case PAGE_3_Set_Temp_coefficient:
			generate_DE26_EC_temp_data_value(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();
			break;
		
		case PAGE_3_Set_Sal_uint:
			generate_Sal_set_uint(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			SAL_UNIT_init(get_CurDo()->Measure_Range.value_f);
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
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					BinaryOption_init(setting_GetIsAlarm_DO());
					break;
				case TYPE_pH:
					BinaryOption_init(setting_GetIsAlarm_pH());
					break;
				case TYPE_Tur:
					BinaryOption_init(setting_GetIsAlarm_Tur());
					break;
				case TYPE_FCL:
					BinaryOption_init(setting_GetIsAlarm_FCL());
					break;
				case TYPE_EC:
					BinaryOption_init(setting_GetIsAlarm_EC());
					break;
				case TYPE_ORP:
					BinaryOption_init(setting_GetIsAlarm_ORP());
					break;
				case TYPE_NH4:
					BinaryOption_init(setting_GetIsAlarm_NH4());
					break;
				case TYPE_F:
					BinaryOption_init(setting_GetIsAlarm_F());
					break;
				case TYPE_CL:
					BinaryOption_init(setting_GetIsAlarm_CL());
					break;
				case TYPE_Chl:
					BinaryOption_init(setting_GetIsAlarm_Chl());
					break;
				case TYPE_Bga:
					BinaryOption_init(setting_GetIsAlarm_Bga());
					break;
				case TYPE_CODuv:
					BinaryOption_init(setting_GetIsAlarm_COD());
					break;
				case TYPE_MLSS:
					BinaryOption_init(setting_GetIsAlarm_MLSS());
					break;
				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						BinaryOption_init(setting_GetIsAlarm_OIW());
					}
					else
					{
						BinaryOption_init(setting_GetIsAlarm_OIW_ppm());
					}
					break;
				case TYPE_TSS:
					BinaryOption_init(setting_GetIsAlarm_TSS());
					break;
				case TYPE_SAL:
					BinaryOption_init(setting_GetIsAlarm_SAL());
					break;
				case TYPE_TDS:
					BinaryOption_init(setting_GetIsAlarm_TDS());
					break;
				default:
					break;
				
			}
			break;
		
		case PAGE_4_CAL://校准界面
			if(interfacial_GetCurPage() == PAGE_0_START || interfacial_GetCurPage() == PAGE_3_SENSORS)
			{
				temp_FatherPage = interfacial_GetCurPage();
			}
			generate_Cal(&cur_interfacial,temp_FatherPage);
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
		
			//测量单位设置
		case PAGE_4_MesUNIT:
			generate_SetMes_Unit(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoShutOption_init(setting_GetMesUnit());
			break;

		case PAGE_4_Mes_Mode:
			generate_SetTemp_para_Mode(&cur_interfacial,PAGE_4_Mes_Mode);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoShutOption_init(Get_OIW_Mes_Mode());
			break;

		case PAGE_4_Set_Mode_Temp:
			generate_SetTemp_para_Mode(&cur_interfacial,PAGE_4_Set_Mode_Temp);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			Option_InitByIndex(Get_OIW_Temp_mode());
			break;

		case PAGE_4_Cal_Info:
			generate_Sensor_Cal_Info(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;

		case PAGE_4_Temp_k:
			generate_Temp_Clear_Time(&cur_interfacial,PAGE_4_Temp_k);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;

		case PAGE_4_Clear_time:
			generate_Temp_Clear_Time(&cur_interfacial,PAGE_4_Clear_time);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;

		case PAGE_4_Temp_xiaoshu:
			generate_DX01_Temp_xiaoshu(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			AutoShutOption_init(setting_GetTemp_jingdu());
			break;

			//标定点数设置
		case PAGE_4_Cal_Num_Set:
			generate_SetCal_Num(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			Option_InitByIndex(get_CurDo()->compensate_b.value_f - 2);
			break;

		case PAGE_4_PARASET:
			generate_ParameterSetting(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(IsBack)
			{
				Option_InitByIndex(FatherPage_OptionIndex);
				refresh_window(cur_option, cur_interfacial.option_head);
			}
			else
			{
				CurOption_init();
			}
			break;
		
		case PAGE_4_SENSORINFO://传感器信息
			generate_SensorInfo(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_4_DATALOG:
			generate_DataLog(&cur_interfacial);
			if(setting_GetHaveGps())
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangePage);
			}
			else
			{
				btnUpDownFunc_register(btn_UpDownMode_NULL);
			}
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		case PAGE_4_DATALOG_GPS:
			generate_DataLogGPS(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangePage);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;

		case PAGE_4_CAL_GROUP://pH的标液组设置
			generate_pH_Standard_liquid_management(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(!setting_GetIs_pH_Group());
			break;
	
		case PAGE_4_Measure_Switch://Chl的量程设置
			generate_Chl_Measure_Switch(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(!(((get_CurDo()->Measure_Range.value_f == 256) ? 0 : 1)));
			break;

		case PAGE_4_Current_absorbance://Chl的量程设置
			generate_shenghui_COD_Current_absorbance(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;		

		case PAGE_4_Mes_Set://测量模式设置
			generate_Set_Mes_mode(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			if(get_CurDo()->DC17_Mes_Para.value_f == 1)
			{
				BinaryOption_init(1);
			}
			else if(get_CurDo()->DC17_Mes_Para.value_f == 2)
			{
				BinaryOption_init(0);
			}
			break;

		case PAGE_4_Mes_Time_Set://测量间隔设置
			generate_Set_Mes_Time(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;
		case PAGE_4_INTERVALSAVE://间隔保存
			generate_IntervaSave(&cur_interfacial,setting_GetAutoIntervalTime(),setting_GetIsChn());
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();				
			break;	
		
		case PAGE_4_LOCKSAVE://锁定保存
			generate_LockSave(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(setting_GetLockSave());		
			break;		
		
		case PAGE_4_COD_SET://DC18的浊度显示开关
			generate_DC18_Set(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(setting_GetIsKey_DC18_Set());		
			break;		

		case PAGE_4_INCAL://内部校准
			generate_INCAL(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_4_FACTORY://工厂模式
			generate_FACTORY(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;

		case PAGE_4_Model_selection://型号选择
			generate_Model_selection(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			BinaryOption_init(setting_Get_Type());	
			break;

		case PAGE_5_ALARMVALUE://高低门限报警值
			generate_AlarmValueSetting(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();
			break;
		
		case PAGE_5_DO_ONE_First://单点校准
		  temp_SensorType=cur_DO.current_sensor_type;
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
			if(temp_SensorType == TYPE_DO)
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				BinaryOption_init(SELECTED);
			}
			else
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
				btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
				NanoOption_init();
			}
			break;

		case PAGE_5_MLSS_zero_signal:
			temp_SensorType=cur_DO.current_sensor_type;
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
			generate_MLSS_ZERO(&cur_interfacial, temp_SensorType);
			if((interfacial_GetOptionSensorName()[0] == 'O' && interfacial_GetOptionSensorName()[1] == 'i' && interfacial_GetOptionSensorName()[2] == 'W'))
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				BinaryOption_init(SELECTED);
			}
			else
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
				btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
				NanoOption_init();
			}
			break;

		case PAGE_5_DO_TWO_FIRST://两点校准的第一个点
		  temp_SensorType=cur_DO.current_sensor_type;
			if(temp_FatherPage == PAGE_3_SENSORS)
			{
				if(interfacial_GetOptionSensorName()[0] == 'D')
				{
					temp_SensorType = TYPE_DO;
				}
				else if(interfacial_GetOptionSensorName()[0] == 'M')
				{
					temp_SensorType = TYPE_MLSS;
				}

			}
			else if(temp_FatherPage == PAGE_0_START)
			{
				temp_SensorType = rs485_GetSensorType();
			}
			generate_TwoPointFirst(&cur_interfacial, temp_SensorType);
			if(temp_SensorType == TYPE_DO)
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				BinaryOption_init(SELECTED);
			}
			else
			{
				btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
				btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
				NanoOption_init();	
			}
			break;
			
		case PAGE_5_DO_TWO_SECOND://两点校准的第二个点
			if(temp_FatherPage == PAGE_3_SENSORS)
			{
				if(interfacial_GetOptionSensorName()[0] == 'D')
				{
					temp_SensorType = TYPE_DO;
				}
				else if(interfacial_GetOptionSensorName()[0] == 'M')
				{
					temp_SensorType = TYPE_MLSS;
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
			
		case PAGE_5_TEMP://温度校准
			if(temp_FatherPage == PAGE_3_SENSORS)
			{
        temp_SensorType = rs485_GetSensorType();
			}
			else if(temp_FatherPage == PAGE_0_START)
			{
				temp_SensorType = rs485_GetSensorType();
			}
			generate_Cal_temp(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
			NanoOption_init();	
			break;
		
		
		case PAGE_4_AUTOLOCK:
			generate_AutoLock(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					AutoLockOption_init(setting_GetAutoLock_DO());
					break;
				case TYPE_pH:
					AutoLockOption_init(setting_GetAutoLock_pH());
					break;
				case TYPE_Tur:
					AutoLockOption_init(setting_GetAutoLock_Tur());
					break;
				case TYPE_FCL:
					AutoLockOption_init(setting_GetAutoLock_FCL());
					break;
				case TYPE_EC:
					AutoLockOption_init(setting_GetAutoLock_EC());
					break;
				case TYPE_ORP:
					AutoLockOption_init(setting_GetAutoLock_ORP());
					break;
				case TYPE_NH4:
					AutoLockOption_init(setting_GetAutoLock_NH4());
					break;
				case TYPE_F:
					AutoLockOption_init(setting_GetAutoLock_F());
					break;
				case TYPE_CL:
					AutoLockOption_init(setting_GetAutoLock_CL());
					break;
				case TYPE_Chl:
					AutoLockOption_init(setting_GetAutoLock_Chl());
					break;
				case TYPE_Bga:
					AutoLockOption_init(setting_GetAutoLock_Bga());
					break;
				case TYPE_CODuv:
					AutoLockOption_init(setting_GetAutoLock_COD());
					break;
				case TYPE_MLSS:
					AutoLockOption_init(setting_GetAutoLock_MLSS());
					break;
				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						AutoLockOption_init(setting_GetAutoLock_OIW());
					}
					else
					{
						AutoLockOption_init(setting_GetAutoLock_OIW_ppm());
					}
					break;
				case TYPE_TSS:
					AutoLockOption_init(setting_GetAutoLock_TSS());
					break;
				case TYPE_SAL:
					AutoLockOption_init(setting_GetAutoLock_SAL());
					break;
				case TYPE_TDS:
					AutoLockOption_init(setting_GetAutoLock_TDS());
					break;
				default:
					break;
			}
			break;
			
		case PAGE_5_AUTOLOCKVALUE:
			generate_AutoLock_value(&cur_interfacial);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			switch(cur_DO.current_sensor_type)
			{
				case TYPE_DO:
					AutoLockOption_init(setting_GetAutoLockLevel_DO());
					break;
				case TYPE_pH:
					AutoLockOption_init(setting_GetAutoLockLevel_pH());
					break;
				case TYPE_Tur:
					AutoLockOption_init(setting_GetAutoLockLevel_Tur());
					break;
				case TYPE_FCL:
					AutoLockOption_init(setting_GetAutoLockLevel_FCL());
					break;
				case TYPE_EC:
					AutoLockOption_init(setting_GetAutoLockLevel_EC());
					break;
				case TYPE_ORP:
					AutoLockOption_init(setting_GetAutoLockLevel_ORP());
					break;
				case TYPE_NH4:
					AutoLockOption_init(setting_GetAutoLockLevel_NH4());
					break;
				case TYPE_F:
					AutoLockOption_init(setting_GetAutoLockLevel_F());
					break;
				case TYPE_CL:
					AutoLockOption_init(setting_GetAutoLockLevel_CL());
					break;
				case TYPE_Chl:
					AutoLockOption_init(setting_GetAutoLockLevel_Chl());
					break;
				case TYPE_Bga:
					AutoLockOption_init(setting_GetAutoLockLevel_Bga());
					break;
				case TYPE_CODuv:
					AutoLockOption_init(setting_GetAutoLockLevel_COD());
					break;
				case TYPE_MLSS:
					AutoLockOption_init(setting_GetAutoLockLevel_MLSS());
					break;
				case TYPE_Oiw:
					if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
					{
						AutoLockOption_init(setting_GetAutoLockLevel_OIW());
					}
					else
					{
						AutoLockOption_init(setting_GetAutoLockLevel_OIW_ppm());
					}
					break;
				case TYPE_TSS:
					AutoLockOption_init(setting_GetAutoLockLevel_TSS());
					break;
				case TYPE_SAL:
					AutoLockOption_init(setting_GetAutoLockLevel_SAL());
					break;
				case TYPE_TDS:
					AutoLockOption_init(setting_GetAutoLockLevel_TDS());
					break;
				
				default:
					break;
			}
			break;
			
			/*滑动平均相关界面*/
			case PAGE_3_SLIDEAVG_TYPE://滑动平均
				generate_SlideAverage_type(&cur_interfacial);
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				CurOption_init();
			break;
			
			case PAGE_4_SLIDEAVG:
				generate_SlideAverage_Switch(&cur_interfacial);
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				switch(cur_DO.current_sensor_type)
				{
					case TYPE_DO:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_DO());	
					break;
					case TYPE_pH:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_pH());	
						break;
					case TYPE_Tur:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_Tur());	
						break;
					case TYPE_FCL:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_FCL());	
						break;
					case TYPE_EC:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_EC());	
						break;
					case TYPE_ORP:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_ORP());	
						break;
					case TYPE_NH4:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_NH4());	
						break;
					case TYPE_F:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_F());	
						break;
					case TYPE_CL:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_CL());	
						break;
					case TYPE_Chl:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_Chl());	
						break;
					case TYPE_Bga:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_Bga());	
						break;
					case TYPE_CODuv:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_COD());	
						break;
					case TYPE_MLSS:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_MLSS());	
						break;
					case TYPE_Oiw:
						if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
						{
							BinaryOption_init(setting_GetIsOpen_SlideAvg_OIW());	
						}
						else
						{
							BinaryOption_init(setting_GetIsOpen_SlideAvg_OIW_ppm());	
						}
						break;
					case TYPE_TSS:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_TSS());
						break;
					case TYPE_SAL:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_SAL());
						break;
					case TYPE_TDS:
						BinaryOption_init(setting_GetIsOpen_SlideAvg_TDS());
						break;
					
					default:
						break;
				}
			break;
			
			case PAGE_4_SENSOR_SLIDEAVG:
				generate_SlideAverageA_Sensor_Value(&cur_interfacial, get_CurDo()->DC17_Mes_Time.value_f);
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				CurOption_init();		
				break;
				
			case PAGE_5_SLIDEVALUE:
				switch(cur_DO.current_sensor_type)
				{
					case TYPE_DO:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_DO());
					break;
					case TYPE_pH:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_pH());
						break;
					case TYPE_Tur:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_Tur());
						break;
					case TYPE_FCL:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_FCL());
						break;
					case TYPE_EC:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_EC());
						break;
					case TYPE_ORP:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_ORP());
						break;
					case TYPE_NH4:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_NH4());
						break;
					case TYPE_F:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_F());
						break;
					case TYPE_CL:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_CL());
						break;
					case TYPE_Chl:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_Chl());
						break;
					case TYPE_Bga:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_Bga());
						break;
					case TYPE_CODuv:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_COD());
						break;
					case TYPE_MLSS:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_MLSS());
						break;
					case TYPE_Oiw:
						if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
						{
							generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_OIW());
						}
						else
						{
							generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_OIW_ppm());
						}
						break;
					case TYPE_TSS:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_TSS());
						break;
					case TYPE_SAL:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_SAL());
						break;
					case TYPE_TDS:
						generate_SlideAverage_Value(&cur_interfacial, setting_GetSlideAvgTimes_TDS());
						break;
					default:
						break;
				}
				btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
				btnOkEscFunc_register(btn_OkEscMode_ChangePage);
				CurOption_init();				
				break;
				
				
		case PAGE_5_NH3N_ONE://NH3N  第一点校准

			temp_SensorType = rs485_GetSensorType();
			generate_NH3N_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_NH3N_TWO://NH3N  第一点校准

			temp_SensorType = rs485_GetSensorType();
			generate_NH3N_TWO(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_NH3N_pH_ONE://NH3N  第一点校准

			temp_SensorType = rs485_GetSensorType();
			generate_NH3N_pH_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_NH3N_pH_TWO://NH3N  第二点校准

			temp_SensorType = rs485_GetSensorType();
			generate_NH3N_pH_TWO(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_NH3N_pH_THREE://NH3N  第三点校准

			temp_SensorType = rs485_GetSensorType();
			generate_NH3N_pH_THREE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;

		case PAGE_5_COD_shenghui_Zero:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_shenghui_Zero(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
		
			CurOption_init();	
			break;
		
		case PAGE_5_COD_shenghui_Tur_ONE:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_shenghui_Tur_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_COD_shenghui_Tur_TWO:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_shenghui_Tur_TWO(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
    case PAGE_5_COD_shenghui_ONE:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_shenghui_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_COD_shenghui_TWO:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_shenghui_TWO(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;			

		case PAGE_5_COD_shenghui_THREE:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_shenghui_THREE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		
		case PAGE_5_COD_DC18_Tur_Zero:
		case PAGE_5_COD_DC17_IN_Tur_Zero:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_DC18_Tur_Zero(&cur_interfacial, temp_SensorType ,page_num);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;			
	
    case PAGE_5_COD_DC18_Tur_Slope:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_DC18_Tur_Slope(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;				
		
		case PAGE_5_COD_DC18_Zero:
		case PAGE_5_COD_DC17_IN_Zero:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_DC18_Zero(&cur_interfacial, temp_SensorType,page_num);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;				
		
		case PAGE_5_COD_DC18_Slope:
			temp_SensorType = rs485_GetSensorType();
			generate_COD_DC18_Slope(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;	
		
		case PAGE_5_COD_DC18_Coefficient:		
			temp_SensorType = rs485_GetSensorType();
			generate_COD_DC18_Coefficient(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;	
		
		case PAGE_5_COD_Cleanse:		
			temp_SensorType = rs485_GetSensorType();
			generate_COD_Cleanse(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeOption);
			btnOkEscFunc_register(btn_OkEscMode_ChangePage);
			CurOption_init();	
			break;	
		
		case PAGE_5_shenghui_Tur_ONE:
			temp_SensorType = rs485_GetSensorType();
			generate_shenghui_Tur_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;

		case PAGE_5_shenghui_Tur_TWO:
			temp_SensorType = rs485_GetSensorType();
			generate_shenghui_Tur_TWO(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;

		case PAGE_5_shenghui_Tur_THREE:
			temp_SensorType = rs485_GetSensorType();
			generate_shenghui_Tur_THREE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;

		case PAGE_5_shenghui_EC_ONE:
		case PAGE_5_DR31_ORP_ONE:
			temp_SensorType = rs485_GetSensorType();
			generate_shenghui_EC_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;
		case PAGE_5_OIW_K_CAL:
			temp_SensorType = rs485_GetSensorType();
			generate_OIW_K_CAL(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();	
			break;

		case PAGE_5_shenghui_BGA_ONE:
			temp_SensorType = rs485_GetSensorType();
			generate_shenghui_BGA_ONE(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();
			break;
		case PAGE_5_shenghui_BGA_TWO:
			temp_SensorType = rs485_GetSensorType();
			generate_shenghui_BGA_TWO(&cur_interfacial, temp_SensorType);
			btnUpDownFunc_register(btn_UpDownMode_ChangeValue);
			btnOkEscFunc_register(btn_OkEscMode_ChangeOption);
		
			NanoOption_init();
			break;
		
	  	case PAGE_5_DE26_EC_Zero:	
			temp_SensorType = rs485_GetSensorType();
			generate_DE26_EC_Zero(&cur_interfacial, temp_SensorType);
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

void interfacial_SetOptionSensorName(uint8_t* value)
{
	option_sensor_name = value;
}

#define Sersor_Number 28    //支持搜索 传感器的数量
uint8_t CircularSent_Count=1;
uint8_t GetCircularSent_Flag=0;
uint8_t GetCircularSent_Flag_0=0;
uint8_t GetCircularSent_Flag_1=0;
uint8_t TwoCircular_Flag=0;
void rs485_Search_Sensor(void){
  	uint8_t CircularSent_isSelect=0;
  	if(get_COMADo() != NULL  && get_COMBDo() != NULL)	//同时识别到两根传感器
  	{	
		if(setting_Get_Type() == 0)//702
		{
			DO_HaiFa_DY56_rs485_GetValue(get_CurDo());//D702只发海发溶解氧
		}
		else//580
		{
			TwoCircular_Flag=!TwoCircular_Flag;
			if(TwoCircular_Flag)
			{
				switch(get_COMADo()->modbus_id)
				{
					case DO_shenghui_ModbusID:
							if(GetCircularSent_Flag_0 == 0){
								GetCircularSent_Flag_0=1;
								DO_shenghui_rs485_GetDOPercent(get_CurDo());
							}else{
								GetCircularSent_Flag_0=0;
								DO_shenghui_rs485_GetTempTwoDO(get_CurDo())	;	
							}
						break;

					case DO_HF1012_ModbusID:
						DO_rs485_GetTempTwoDO(get_CurDo());
						break;
					
					case DO_HF_DY56_ModbusID:
						DO_HaiFa_DY56_rs485_GetValue(get_CurDo());
						break;
					
					case NH3N_DN02_ModbusID:
						NH3N_DN02_rs485_GetValue(get_CurDo());	
						break;
					
					case NH3N_shenghui_ModbusID:
						NH3N_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case COD_DC18_ModbusID:
						COD_DC18_rs485_GetValue(get_CurDo(),COD_DC18_ModbusID);		
						break;

					case COD_DC17_ModbusID:
						COD_DC18_rs485_GetValue(get_CurDo(),COD_DC17_ModbusID);	
						break;
					
					case COD_shenghui_ModbusID:
						COD_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case pH_shenghui_ModbusID:
						pH_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case Tur_shenghui_ModbusID:
						Tur_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case EC_shenghui_ModbusID:
						EC_shenghui_rs485_GetValue(get_CurDo());
						break;

					case EC_DE26_ModbusID:
						EC_DE26_rs485_GetValue(get_CurDo());
						break;
					
					case EC_DE40_ModbusID:
						EC_DE40_rs485_GetValue(get_CurDo(),EC_DE40_ModbusID);
						break;

					case EC_DS46_ModbusID:
						EC_DE40_rs485_GetValue(get_CurDo(),EC_DS46_ModbusID);
						break;
					
					case pH_DpH07_ModbusID:
						pH_DpH07_rs485_GetValue(get_CurDo());
						break;
					
					case Chl_shenghui_ModbusID:
							if(GetCircularSent_Flag_1 == 0){
								GetCircularSent_Flag_1=1;
								Chl_shenghui_rs485_GetVol(get_CurDo());
							}else{
								GetCircularSent_Flag_1=0;
								Chl_shenghui_rs485_GetValue(get_CurDo())	;	
							}
						break;

					case ORP_DR31_ModbusID:
						ORP_DR31_rs485_GetValue(get_CurDo());
						break;

					case Bga_shenghui_ModbusID:
						BGA_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case MLSS_Tianjian_ModbusID:
						MLSS_Tianjian_rs485_GetValue(get_CurDo());
						break;

					case OiW_guohong_ModbusID:
						OiW_Guohong_rs485_GetValue(get_CurDo());
						break;
					
					case OiW_yushan_ModbusID:
						OiW_yushan_rs485_GetValue(get_CurDo(),OiW_yushan_ModbusID);
						break;
					case OiW_yushan_DA511_ModbusID:
						OiW_yushan_rs485_GetValue(get_CurDo(),OiW_yushan_DA511_ModbusID);
						break;
					case DO_HF_DY12_ModbusID:
						DO_HaiFa_DY12_rs485_GetValue(get_CurDo());
						break;

					case LH_DX01_ModbusID:
						DX01_rs485_GetValue(get_CurDo());
						break;

					case ZS_DZ09_ModbusID:
						DZ09_rs485_GetValue(get_CurDo());
						break;

					case TDS_DT49_Modbus:
						TDS_DT49_rs485_GetValue();
						break;
					
					case Cl_DL312_ModbusID:
						DL312_rs485_GetValue(get_CurDo());
						break;
					
					case DO_DY05_ModbusID:
						DY05_rs485_GetValue(get_CurDo());
						break;
					
					case DO_DO59_ModbusID:
						DO59_rs485_GetValue(get_CurDo());
						break;
					
					default:
						break;
				}  		
			}
			else
			{
				switch(get_COMBDo()->modbus_id)
				{
					case DO_shenghui_ModbusID:
							if(GetCircularSent_Flag_0 == 0){
								GetCircularSent_Flag_0=1;
								DO_shenghui_rs485_GetDOPercent(get_CurDo());
							}else{
								GetCircularSent_Flag_0=0;
								DO_shenghui_rs485_GetTempTwoDO(get_CurDo())	;	
							}
						break;
							
					case DO_HF1012_ModbusID:
						DO_rs485_GetTempTwoDO(get_CurDo());
						break;
					
					case DO_HF_DY56_ModbusID:
						DO_HaiFa_DY56_rs485_GetValue(get_CurDo());
						break;
					
					case NH3N_DN02_ModbusID:
						NH3N_DN02_rs485_GetValue(get_CurDo());	
						break;
					
					case NH3N_shenghui_ModbusID:
						NH3N_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case COD_DC18_ModbusID:
						COD_DC18_rs485_GetValue(get_CurDo(),COD_DC18_ModbusID);		
						break;

					case COD_DC17_ModbusID:
						COD_DC18_rs485_GetValue(get_CurDo(),COD_DC17_ModbusID);	
						break;
					
					case COD_shenghui_ModbusID:
						COD_shenghui_rs485_GetValue(get_CurDo());
						break;

					case pH_shenghui_ModbusID:
						pH_shenghui_rs485_GetValue(get_CurDo());
						break;

					case Tur_shenghui_ModbusID:
						Tur_shenghui_rs485_GetValue(get_CurDo());
						break;

					case EC_shenghui_ModbusID:
						EC_shenghui_rs485_GetValue(get_CurDo());
						break;

					case EC_DE26_ModbusID:
						EC_DE26_rs485_GetValue(get_CurDo());
						break;

					case EC_DE40_ModbusID:
						EC_DE40_rs485_GetValue(get_CurDo(),EC_DE40_ModbusID);
						break;

					case EC_DS46_ModbusID:
						EC_DE40_rs485_GetValue(get_CurDo(),EC_DS46_ModbusID);
						break;
					
					case pH_DpH07_ModbusID:
						pH_DpH07_rs485_GetValue(get_CurDo());
						break;
					
					case Chl_shenghui_ModbusID:
							if(GetCircularSent_Flag_1 == 0){
								GetCircularSent_Flag_1=1;
								Chl_shenghui_rs485_GetVol(get_CurDo());
							}else{
								GetCircularSent_Flag_1=0;
								Chl_shenghui_rs485_GetValue(get_CurDo())	;	
							}
						break;

					case ORP_DR31_ModbusID:
						ORP_DR31_rs485_GetValue(get_CurDo());
						break;
							
					case Bga_shenghui_ModbusID:
						BGA_shenghui_rs485_GetValue(get_CurDo());
						break;

					case MLSS_Tianjian_ModbusID:
						MLSS_Tianjian_rs485_GetValue(get_CurDo());
						break;

					case OiW_guohong_ModbusID:
						OiW_Guohong_rs485_GetValue(get_CurDo());
						break;

					case OiW_yushan_ModbusID:
						OiW_yushan_rs485_GetValue(get_CurDo(),OiW_yushan_ModbusID);
						break;
					case OiW_yushan_DA511_ModbusID:
						OiW_yushan_rs485_GetValue(get_CurDo(),OiW_yushan_DA511_ModbusID);
						break;

					case DO_HF_DY12_ModbusID:
						DO_HaiFa_DY12_rs485_GetValue(get_CurDo());
						break;

					case LH_DX01_ModbusID:
						DX01_rs485_GetValue(get_CurDo());
						break;

					case ZS_DZ09_ModbusID:
						DZ09_rs485_GetValue(get_CurDo());
						break;

					case TDS_DT49_Modbus:
						TDS_DT49_rs485_GetValue();
						break;

					case Cl_DL312_ModbusID:
						DL312_rs485_GetValue(get_CurDo());
						break;	

					case DO_DY05_ModbusID:
						DY05_rs485_GetValue(get_CurDo());
						break;

					case DO_DO59_ModbusID:
						DO59_rs485_GetValue(get_CurDo());
						break;
					
					default:
						break;
				}  		
			}
		}		
  	}
	else if(get_COMADo() != NULL  && get_COMBDo() == NULL)
	{   //只识别到一根传感器
		if(setting_Get_Type() == 0)//702
		{
			DO_HaiFa_DY56_rs485_GetValue(get_CurDo());//D702只发海发溶解氧
		}
		else//580
		{
			TwoCircular_Flag=!TwoCircular_Flag;	
			if(TwoCircular_Flag)
			{
				switch(get_COMADo()->modbus_id)
				{
					case DO_shenghui_ModbusID:
						if(GetCircularSent_Flag_0 == 0)
						{
							GetCircularSent_Flag_0=1;
							DO_shenghui_rs485_GetDOPercent(get_CurDo());
						}
						else
						{
							GetCircularSent_Flag_0=0;
							DO_shenghui_rs485_GetTempTwoDO(get_CurDo())	;	
						}
					break;
							
					case DO_HF1012_ModbusID:
						DO_rs485_GetTempTwoDO(get_CurDo());
						break;
					
					case DO_HF_DY56_ModbusID:
						DO_HaiFa_DY56_rs485_GetValue(get_CurDo());
						break;
					
					case NH3N_DN02_ModbusID:
						NH3N_DN02_rs485_GetValue(get_CurDo());	
						break;
					
					case NH3N_shenghui_ModbusID:
						NH3N_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case COD_DC18_ModbusID:
						COD_DC18_rs485_GetValue(get_CurDo(),COD_DC18_ModbusID);		
						break;

					case COD_DC17_ModbusID:
						COD_DC18_rs485_GetValue(get_CurDo(),COD_DC17_ModbusID);	
						break;
					
					case COD_shenghui_ModbusID:
						COD_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case pH_shenghui_ModbusID:
						pH_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case Tur_shenghui_ModbusID:
						Tur_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case EC_shenghui_ModbusID:
						EC_shenghui_rs485_GetValue(get_CurDo());
						break;
					
					case EC_DE26_ModbusID:
						EC_DE26_rs485_GetValue(get_CurDo());
						break;

					case EC_DE40_ModbusID:
						EC_DE40_rs485_GetValue(get_CurDo(),EC_DE40_ModbusID);
						break;

					case EC_DS46_ModbusID:
						EC_DE40_rs485_GetValue(get_CurDo(),EC_DS46_ModbusID);
						break;
					
					case pH_DpH07_ModbusID:
						pH_DpH07_rs485_GetValue(get_CurDo());
						break;
					
					case Chl_shenghui_ModbusID:
						if(GetCircularSent_Flag_1 == 0)
						{
							GetCircularSent_Flag_1=1;
							Chl_shenghui_rs485_GetVol(get_CurDo());
						}
						else
						{
							GetCircularSent_Flag_1=0;
							Chl_shenghui_rs485_GetValue(get_CurDo());	
						}
						break;

					case ORP_DR31_ModbusID:
						ORP_DR31_rs485_GetValue(get_CurDo());
						break;

					case Bga_shenghui_ModbusID:
						BGA_shenghui_rs485_GetValue(get_CurDo());
						break;
							
					case MLSS_Tianjian_ModbusID:
						MLSS_Tianjian_rs485_GetValue(get_CurDo());
						break;

					case OiW_guohong_ModbusID:
						OiW_Guohong_rs485_GetValue(get_CurDo());
						break;

					case OiW_yushan_ModbusID:
						OiW_yushan_rs485_GetValue(get_CurDo(),OiW_yushan_ModbusID);
						break;
					case OiW_yushan_DA511_ModbusID:
						OiW_yushan_rs485_GetValue(get_CurDo(),OiW_yushan_DA511_ModbusID);
						break;

					case DO_HF_DY12_ModbusID:
						DO_HaiFa_DY12_rs485_GetValue(get_CurDo());
						break;

					case LH_DX01_ModbusID:
						DX01_rs485_GetValue(get_CurDo());
						break;

					case ZS_DZ09_ModbusID:
						DZ09_rs485_GetValue(get_CurDo());
						break;
						
					case TDS_DT49_Modbus:
						TDS_DT49_rs485_GetValue();
						break;
					
					case Cl_DL312_ModbusID:
						DL312_rs485_GetValue(get_CurDo());
						break;

					case DO_DY05_ModbusID:
						DY05_rs485_GetValue(get_CurDo());
						break;

					case DO_DO59_ModbusID:
						DO59_rs485_GetValue(get_CurDo());
						break;
					
					default:
						break;
				} 		
			}	
			else
			{
				while(!CircularSent_isSelect)
				{
					CircularSent_Count++;
					if( CircularSent_Count ==1 )
					{
						if(get_COMADo()->modbus_id != DO_HF_DY56_ModbusID )
						{	
							CircularSent_isSelect=1;
							DO_HaiFa_DY56_rs485_GetModbusId();
						}
					}
					
					
					if( CircularSent_Count ==2){
						if(get_COMADo()->modbus_id != DO_shenghui_ModbusID){			
							CircularSent_isSelect=1;
							DO_shenghui_rs485_GetModbusId();
						}			
					}		
					
					if(CircularSent_Count ==3){

						if(get_COMADo()->modbus_id != NH3N_DN02_ModbusID){
						CircularSent_isSelect=1;		
							NH3N_DN02_rs485_GetModbusId();
						}			
					}	

					if( CircularSent_Count ==4){

						if(get_COMADo()->modbus_id != NH3N_shenghui_ModbusID){
						CircularSent_isSelect=1;			
							NH3N_shenghui_rs485_GetModbusId();
						}			
					}	
					
					if( CircularSent_Count ==5){

						if(get_COMADo()->modbus_id != COD_DC18_ModbusID){
						CircularSent_isSelect=1;		
							COD_DC18_rs485_GetModbusId(COD_DC18_ModbusID);
						}		
						
					}	
					
					if( CircularSent_Count ==6){

						if(get_COMADo()->modbus_id != COD_shenghui_ModbusID ){
						CircularSent_isSelect=1;			
							COD_shenghui_rs485_GetModbusId();
						}					
					}	

					if( CircularSent_Count ==7){

						if(get_COMADo()->modbus_id != pH_shenghui_ModbusID ){
						CircularSent_isSelect=1;			
							pH_shenghui_rs485_GetModbusId();
						}					
					}	

					if( CircularSent_Count ==8){

						if(get_COMADo()->modbus_id != Tur_shenghui_ModbusID ){
						CircularSent_isSelect=1;			
							Tur_shenghui_rs485_GetModbusId();
						}					
					}	

					if( CircularSent_Count ==9){

						if(get_COMADo()->modbus_id != EC_shenghui_ModbusID ){
						CircularSent_isSelect=1;			
							EC_shenghui_rs485_GetModbusId();
						}					
					}	
					
					if( CircularSent_Count ==10){

						if(get_COMADo()->modbus_id != EC_DE26_ModbusID ){
						CircularSent_isSelect=1;			
							EC_DE26_rs485_GetModbusId();
						}					
					}	
					
					if( CircularSent_Count ==11){

						if(get_COMADo()->modbus_id != pH_DpH07_ModbusID ){
						CircularSent_isSelect=1;			
							pH_DpH07_rs485_GetModbusId();
						}					
					}	
					
					
					if( CircularSent_Count ==12){

						if(get_COMADo()->modbus_id != Chl_shenghui_ModbusID ){
						CircularSent_isSelect=1;			
							Chl_shenghui_rs485_GetModbusId();
						}					
					}

					if( CircularSent_Count ==13){

						if(get_COMADo()->modbus_id != ORP_DR31_ModbusID ){
						CircularSent_isSelect=1;			
							ORP_DR31_rs485_GetModbusId();
						}					
					}

					if( CircularSent_Count ==14){

						if(get_COMADo()->modbus_id != Bga_shenghui_ModbusID ){
						CircularSent_isSelect=1;			
							BGA_shenghui_rs485_GetModbusId();
						}					
					}

					if( CircularSent_Count ==15){

						if(get_COMADo()->modbus_id != COD_DC17_ModbusID ){
						CircularSent_isSelect=1;			
							COD_DC18_rs485_GetModbusId(COD_DC17_ModbusID);
						}					
					}
					
					if( CircularSent_Count ==16){

						if(get_COMADo()->modbus_id != MLSS_Tianjian_ModbusID ){
						CircularSent_isSelect=1;			
							MLSS_Tianjian_rs485_GetModbusId();
						}					
					}

					if( CircularSent_Count ==17){

						if(get_COMADo()->modbus_id != OiW_guohong_ModbusID ){
						CircularSent_isSelect=1;			
							OiW_Guohong_Get_Modbus_ID();
						}					
					}

					if( CircularSent_Count ==18){

						if(get_COMADo()->modbus_id != OiW_yushan_ModbusID ){
						CircularSent_isSelect=1;			
							OiW_yushan_rs485_GetModbusId(OiW_yushan_ModbusID);
						}					
					}

					if( CircularSent_Count ==19){
						if(get_COMADo()->modbus_id != DO_HF_DY12_ModbusID ){
						CircularSent_isSelect=1;			
							DO_HaiFa_DY12_rs485_GetModbusId();
						}
					}

					if( CircularSent_Count ==20){
						if(get_COMADo()->modbus_id != LH_DX01_ModbusID ){
						CircularSent_isSelect=1;			
							DX01_rs485_GetModbusId();
						}
					}

					if( CircularSent_Count ==21){
						if(get_COMADo()->modbus_id != EC_DE40_ModbusID ){
						CircularSent_isSelect=1;			
							EC_DE40_rs485_GetModbusId(EC_DE40_ModbusID);
						}
					}

					if( CircularSent_Count ==22){
						if(get_COMADo()->modbus_id != EC_DS46_ModbusID ){
						CircularSent_isSelect=1;			
							EC_DE40_rs485_GetModbusId(EC_DS46_ModbusID);
						}
					}

					if( CircularSent_Count ==23){

						if(get_COMADo()->modbus_id != OiW_yushan_DA511_ModbusID ){
						CircularSent_isSelect=1;			
							OiW_yushan_rs485_GetModbusId(OiW_yushan_DA511_ModbusID);
						}					
					}

					if( CircularSent_Count ==24){

						if(get_COMADo()->modbus_id != ZS_DZ09_ModbusID ){
						CircularSent_isSelect=1;			
							if(Get_TUR_Connect_FLAG() == 0)
							{
								DZ09_rs485_GetModbusId();
							}
						}					
					}

					if( CircularSent_Count ==25){

						if(get_COMADo()->modbus_id != TDS_DT49_Modbus ){
						CircularSent_isSelect=1;			
							TDS_DT49_rs485_GetModbusId();
						}					
					}
					
					if( CircularSent_Count ==26){

						if(get_COMADo()->modbus_id != Cl_DL312_ModbusID ){
						  CircularSent_isSelect=1;			
							DL312_rs485_GetModbusId();
						}					
					}

					if( CircularSent_Count ==27){

						if(get_COMADo()->modbus_id != DO_DY05_ModbusID ){
						  CircularSent_isSelect=1;			
							DY05_rs485_GetModbusId();
						}					
					}

					if( CircularSent_Count ==28){

						if(get_COMADo()->modbus_id != DO_DO59_ModbusID ){
						  CircularSent_isSelect=1;			
							DO59_rs485_GetModbusId();
						}					
					}
					
					if(CircularSent_Count == Sersor_Number){
						CircularSent_Count=0;
					}				
				}		
			}
		}
	}
	else
	{                                                 //两根传感器都没接
		if(setting_Get_Type() == 0)//702
		{
			DO_HaiFa_DY56_rs485_GetModbusId();
		}
		else
		{
			while(!CircularSent_isSelect)
			{
				CircularSent_Count++;
				if( CircularSent_Count ==1 ){
					CircularSent_isSelect=1;			
					DO_HaiFa_DY56_rs485_GetModbusId();
				}
				if( CircularSent_Count ==2){
					CircularSent_isSelect=1;		
					DO_shenghui_rs485_GetModbusId();			
				}		
				
				if(CircularSent_Count ==3){
					CircularSent_isSelect=1;	
					NH3N_DN02_rs485_GetModbusId();
				}	

				if( CircularSent_Count ==4){
					CircularSent_isSelect=1;				
					NH3N_shenghui_rs485_GetModbusId();	
				}	
				
				if( CircularSent_Count ==5){
					CircularSent_isSelect=1;			
					COD_DC18_rs485_GetModbusId(COD_DC18_ModbusID);			
				}	
				
				if( CircularSent_Count ==6){
					CircularSent_isSelect=1;		
					COD_shenghui_rs485_GetModbusId();						
				}	
				
				if( CircularSent_Count ==7){
					CircularSent_isSelect=1;		
					pH_shenghui_rs485_GetModbusId();						
				}
				
				if( CircularSent_Count ==8){
					CircularSent_isSelect=1;		
					Tur_shenghui_rs485_GetModbusId();						
				}
				
				if( CircularSent_Count ==9){
					CircularSent_isSelect=1;		
					EC_shenghui_rs485_GetModbusId();						
				}
				
				if( CircularSent_Count ==10){
					CircularSent_isSelect=1;		
					EC_DE26_rs485_GetModbusId();						
				}
				
				if( CircularSent_Count ==11){
					CircularSent_isSelect=1;		
					pH_DpH07_rs485_GetModbusId();						
				}

				if( CircularSent_Count ==12){
					CircularSent_isSelect=1;		
					Chl_shenghui_rs485_GetModbusId();						
				}

				if( CircularSent_Count ==13){
					CircularSent_isSelect=1;		
					ORP_DR31_rs485_GetModbusId();						
				}

				if( CircularSent_Count ==14){
					CircularSent_isSelect=1;			
					BGA_shenghui_rs485_GetModbusId();
				}

				if( CircularSent_Count ==15){
					CircularSent_isSelect=1;			
					COD_DC18_rs485_GetModbusId(COD_DC17_ModbusID);			
				}	

				if( CircularSent_Count ==16){
					CircularSent_isSelect=1;			
					MLSS_Tianjian_rs485_GetModbusId();			
				}

				if( CircularSent_Count ==17){
					CircularSent_isSelect=1;			
					OiW_Guohong_Get_Modbus_ID();
				}

				if( CircularSent_Count ==18){
					CircularSent_isSelect=1;			
					OiW_yushan_rs485_GetModbusId(OiW_yushan_ModbusID);
				}

				if( CircularSent_Count ==19){
					CircularSent_isSelect=1;			
					DO_HaiFa_DY12_rs485_GetModbusId();
				}

				if( CircularSent_Count ==20){
					CircularSent_isSelect=1;			
					DX01_rs485_GetModbusId();
				}
				
				if( CircularSent_Count ==21){
					CircularSent_isSelect=1;		
					EC_DE40_rs485_GetModbusId(EC_DE40_ModbusID);						
				}

				if( CircularSent_Count ==22){
					CircularSent_isSelect=1;		
					EC_DE40_rs485_GetModbusId(EC_DS46_ModbusID);						
				}

				if( CircularSent_Count ==23){
					CircularSent_isSelect=1;			
					OiW_yushan_rs485_GetModbusId(OiW_yushan_DA511_ModbusID);
				}

				if( CircularSent_Count ==24){
					CircularSent_isSelect=1;			
					DZ09_rs485_GetModbusId();			
				}	
				
				if(CircularSent_Count == 25)
				{
					CircularSent_isSelect=1;	
					TDS_DT49_rs485_GetModbusId();
				}
				
				if(CircularSent_Count == 26)
				{
					CircularSent_isSelect=1;	
					DL312_rs485_GetModbusId();
				}

				if(CircularSent_Count == 27)
				{
					CircularSent_isSelect=1;	
					DY05_rs485_GetModbusId();
				}

				if(CircularSent_Count == 28)
				{
					CircularSent_isSelect=1;	
					DO59_rs485_GetModbusId();
				}
				
				if(CircularSent_Count == Sersor_Number){
					CircularSent_Count=0;
				}				
			}
		}
	}
}




static uint8_t location = 0;
void clear_NoSignal(void)
{
	gui_ClearNoSignal(setting_GetIsChn(), 36+location*38);
}



uint16_t Auto_Search_Count=0;
void no_signal(void)
{
	if(Auto_Search_Count == Auto_Search_Time-1){
		interfacial_ClearLabel();                                  //清所有的数据标签
		LabelList_Add((setting_GetIsChn() ? 48: 44), 36, (uint8_t *)wuxinghao_cn, sizeof(wuxinghao_cn), (uint8_t *)wuxinghao_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(cur_interfacial.label_head));		
	}
	if(cur_interfacial.label_head == NULL)
	{
		location = 0;
		if(Auto_Search_Count < Auto_Search_Time){
		  LabelList_Add((setting_GetIsChn() ? 54 : 44), 36, (uint8_t *)zidongsousuozhong_cn, sizeof(zidongsousuozhong_cn), (uint8_t *)zidongsousuozhong_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(cur_interfacial.label_head));				
		}else{
		  LabelList_Add((setting_GetIsChn() ? 48 : 44), 36, (uint8_t *)wuxinghao_cn, sizeof(wuxinghao_cn), (uint8_t *)wuxinghao_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(cur_interfacial.label_head));		
		}

	}
	else
	{
		clear_NoSignal();
		value_PlusPlus(&location, 0, 2);
		cur_interfacial.label_head->y = 36+location*38;
	}
	set_StartPoint(0);
}


/*通过一个全局的界面指针变量来设置刷新界面*/
void interfacial_refresh(void)                                                            //刷新界面 包括生成状态栏和设置界面
{
	uint8_t temp_warning = 0;
	uint8_t Enable_warning = 0;
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

		switch(interfacial_GetCurPage())
		{
			case PAGE_0_START:
			if(get_CurDo() != NULL )
			{
				
				if(rs485_GetIsChangeSenesor())//第一次是溶解氧的话添加一下三个参数标签
				{
					rs485_ClearIsChangeSenesor();
					interfacial_ClearLabel();	  //防止堆栈溢出
					gui_ClearLines(26, 130, 0);   //确保"无传感器"字样清除干净
					list_label label_head = NULL; //溶解氧数据label列表
					switch(rs485_GetSensorType())
					{
						case TYPE_DO:
							
							switch(get_CurDo()->modbus_id)
							{
								case DO_shenghui_ModbusID:  
									LabelList_Add( 0, 48, (uint8_t *)baohelv_cn, sizeof(baohelv_cn), (uint8_t *)baohelv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//饱和率
									LabelList_Add( 48,  36, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,   LABEL_LARGE,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //do %
								
								  	LabelList_Add( 0, 84, (uint8_t *)rongyang_cn, sizeof(rongyang_cn), (uint8_t *)rongyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//溶氧						
									LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //do mg/L
								
						    		LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature											
									LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
									
									LabelList_Add( 152, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//%
									LabelList_Add( 128, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L
									if(setting_Get_Temp_Unit())
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
									}
									else
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
									}
																					
									break;	
															
								default:								
									LabelList_Add( 0, 48, (uint8_t *)baohedu_cn, sizeof(baohedu_cn), (uint8_t *)baohedu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//饱和度
									LabelList_Add( 48,  36, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,   LABEL_LARGE,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //do %
								
								  	LabelList_Add( 0, 84, (uint8_t *)rongyang_cn, sizeof(rongyang_cn), (uint8_t *)rongyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//溶氧						
									LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //do mg/L
								
						    		LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature											
									LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
									
									LabelList_Add( 152, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//%
									LabelList_Add( 128, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L
									if(setting_Get_Temp_Unit())
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
									}
									else
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
									}	
									break;
							}
				                    
							break;
						
						case TYPE_pH:						
							LabelList_Add( 0, 66, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//pH
							LabelList_Add( 32,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //pH
	
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							if(setting_Get_Temp_Unit())
	            			{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
						  	}
						  	else
              				{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
						  	}

							break;
						
						case TYPE_Tur:
							switch(get_CurDo()->modbus_id)
							{
								case Tur_shenghui_ModbusID: 
									LabelList_Add( 0, 82, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
									LabelList_Add( 32,  70, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //浊度						
									LabelList_Add( 128, 84, (uint8_t *)NTU_cn, sizeof(NTU_cn), (uint8_t *)NTU_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//NTU
									break;
								case ZS_DZ09_ModbusID:
									LabelList_Add( 0, 66, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
									LabelList_Add( 32,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //浊度						
									LabelList_Add( 128, 68, (uint8_t *)NTU_cn, sizeof(NTU_cn), (uint8_t *)NTU_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//NTU
									
									LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
									LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
									
									if(setting_Get_Temp_Unit())
									{
									LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
									}
									else
									{
									LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
									}
									break;
							}
							break;
						
						case TYPE_FCL:

							break;
						
						case TYPE_EC:									
							switch(get_CurDo()->modbus_id)
							{
								case EC_DE26_ModbusID:  	
									LabelList_Add( 0, 48, (uint8_t *)diandao_cn, sizeof(diandao_cn), (uint8_t *)diandaolv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 62,  48, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

									LabelList_Add( 0, 69, (uint8_t *)TDS_cn, sizeof(TDS_cn), (uint8_t *)TDS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 62,  69, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

									LabelList_Add( 0, 90, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_zhu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 62,  90, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

									// if(setting_GetSal_Uni() == 0)
									// {
										LabelList_Add( 120, 92, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, DONT_HAVE_PARENTHESIS, &label_head);//ppt									
									// }
									// else if(setting_GetSal_Uni() == 5)
									// {
									// 	LabelList_Add( 120, 92, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//ppt									
									// }
									// else if(setting_GetSal_Uni() == 10)
									// {
									// 	LabelList_Add( 120, 92, (uint8_t *)qianfenhao1_cn, sizeof(qianfenhao1_cn), (uint8_t *)qianfenhao_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									// }

									LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
									LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
									
									if(get_CurDo()->DOmgl.value_f >= 10000)
									{
                  						LabelList_Add( 120, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_mSCM, DONT_HAVE_PARENTHESIS, &label_head);//ms/cm										
										LabelList_Add( 120, 71, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_GL, DONT_HAVE_PARENTHESIS, &label_head);//g/l									
									}
									else
									{
                  						LabelList_Add( 120, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_USCM, DONT_HAVE_PARENTHESIS, &label_head);//us/cm										
										LabelList_Add( 120, 71, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/l							
									}

									if(setting_Get_Temp_Unit())
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
									}
									else
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
									}
									break;							
								case EC_shenghui_ModbusID:								
									LabelList_Add( 0, 66, (uint8_t *)diandao_cn, sizeof(diandao_cn), (uint8_t *)diandaolv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 32,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC
			
									LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
									LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
									
									if(get_CurDo()->DOmgl.value_f >= 10000)
									{
                  						LabelList_Add( 120, 68, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_mSCM, DONT_HAVE_PARENTHESIS, &label_head);//ms/cm										
									}
									else
									{
                  						LabelList_Add( 120, 68, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_USCM, DONT_HAVE_PARENTHESIS, &label_head);//us/cm										
									}

									if(setting_Get_Temp_Unit())
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
									}
									else
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
									}
									break;

								case EC_DE40_ModbusID: 
									LabelList_Add( 0, 48, (uint8_t *)diandao_cn, sizeof(diandao_cn), (uint8_t *)diandaolv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 62,  48, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

									LabelList_Add( 0, 69, (uint8_t *)TDS_cn, sizeof(TDS_cn), (uint8_t *)TDS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 62,  69, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

									LabelList_Add( 0, 90, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_zhu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
									LabelList_Add( 62,  90, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

									LabelList_Add( 120, 92, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, DONT_HAVE_PARENTHESIS, &label_head);//ppt									
									LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
									LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
									
									LabelList_Add( 120, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_mSCM, DONT_HAVE_PARENTHESIS, &label_head);//ms/cm										
									LabelList_Add( 120, 71, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_GL, DONT_HAVE_PARENTHESIS, &label_head);//g/l									
									if(setting_Get_Temp_Unit())
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
									}
									else
									{
										LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
									}
									break;
								
								default:
									break;
							}

						
							break;
						
						case TYPE_SAL:
							switch (get_CurDo()->modbus_id)
							{
							case EC_DS46_ModbusID:
								LabelList_Add( 0, 66, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_zhu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
								LabelList_Add( 32,  56, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

								if(get_CurDo()->Measure_Range.value_f == 0)
								{
								LabelList_Add( 128, 65, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, DONT_HAVE_PARENTHESIS, &label_head);//ppt									
								}
								else if(get_CurDo()->Measure_Range.value_f == 1)
								{
								LabelList_Add( 128, 65, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//ppt									
								}			
								LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
								LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
								
								if(setting_Get_Temp_Unit())
								{
									LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
								}
								else
								{
									LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
								}
								break;
							default:
								break;
							}
							break;

						case TYPE_ORP:
							switch(get_CurDo()->modbus_id)
							{
								case ORP_DR31_ModbusID:
									LabelList_Add( 0, 84, (uint8_t *)ORP_cn, sizeof(ORP_cn), (uint8_t *)ORP_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//ORP
									LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//ORP_value
									LabelList_Add( 128, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MV, DONT_HAVE_PARENTHESIS, &label_head);//mv	
									break;
							}							
							break;
						
						case TYPE_NH4:
							LabelList_Add( 0, 48, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//pH
							LabelList_Add( 32,  36, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,   LABEL_LARGE,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //pH
							
							LabelList_Add( 0, 84, (uint8_t *)andan_cn, sizeof(andan_cn), (uint8_t *)andan_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//NH4N
							LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //NH4N
	
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							

						  	LabelList_Add( 128, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L
							if(setting_Get_Temp_Unit())
	            			{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
						  	}
						  	else
              				{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
						  	}
						

							break;
						
						case TYPE_F:

							break;
						
						case TYPE_CL:
							LabelList_Add( 0, 66, (uint8_t *)cl_lvlizi_cn, sizeof(cl_lvlizi_cn), (uint8_t *)cl_lvlizi_en,  LABEL_NORMAL, LABEL_xinziku, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
							LabelList_Add( 54,  66, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

							LabelList_Add( 128, 66, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);									

							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							if(setting_Get_Temp_Unit())
							{
								LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
							}
							else
							{
								LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
							}
							break;
						
						case TYPE_Chl:
							LabelList_Add( 0, 66, (uint8_t *)yelvsu_cn, sizeof(yelvsu_cn), (uint8_t *)yelvsu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//叶绿素名称

							
	
							LabelList_Add( 32,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //值
	
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							LabelList_Add( 120, 68, (uint8_t *)yelvsudanwei_cn, sizeof(yelvsudanwei_cn), (uint8_t *)yelvsudanwei_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//ug/L
							if(setting_Get_Temp_Unit())
	            			{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
						  	}
						  	else
              				{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
						  	}
							break;
						
						case TYPE_Bga:
							LabelList_Add( 0, 66, (uint8_t *)Bga_cn, sizeof(Bga_cn), (uint8_t *)Bga_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//叶绿素名称
							LabelList_Add( 48,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //测量值
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 38, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							LabelList_Add( 88, 88, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_Kcells_mL, DONT_HAVE_PARENTHESIS, &label_head);//Kcells/mL
							LabelList_Add( 96, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃

							break;
						
						case TYPE_CODuv:
							if(get_CurDo()->modbus_id == COD_DC17_ModbusID && get_CurDo()->DC17_Mes_Para.value_f == 2)
							{									
								gui_DrawMes((uint8_t *)celiangfuhao);
							}
							else
							{
								gui_ClearMes();
							}
							if((get_CurDo()->modbus_id == COD_DC18_ModbusID || get_CurDo()->modbus_id == COD_DC17_ModbusID) && (!setting_GetIsKey_DC18_Set() ) )
							{
								LabelList_Add( 0, 48, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//COD
								LabelList_Add( 32,  36, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //COD		
							
								LabelList_Add( 128, 50, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L								

								LabelList_Add( 0, 84, (uint8_t *)TOC_cn, sizeof(TOC_cn), (uint8_t *)TOC_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//COD
								LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->tocmgl_Vol_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //COD		
							
								LabelList_Add( 128, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L								

							}
							else
							{
								LabelList_Add( 0, 48, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
								LabelList_Add( 32,  36, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,   LABEL_LARGE,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //浊度
								
								LabelList_Add( 0, 84, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//COD
								LabelList_Add( 32,  72, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //COD
								
								LabelList_Add( 128, 50, (uint8_t *)NTU_cn, sizeof(NTU_cn), (uint8_t *)NTU_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//NTU
								LabelList_Add( 128, 86, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
							}
																	
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							if(setting_Get_Temp_Unit())
							{
							LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
							}
							else
							{
							LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
							}

							break;

						case TYPE_MLSS:
							LabelList_Add( 0, 60, (uint8_t *)MLSS_cn, sizeof(MLSS_cn), (uint8_t *)MLSS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//MLSS
							LabelList_Add( 32,  76, NULL, 0, (uint8_t *)get_CurDo()->DOpercent_Vol_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //MLSS
							
							if(setting_GetMesUnit() == 0)
							{
								LabelList_Add( 128, 90, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
							}
							else if(setting_GetMesUnit() == 10)
							{
								LabelList_Add( 128, 90, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPM, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
							}
							else if(setting_GetMesUnit() == 5)
							{
								LabelList_Add( 128, 90, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_UGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
							}
							else
							{
								LabelList_Add( 128, 90, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
							}
							break;

						case TYPE_Oiw:
							LabelList_Add( 0, 66, (uint8_t *)Oiw_cn, sizeof(Oiw_cn), (uint8_t *)Oiw_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//叶绿素名称

							LabelList_Add( 32,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //值
	
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							if(get_CurDo()->modbus_id == OiW_guohong_ModbusID || get_CurDo()->modbus_id == OiW_yushan_DA511_ModbusID)
							{
								LabelList_Add( 128, 68, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPM, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
							}
							else
							{
								LabelList_Add( 128, 68, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPB, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							

							}
							if(setting_Get_Temp_Unit())
	            			{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
						  	}
						  	else
              				{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
						  	}
							break;
						
						case TYPE_TSS:
							LabelList_Add( 0, 66, (uint8_t *)xuanfuwu_cn, sizeof(xuanfuwu_cn), (uint8_t *)xuanfuwu_en,  LABEL_NORMAL, LABEL_xinziku, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

							LabelList_Add( 32,  54, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							LabelList_Add( 128, 68, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg/L							
	
							if(setting_Get_Temp_Unit())
	            			{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
						  	}
						  	else
              				{
							  LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
						  	}
							break;

						case TYPE_TDS:
							LabelList_Add( 0, 66, (uint8_t *)TDS_cn, sizeof(TDS_cn), (uint8_t *)TDS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
							LabelList_Add( 32,  56, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,       LABEL_LARGE, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //EC

							LabelList_Add( 128, 65, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_GL, DONT_HAVE_PARENTHESIS, &label_head);									

							LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature						
							LabelList_Add( 62, 112, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_MEDIUM, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);  //temperature
							
							if(setting_Get_Temp_Unit())
							{
								LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_FAHRENHEIT, DONT_HAVE_PARENTHESIS, &label_head);//°F								 
							}
							else
							{
								LabelList_Add( 120, 114, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//℃						 
							}
							break;

						default:
							break;
						
				   }					
					
				  cur_interfacial.label_head = label_head;
					
				}
				
					if(DO_GetIsGetedValue(get_CurDo()))//如果有获取到数值的话
					{
						float high,low;
						
						switch(rs485_GetSensorType())
						{
								case TYPE_DO:
									Enable_warning=setting_GetIsAlarm_DO();
									high = float_format(setting_GetHighThreshold_DO());
									low = float_format(setting_GetLowThreshold_DO());					                    
									break;
								
								case TYPE_pH:
									Enable_warning=setting_GetIsAlarm_pH();
							  		high = float_format(setting_GetHighThreshold_pH());
									low = float_format(setting_GetLowThreshold_pH());			
									break;
								
								case TYPE_Tur:
									Enable_warning=setting_GetIsAlarm_Tur();
							  		high = float_format(setting_GetHighThreshold_Tur());
									low = float_format(setting_GetLowThreshold_Tur());		
									break;
								
								case TYPE_FCL:
									Enable_warning=setting_GetIsAlarm_FCL();
							  		high = float_format(setting_GetHighThreshold_FCL());
									low = float_format(setting_GetLowThreshold_FCL());		
									break;
								
								case TYPE_EC:
									Enable_warning=setting_GetIsAlarm_EC();
							  		high = float_format(setting_GetHighThreshold_EC());
									low = float_format(setting_GetLowThreshold_EC());		
									break;
								
								case TYPE_ORP:
									Enable_warning=setting_GetIsAlarm_ORP();
							  		high = float_format(setting_GetHighThreshold_ORP());
									low = float_format(setting_GetLowThreshold_ORP());		
									break;
								
								case TYPE_NH4:
									Enable_warning=setting_GetIsAlarm_NH4();
							  		high = float_format(setting_GetHighThreshold_NH4());
									low = float_format(setting_GetLowThreshold_NH4());							
									break;
								
								case TYPE_F:
									Enable_warning=setting_GetIsAlarm_F();
							  		high = float_format(setting_GetHighThreshold_F());
									low = float_format(setting_GetLowThreshold_F());		
									break;
								
								case TYPE_CL:
									Enable_warning=setting_GetIsAlarm_CL();
							  		high = float_format(setting_GetHighThreshold_CL());
									low = float_format(setting_GetLowThreshold_CL());		
									break;
								
								case TYPE_Chl:
									Enable_warning=setting_GetIsAlarm_Chl();
							  		high = float_format(setting_GetHighThreshold_Chl());
									low = float_format(setting_GetLowThreshold_Chl());		
									break;
								
								case TYPE_Bga:
									Enable_warning=setting_GetIsAlarm_Bga();
							  		high = float_format(setting_GetHighThreshold_Bga());
									low = float_format(setting_GetLowThreshold_Bga());		
									break;
								
								case TYPE_CODuv:
									Enable_warning=setting_GetIsAlarm_COD();
							  		high = float_format(setting_GetHighThreshold_COD());
									low = float_format(setting_GetLowThreshold_COD());		
									break;

								case TYPE_MLSS:
									Enable_warning=setting_GetIsAlarm_MLSS();
							  		high = float_format(setting_GetHighThreshold_MLSS());
									low = float_format(setting_GetLowThreshold_MLSS());		
									break;

								case TYPE_Oiw:
									if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
									{
										Enable_warning=setting_GetIsAlarm_OIW();
										high = float_format(setting_GetHighThreshold_OIW());
										low = float_format(setting_GetLowThreshold_OIW());
									}
									else
									{
										Enable_warning=setting_GetIsAlarm_OIW_ppm();
										high = float_format(setting_GetHighThreshold_OIW_ppm());
										low = float_format(setting_GetLowThreshold_OIW_ppm());
									}	
									break;
								case TYPE_TSS:
									Enable_warning=setting_GetIsAlarm_TSS();
							  		high = float_format(setting_GetHighThreshold_TSS());
									low = float_format(setting_GetLowThreshold_TSS());		
									break;
								case TYPE_SAL:
									Enable_warning=setting_GetIsAlarm_SAL();
							  		high = float_format(setting_GetHighThreshold_SAL());
									low = float_format(setting_GetLowThreshold_SAL());	
									break;

								case TYPE_TDS:
									Enable_warning=setting_GetIsAlarm_TDS();
							  		high = float_format(setting_GetHighThreshold_TDS());
									low = float_format(setting_GetLowThreshold_TDS());	
									break;
								default:
									break;
								
						}						
						temp_warning = flag_NeedWarning;
						
						
						flag_NeedWarning = 0;//先初始化为0
						if(Enable_warning)
						{
							if((high!=0 && DO_GetDOmgL(get_CurDo())>=high) || (low!=0 && DO_GetDOmgL(get_CurDo())<=low))
							{
								flag_NeedWarning = 1;//报警标志
							}else{
								flag_NeedWarning = 0;//报警标志
							}
	//						if(low!=0 && DO_GetDOmgL(get_CurDo())<=low)
	//						{
	//							flag_NeedWarning = 1;//比低阈值低的话
	//						}
							if(temp_warning == 0 && flag_NeedWarning)//第一次报警响蜂鸣器
							{
								set_BeeAlarm();
							}
							if(temp_warning == 1 && !flag_NeedWarning)//第一次没有报警清一下图标
							{
								gui_ClearWarining();
							}
						}
						else
						{
				      		flag_NeedWarning = 0;//报警标志
						}				
					}

			
				if(DO_GetValueLocked(get_CurDo()))
				{
					gui_DrawLock((uint8_t *)icon_lock);//画锁
				}
				else 
				{
					gui_ClearLock();//删锁
				}
				

				if(get_COMADo() != NULL && get_COMBDo() != NULL)
				{
					if(get_CurDo() == get_COMADo())
					{
					  gui_DrawUpDownPage((uint8_t *)Down_Page);						
					}else{

						gui_DrawUpDownPage((uint8_t *)Up_Page);
					}			
				}else{
				    gui_ClearUpDownPage();
				}
				
			}
			break;
			

					
			default:
				break;
		}
	}
	else
	{
		switch(interfacial_GetCurPage())
	  	{
			case PAGE_0_START:
				gui_ClearLock();//删锁
				gui_ClearUpDownPage();
			  break;
			default:
				break;
		}
	}
	
	
	if(interfacial_GetCurPage() == PAGE_3_GPS && setting_GetIsKeyGps())//添加正在搜索
	{
		static uint8_t gps_searching = 0;
		if(get_LatitudeArr()[0] == 0)
		{
			gps_searching = 1;
			if(setting_GetIsChn())                                                                  //状态栏文字标签
			{
				GUI_PutChnStr(16, 104, (uint8_t *)zhengzaisousuo_cn, sizeof(zhengzaisousuo_cn), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
			}
			else
			{
				GUI_PutEngStr(16, 104, (uint8_t *)zhengzaisousuo_en, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
			}
		}
		else
		{
			if(gps_searching)
			{
				gps_searching=0;
				gui_ClearLines(84, 120, 0);
				LabelList_Add( 0,  126, (uint8_t *)jingdu_cn, sizeof(jingdu_cn), (uint8_t *)jingdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//经度
				LabelList_Add( 0,  78, (uint8_t *)weidu_cn, sizeof(weidu_cn), (uint8_t *)weidu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//纬度
				
				LabelList_Add( 0,  94, NULL, 0,  (uint8_t *)get_ns(),           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,&(interfacial_GetCurrentInterfacial()->label_head));//南北
				LabelList_Add(24,  94, NULL, 0, (uint8_t *)get_LatitudeArr(),  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,&(interfacial_GetCurrentInterfacial()->label_head));//纬度						
				LabelList_Add( 0, 142, NULL, 0, (uint8_t *)get_ew(),           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &(interfacial_GetCurrentInterfacial()->label_head));//东西
				LabelList_Add(24, 142, NULL, 0, (uint8_t *)get_LongitudeArr(), LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,&(interfacial_GetCurrentInterfacial()->label_head));//经度
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



