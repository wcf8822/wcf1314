//用来生成界面 各个界面所包含的元素都写在这里

#include "generate_page.h"

#include "lcd_drive.h"
#include "loadbit.h"


#include "gps.h"

#include "rtc.h"

#include "setting.h"

#include "bmp280.h"

#include "rs485.h"

#include <stdio.h>

#include "dissolved_oxygen.h"
//uint32_t stack_top = __get_MSP(); 获取栈顶位置
//界面全白就说明堆满了分配不到空间了

static log_union log_u_generate;
static uint8_t LogTime_arr[19] = {0};

STATIC float row_spacing = 0;//设定行间距

log_union* generate_GetLogUnion(void)
{
	return &log_u_generate;
}

uint8_t* generate_GetLogTimeArr(void)
{
	return LogTime_arr;
}

float get_RowSpacing(void)
{
	return row_spacing;
}

//设置行间距
void set_RowSpacing(float value)
{
	row_spacing = value;
}

/*开始界面*/
void generate_StartPage(PtrToInterfacial interfacial)
{
	interfacial_Destory(interfacial);
	
}

/*菜单界面*/
void generate_MenuPage(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	OptionList_Add(0, (uint8_t *)chuanganqiguanli_cn,  sizeof(chuanganqiguanli_cn), (uint8_t *)chuanganqiguanli_en, PAGE_2_SENSORMANAGE,  OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)xitongshezhi_cn,      sizeof(xitongshezhi_cn),     (uint8_t *)xitongshezhi_en,     PAGE_2_SYSTEM,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, (uint8_t *)lishishuju_cn,        sizeof(lishishuju_cn),       (uint8_t *)lishishuju_en,       PAGE_2_HISTORY,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);

	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_0_START;//设定父界面
	
	interfacial->content_chn = (uint8_t *)caidan_cn;
	interfacial->ChnContent_size = sizeof(caidan_cn);
	interfacial->content_eng = (uint8_t *)caidan_en;
}
/*恢复校准系数*/
void generate_ResetCal(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_ONE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)&shi_cn, 1, (uint8_t *)shi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
//	OptionList_Add(1, (uint8_t *)&fou_cn, 1, (uint8_t *)fou_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NULL, &option_head);	//报警音
	
	interfacial->page_father = PAGE_0_START;//设定父界面
	
	interfacial->content_chn = (uint8_t *)huifuxiaozhunxishu_cn ;
	interfacial->ChnContent_size = sizeof(huifuxiaozhunxishu_cn);
	interfacial->content_eng = (uint8_t *)huifuxiaozhunxishu_en;
	
	interfacial->option_head = option_head;
}


/*传感器管理界面*/
void generate_SensorManage(PtrToInterfacial interfacial)
{
	uint8_t device_count = rs485_GetDeviceCount();
	PtrToDOProbe p = *(rs485_GetDoList());
	uint8_t option_index = 0;
	
	
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	/*自动搜索设备*/
//	OptionList_Add(0, (uint8_t *)zidongsousuoshebei_cn , sizeof(zidongsousuoshebei_cn), (uint8_t *)zidongsousuoshebei_en, PAGE_3_SENSORSSEARCH, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NULL, &option_head);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////这里的传感器得根据情况自动生成然后添加到这个链表上
	if(p != NULL)//如果do设备不为空的话
	{
		while(p!=NULL)
		{
			OptionList_Add(option_index++, NULL, 0, (uint8_t *)(p->name), PAGE_3_SENSORS, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
			p = p->next_DO;
		}
		
		//
	}
				
	
	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_1_MENU;//设定父界面
	
	interfacial->content_chn = (uint8_t *)chuanganqiguanli_cn;
	interfacial->ChnContent_size = sizeof(chuanganqiguanli_cn);
	interfacial->content_eng = (uint8_t *)chuanganqiguanli_en;
	
}

/*搜索传感器界面*/
void generate_SensorSearch(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);
	CurInterfacial_Destory();
	
	interfacial->page_father = PAGE_2_SENSORMANAGE;//设定父界面
	
	interfacial->content_chn = (uint8_t *)zidongsousuoshebei_cn;
	interfacial->ChnContent_size = sizeof(zidongsousuoshebei_cn);
	interfacial->content_eng = (uint8_t *)zidongsousuoshebei_en;
}


/*系统设置界面*/
void generate_SystemPage(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);//设置选项行间距为0
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	OptionList_Add(0, (uint8_t *)zuobiaochaxun_cn,       sizeof(zuobiaochaxun_cn),       (uint8_t *)zuobiaochaxun_en,       PAGE_3_GPS,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//坐标查询
	OptionList_Add(1, (uint8_t *)yandushezhi_cn,         sizeof(yandushezhi_cn),         (uint8_t *)yandushezhi_en,         PAGE_3_SALT,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//盐度设置
	OptionList_Add(2, (uint8_t *)qiyashezhi_cn,          sizeof(qiyashezhi_cn),          (uint8_t *)qiyashezhi_en,          PAGE_3_PRESSURE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//气压设置
	OptionList_Add(3, (uint8_t *)zidongguanji_cn,        sizeof(zidongguanji_cn),        (uint8_t *)zidongguanji_en,        PAGE_3_AUTOSHUT,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//自动关机
	OptionList_Add(4, (uint8_t *)baojingshezhi_cn,       sizeof(baojingshezhi_cn),       (uint8_t *)baojingshezhi_en,       PAGE_3_ALARM_TYPE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警设置
	OptionList_Add(5, (uint8_t *)tishiyinshezhi_cn,      sizeof(tishiyinshezhi_cn),      (uint8_t *)tishiyinshezhi_en,      PAGE_3_BEEP,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//按键音设置
	OptionList_Add(6, (uint8_t *)shijianshezhi_cn,       sizeof(shijianshezhi_cn),       (uint8_t *)shijianshezhi_en,       PAGE_3_TIME,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //时间设置
	
	OptionList_Add(7, (uint8_t *)dushusuoding_cn,       sizeof(dushusuoding_cn),         (uint8_t *)dushusuoding_en,     PAGE_3_AUTOLOCK,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//语言设置
	
	OptionList_Add(8, (uint8_t *)yuyanshezhi_cn,         sizeof(yuyanshezhi_cn),         (uint8_t *)yuyanshezhi_en,         PAGE_3_LANGUAGE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//语言设置
	OptionList_Add(9, (uint8_t *)yibiaoxinxi_cn,         sizeof(yibiaoxinxi_cn),         (uint8_t *)yibiaoxinxi_en,         PAGE_3_INFO,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//仪表信息
	OptionList_Add(10, (uint8_t *)chuchangshezhi_cn,       sizeof(chuchangshezhi_cn),     (uint8_t *)chuchangshezhi_en,       PAGE_3_RESERT,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //恢复出厂设置
	
	
	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_1_MENU;//设定父界面
	
	interfacial->content_chn = (uint8_t *)xitongshezhi_cn;
	interfacial->ChnContent_size = sizeof(xitongshezhi_cn);
	interfacial->content_eng = (uint8_t *)xitongshezhi_en;
}


/*时间设置界面*/
void generate_SetTimePage(PtrToInterfacial interfacial)
{
	uint8_t other_y = 0;
	set_RowSpacing(ROWSPACING_THREE);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	list_NanoOption NanoDate = NULL;   //日期子标签
	list_NanoOption NanoTime = NULL;   //时间子标签
	list_label label_head = NULL;
	
	NanoOptionList_Add( 56, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, RTC_GetYear(),   0, 99, NOT_SINGLE, &NanoDate);
	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, RTC_GetMonth(),  1, 12, NOT_SINGLE, &NanoDate);
	NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, RTC_GetDay(),    1, 31, NOT_SINGLE, &NanoDate);
	LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIEGANG, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add(72, other_y, NULL, 0, (uint8_t *)XIEGANG, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	OptionList_Add(0, (uint8_t *)riqi_cn ,   sizeof(riqi_cn),   (uint8_t *)riqi_en,     NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoDate, &option_head);

	other_y += 16 + get_RowSpacing();

	NanoOptionList_Add( 56, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, RTC_GetHour(),   0, 23, NOT_SINGLE, &NanoTime);
	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, RTC_GetMinute(), 0, 59, NOT_SINGLE, &NanoTime);
	NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, RTC_GetSecond(), 0, 59, NOT_SINGLE, &NanoTime);
	LabelList_Add(72, other_y, NULL, 0, (uint8_t *)MAOHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add(96, other_y, NULL, 0, (uint8_t *)MAOHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	OptionList_Add(1, (uint8_t *)shijian_cn, sizeof(shijian_cn), (uint8_t *)shijian_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoTime, &option_head);
	
	
	OptionList_Add(2, (uint8_t *)baocun_cn , sizeof(baocun_cn ), (uint8_t *)baocun_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)shijianshezhi_cn;
	interfacial->ChnContent_size = sizeof(shijianshezhi_cn);
	interfacial->content_eng = (uint8_t *)shijianshezhi_en;
}



/*设置气压值*/
void generate_SetPressure(PtrToInterfacial interfacial)
{
	double temp;
	int temp_int;
	uint8_t other_y;
	
	temp = bmp280_GetPress() + setting_GetAirCompensate();
	
	temp_int = (int)((temp*100) + 0.5);
	
	set_RowSpacing(ROWSPACING_FOUR);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;   //子选项  就一串数字
	list_NanoOption NanoUint  = NULL;
	list_label label_head     = NULL;      //标签项  显示单位 实时数值 和一些标签
	
	
	
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_UINT, 0, 0, 0, NOT_SINGLE, &NanoUint);//具体显示的东西在nanolist print通过判断界面来显示
	OptionList_Add(0, (uint8_t *)danwei_cn ,     sizeof(danwei_cn),      (uint8_t *)danwei_en,      NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoUint,  &option_head); //单位
	
	other_y += 16 + get_RowSpacing();
	LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_PressArr(), LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//气压值
	OptionList_Add(1, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//当前值
	
	other_y += 16 + get_RowSpacing();
	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000),      0, 9, IS_SINGLE,  &NanoPress);//下面修改值的选项
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位
	NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
	LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	OptionList_Add(2, (uint8_t *)xiaozhunzhi_cn, sizeof(xiaozhunzhi_cn), (uint8_t *)xiaozhunzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(3, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)qiyashezhi_cn ;
	interfacial->ChnContent_size = sizeof(qiyashezhi_cn);
	interfacial->content_eng = (uint8_t *)qiyashezhi_en;
}

/*设置盐度计*/
void generate_SetSalinity(PtrToInterfacial interfacial)
{
	uint8_t other_y;
	value_type temp;
	int temp_int;
	
	temp = setting_GetSalinity();
	temp_int = (int)((temp*10) + 0.5);
	
	set_SalArr(temp); //进入界面更新一下盐度的显示数组
	
	set_RowSpacing(ROWSPACING_FOUR);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head  = NULL;    //主选项  就一个save
	list_NanoOption NanoSal  = NULL;   //子选项  就一串数字
	list_NanoOption NanoUint = NULL;
	list_label label_head    = NULL;      //标签项  显示单位 实时数值 和一些标签
	
	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_UINT, 0, 0, 0, NOT_SINGLE, &NanoUint);//单位
	OptionList_Add(0, (uint8_t *)danwei_cn ,     sizeof(danwei_cn),      (uint8_t *)danwei_en,      NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoUint, &option_head); //单位
	
	other_y += 16 + get_RowSpacing();
	LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_SalArr(), LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//盐度值
	OptionList_Add(1, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);//当前值
	
	other_y += 16 + get_RowSpacing();
	LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/1000),      0, 9, IS_SINGLE,  &NanoSal);//000.0
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100), 0, 9, IS_SINGLE,  &NanoSal);
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoSal);
	NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoSal);
	OptionList_Add(2, (uint8_t *)xiaozhunzhi_cn, sizeof(xiaozhunzhi_cn), (uint8_t *)xiaozhunzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoSal,  &option_head);//实际值
	
	OptionList_Add(3, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);//保存
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)yandushezhi_cn  ;
	interfacial->ChnContent_size = sizeof(yandushezhi_cn );
	interfacial->content_eng = (uint8_t *)yandushezhi_en;
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
}

/*查询gps信息*/
void generate_GpsInfo(PtrToInterfacial interfacial)
{
	CurInterfacial_Destory();
	
	list_label label_head = NULL;      //标签项  显示单位 实时数值 和一些标签
	
	if(get_LatitudeArr()[0] != 0) //有数据的话
	{
		LabelList_Add( 0,  96, (uint8_t *)jingdu_cn, sizeof(jingdu_cn), (uint8_t *)jingdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//经度
		LabelList_Add( 0,  48, (uint8_t *)weidu_cn, sizeof(weidu_cn), (uint8_t *)weidu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//纬度
	}
	
	
	LabelList_Add( 0,  64, NULL, 0,  (uint8_t *)get_ns(),           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//南北
	LabelList_Add(24,  64, NULL, 0, (uint8_t *)get_LatitudeArr(),  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//纬度
	
	
	LabelList_Add( 0, 112, NULL, 0, (uint8_t *)get_ew(),           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//东西
	LabelList_Add(24, 112, NULL, 0, (uint8_t *)get_LongitudeArr(), LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//经度
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)zuobiaochaxun_cn ;
	interfacial->ChnContent_size = sizeof(zuobiaochaxun_cn);
	interfacial->content_eng = (uint8_t *)zuobiaochaxun_en;
	
	interfacial->label_head = label_head;
}

/*仪表信息*/
void generate_MeterInfo(PtrToInterfacial interfacial)///////////////////////////////////////////这个y要自己算
{
	CurInterfacial_Destory();
	
	list_label label_head = NULL;
	
	LabelList_Add( 0, 32+ROWSPACING_TWO, (uint8_t *)yingjianbanben_cn, sizeof(yingjianbanben_cn), (uint8_t *)yingjianbanben_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//硬件版本
	LabelList_Add( 0, 48+2*ROWSPACING_TWO, (uint8_t *)ruanjianbanben_cn, sizeof(ruanjianbanben_cn), (uint8_t *)ruanjianbanben_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//软件版本
	
	LabelList_Add( 96, 32+ROWSPACING_TWO, NULL, 0, (uint8_t *)HW_version[interfacial_GetHWVersion()], LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//HW
	LabelList_Add( 96, 48+2*ROWSPACING_TWO, NULL, 0, interfacial_GetSWVersion(), LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//SW
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)yibiaoxinxi_cn ;
	interfacial->ChnContent_size = sizeof(yibiaoxinxi_cn);
	interfacial->content_eng = (uint8_t *)yibiaoxinxi_en;
	
	interfacial->label_head = label_head;
}

/*设置提示音*/
void generate_BeepSeting(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_TWO);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)anjianyin_cn,  sizeof(anjianyin_cn),  (uint8_t *)anjianyin_en,  PAGE_4_KEYPADTONE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //按键音
	OptionList_Add(1, (uint8_t *)baojingyin_cn, sizeof(baojingyin_cn), (uint8_t *)baojingyin_en, PAGE_4_ALARMTONE,  OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)tishiyinshezhi_cn ;
	interfacial->ChnContent_size = sizeof(tishiyinshezhi_cn);
	interfacial->content_eng = (uint8_t *)tishiyinshezhi_en;
	
	interfacial->option_head = option_head;
}

/*设置按键音*/
void generate_KeypadTone(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_TWO);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //按键音
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	
	interfacial->page_father = PAGE_3_BEEP;//设定父界面
	
	interfacial->content_chn = (uint8_t *)anjianyin_cn ;
	interfacial->ChnContent_size = sizeof(anjianyin_cn);
	interfacial->content_eng = (uint8_t *)anjianyin_en;
	
	interfacial->option_head = option_head;
}
/*设置报警音*/
void generate_AlarmTone(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_TWO);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //按键音
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	
	interfacial->page_father = PAGE_3_BEEP;//设定父界面
	
	interfacial->content_chn = (uint8_t *)baojingyin_cn ;
	interfacial->ChnContent_size = sizeof(baojingyin_cn);
	interfacial->content_eng = (uint8_t *)baojingyin_en;
	
	interfacial->option_head = option_head;
}

/*自动关机*/
void generate_AutoShut(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)guanbi_cn,   sizeof(guanbi_cn),   (uint8_t *)guanbi_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //按键音
	OptionList_Add(1, (uint8_t *)minute5_cn,  sizeof(minute5_cn),  (uint8_t *)minute5_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	OptionList_Add(2, (uint8_t *)minute10_cn, sizeof(minute10_cn), (uint8_t *)minute10_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	OptionList_Add(3, (uint8_t *)minute20_cn, sizeof(minute20_cn), (uint8_t *)minute20_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)zidongguanji_cn ;
	interfacial->ChnContent_size = sizeof(zidongguanji_cn);
	interfacial->content_eng = (uint8_t *)zidongguanji_en;
	
	interfacial->option_head = option_head;
}

/*语言设置*/
void generate_Language(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_TWO);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	if(setting_GetIsChn())
	{
		OptionList_Add(0, (uint8_t *)zhongwen_cn, sizeof(zhongwen_cn), NULL, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //中文
		OptionList_Add(1, (uint8_t *)yingwen_cn,  sizeof(yingwen_cn),  NULL, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //英文
	}
	else
	{
		OptionList_Add(0, (uint8_t *)zhongwen_en, sizeof(zhongwen_en), NULL, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //中文
		OptionList_Add(1, (uint8_t *)yingwen_en,  sizeof(yingwen_en),  NULL, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //英文
	}
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)yuyanshezhi_cn ;
	interfacial->ChnContent_size = sizeof(yuyanshezhi_cn);
	interfacial->content_eng = (uint8_t *)yuyanshezhi_en;
	
	interfacial->option_head = option_head;
}

/*报警界面显示已连接的设备类型*/
void generate_AlarmType(PtrToInterfacial interfacial)//////////////////////////////////////////有新设备添加进来要改这里
{
	uint8_t option_index = 0;
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	set_RowSpacing(ROWSPACING_TWO);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	if(*(rs485_GetDoList())!=NULL)         //如果do设备列表中有设备的话添加DO设备
	{
		OptionList_Add(option_index++, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en_quan, PAGE_4_ALARM, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)baojingshezhi_cn ;
	interfacial->ChnContent_size = sizeof(baojingshezhi_cn);
	interfacial->content_eng = (uint8_t *)baojingshezhi_en;
	
	interfacial->option_head = option_head;
	
}

/*报警设置是否开启*/
void generate_AlarmSetting(PtrToInterfacial interfacial)
{
	
	
	set_RowSpacing(ROWSPACING_TWO);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  PAGE_5_ALARMVALUE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //开启
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE,         OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //关闭
	
	
	interfacial->page_father = PAGE_3_ALARM_TYPE;//设定父界面
	
	interfacial->content_chn = (uint8_t *)baojingshezhi_cn ;
	interfacial->ChnContent_size = sizeof(baojingshezhi_cn);
	interfacial->content_eng = (uint8_t *)baojingshezhi_en;
	
	interfacial->option_head = option_head;
}

/*高低门限阈值设置*/
void generate_AlarmValueSetting(PtrToInterfacial interfacial)
{
	uint8_t other_y;
	
	value_type temp_high;
	value_type temp_low;
	
	set_RowSpacing(ROWSPACING_THREE);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	list_option option_head  = NULL;
	list_NanoOption NanoHigh  = NULL;
	list_NanoOption NanoLow  = NULL;
	list_label label_head    = NULL;      //标签项  显示单位 实时数值 和一些标签
	
//	temp = setting_GetLowThreshold();
	
	switch(interfacial_GetAlarmSensorType())
	{
		case TYPE_DO:
			temp_low = setting_GetLowThreshold_DO();
			temp_high = setting_GetHighThreshold_DO();
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
	
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint16_t)temp_low/10,      0, 9, IS_SINGLE, &NanoLow);//低门限
	NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint16_t)temp_low%10,      0, 9, IS_SINGLE, &NanoLow);
	temp_low *= 10;
	NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, ((uint16_t)temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);
	temp_low *= 10;
	NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, ((uint16_t)temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);
	LabelList_Add(112, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	OptionList_Add(0, (uint8_t *)dimenxianyuzhi_cn,  sizeof(dimenxianyuzhi_cn),  (uint8_t *)dimenxianyuzhi_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoLow, &option_head);	//报警音
	
	other_y += 16 + get_RowSpacing();
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint16_t)temp_high/10,     0, 9, IS_SINGLE,  &NanoHigh);//高门限
	NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint16_t)temp_high%10,     0, 9, IS_SINGLE,  &NanoHigh);
	temp_high *= 10;
	NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  ((uint16_t)temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);
	temp_high *= 10;
	NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  ((uint16_t)temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);
	LabelList_Add(112, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	OptionList_Add(1, (uint8_t *)gaomenxianyuzhi_cn, sizeof(gaomenxianyuzhi_cn), (uint8_t *)gaomenxianyuzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoHigh, &option_head);	//报警音
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);//保存
	
	interfacial->page_father = PAGE_4_ALARM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)baojingshezhi_cn  ;
	interfacial->ChnContent_size = sizeof(baojingshezhi_cn );
	interfacial->content_eng = (uint8_t *)baojingshezhi_en;
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
}

/*恢复出厂设置*/
void generate_reset(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_ONE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)huifuchuchangshezhi_cn, sizeof(huifuchuchangshezhi_cn), (uint8_t *)huifuchuchangshezhi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
//	OptionList_Add(1, (uint8_t *)&fou_cn, 1, (uint8_t *)fou_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NULL, &option_head);	
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)chuchangshezhi_cn ;
	interfacial->ChnContent_size = sizeof(chuchangshezhi_cn);
	interfacial->content_eng = (uint8_t *)chuchangshezhi_en;
	
	interfacial->option_head = option_head;
}

/*自动锁定*/
void generate_AutoLock(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_THREE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)guanbi_cn,   sizeof(guanbi_cn),   (uint8_t *)guanbi_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)zidong_cn,   sizeof(zidong_cn),   (uint8_t *)zidong_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, (uint8_t *)shoudong_cn, sizeof(shoudong_cn), (uint8_t *)shoudong_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)dushusuoding_cn ;
	interfacial->ChnContent_size = sizeof(dushusuoding_cn);
	interfacial->content_eng = (uint8_t *)dushusuoding_en;
	
	interfacial->option_head = option_head;
}



/*就是具体传感器点进去*/
void generate_Sensors(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;
	uint8_t* sensor_name = interfacial_GetOptionSensorName();
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	OptionList_Add(option_index++, (uint8_t *)chuanganqixiaozhun_cn, sizeof(chuanganqixiaozhun_cn), (uint8_t *)chuanganqixiaozhun_en,     PAGE_4_CAL,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(option_index++, (uint8_t *)chuanganqixinxi_cn,    sizeof(chuanganqixinxi_cn),    (uint8_t *)chuanganqixinxi_en,        PAGE_4_SENSORINFO, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	if(sensor_name[0] != 'D')//如果是溶解氧的话
	{
		OptionList_Add(option_index++, (uint8_t *)canshushezhi_cn,       sizeof(canshushezhi_cn),       (uint8_t *)chuanganqicanshushezhi_en, PAGE_4_PARASET,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_2_SENSORMANAGE;//设定父界面
	
	interfacial->content_chn = (uint8_t *)NULL;
	interfacial->ChnContent_size = 0;
	interfacial->content_eng = interfacial_GetOptionSensorName(); //将标签的字赋给界面
}

/*传感器信息*/
void generate_SensorInfo(PtrToInterfacial interfacial)
{
	uint8_t other_y;
	
	uint8_t* sensor_name = interfacial_GetOptionSensorName();
	
	set_RowSpacing(ROWSPACING_FIVE);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head      = NULL;
	list_NanoOption NanoModbusId = NULL;   //子选项  就一串数字
	list_label label_head        = NULL;      //标签项  显示单位 实时数值
	
	//根据设备名字来判断是哪种设备
	if(sensor_name[0] == 'D')//如果是溶解氧的话
	{
		PtrToDOProbe probe_do = DO_FindByName(sensor_name, rs485_GetDoList());

		LabelList_Add( 32, other_y, NULL, 0, (uint8_t *)probe_do->SN,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//SN
		
		other_y += 16 + get_RowSpacing();
		LabelList_Add( 96, other_y, NULL, 0, (uint8_t *)probe_do->HWV, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//HW
		
		other_y += 16 + get_RowSpacing();
		LabelList_Add( 96, other_y, NULL, 0, (uint8_t *)probe_do->SWV, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//SW
		
		other_y += 16 + get_RowSpacing();
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, ((probe_do->modbus_id)>>4), 0, 15, IS_SINGLE, &NanoModbusId);//modbusid   F0
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (probe_do->modbus_id)&0x0f, 0, 15, IS_SINGLE, &NanoModbusId);
	}
	
	OptionList_Add(0, (uint8_t *)NULL,              0,                         (uint8_t *)chuchangxulie_en,  NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY,  NULL,         &option_head);//SN
	OptionList_Add(1, (uint8_t *)yingjianbanben_cn, sizeof(yingjianbanben_cn), (uint8_t *)yingjianbanben_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);//HW
	OptionList_Add(2, (uint8_t *)ruanjianbanben_cn, sizeof(ruanjianbanben_cn), (uint8_t *)ruanjianbanben_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);//SW
	OptionList_Add(3, (uint8_t *)NULL,              0,                         (uint8_t *)dizhi_en,          NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, IS_ENG_ONLY,  NanoModbusId, &option_head);//ID
	OptionList_Add(4, (uint8_t *)baocun_cn,         sizeof(baocun_cn),         (uint8_t *)baocun_en,         NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);//保存
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_3_SENSORS;//设定父界面
	
	interfacial->content_chn = (uint8_t *)chuanganqixinxi_cn ;
	interfacial->ChnContent_size = sizeof(chuanganqixinxi_cn);
	interfacial->content_eng = (uint8_t *)chuanganqixinxi_en;
}
/*传感器界面里的参数设置*/
void generate_ParameterSetting(PtrToInterfacial interfacial)
{
	uint8_t* sensor_name = interfacial_GetOptionSensorName();
	
	set_RowSpacing(ROWSPACING_MORE);
	CurInterfacial_Destory();
	
	list_option option_head      = NULL;
	
	//根据设备名字来判断是哪种设备
	if(sensor_name[0] == 'D')//如果是溶解氧的话
	{
			OptionList_Add(0, (uint8_t *)maomoxiaozhun_cn, sizeof(maomoxiaozhun_cn), (uint8_t *)maomoxiaozhun_en, PAGE_5_SENSORCAP, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	
	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_3_SENSORS;//设定父界面
	
	interfacial->content_chn = (uint8_t *)canshushezhi_cn ;
	interfacial->ChnContent_size = sizeof(canshushezhi_cn);
	interfacial->content_eng = (uint8_t *)chuanganqicanshushezhi_en;
}

/*溶解氧荧光帽设置*/
void generate_SensorCap(PtrToInterfacial interfacial)
{
	
}

/*校准界面*/
void generate_Cal(PtrToInterfacial interfacial, PAGE_NUM father_page)
{
	uint8_t* sensor_name;//获取名字
	
	set_RowSpacing(ROWSPACING_MORE);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	
	switch (father_page)
	{
		case PAGE_3_SENSORS:
			sensor_name = interfacial_GetOptionSensorName();
			if(sensor_name!=NULL && sensor_name[0] == 'D')//如果是溶解氧的话
			{
				OptionList_Add(0, (uint8_t *)dandian_cn,   sizeof(dandian_cn),   (uint8_t *)dandian_en,   PAGE_5_ONE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
				OptionList_Add(1, (uint8_t *)liangdian_cn, sizeof(liangdian_cn), (uint8_t *)liangdian_en, PAGE_5_TWOFIRST, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
				interfacial->content_chn = (uint8_t *)rongjieyangjiaozhun_cn;
				interfacial->ChnContent_size = sizeof(rongjieyangjiaozhun_cn);
				interfacial->content_eng = (uint8_t *)rongjieyangjiaozhun_en;
			}
			break;
		
		case PAGE_0_START:
			switch(rs485_GetSensorType())
			{
				case TYPE_DO:
					OptionList_Add(0, (uint8_t *)dandian_cn,   sizeof(dandian_cn),   (uint8_t *)dandian_en,   PAGE_5_ONE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
					OptionList_Add(1, (uint8_t *)liangdian_cn, sizeof(liangdian_cn), (uint8_t *)liangdian_en, PAGE_5_TWOFIRST, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
					interfacial->content_chn = (uint8_t *)rongjieyangjiaozhun_cn;
					interfacial->ChnContent_size = sizeof(rongjieyangjiaozhun_cn);
					interfacial->content_eng = (uint8_t *)rongjieyangjiaozhun_en;
				break;
				
				default:
					break;
			}
			break;
		
		default:
			break;
	}

	interfacial->option_head = option_head;
	interfacial->page_father = father_page;//设定父界面
}


/*单点校准*/
void generate_OnePoint(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;
	
	switch (sensor_type)
	{
		case TYPE_DO:
			LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
			LabelList_Add(100, 56, NULL, 0, (uint8_t *)BAIFENGHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
			break;

		
		
		
		
		default:
			break;
	}
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)dandian_cn;
	interfacial->ChnContent_size = sizeof(dandian_cn);
	interfacial->content_eng = (uint8_t *)dandian_en;
}

/*两点校准第一个点的界面*/
void generate_TwoPointFirst(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head = NULL;
	
	switch (sensor_type)
	{
		case TYPE_DO:
			LabelList_Add( 0, 36, (uint8_t *)diyidian_cn, sizeof(diyidian_cn), (uint8_t *)diyidian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点
			LabelList_Add(100, 56, NULL, 0, (uint8_t *)BAIFENGHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
			break;

		
		
		
		
		
		
		
		
		
		
		default:
			break;
	}
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)liangdian_cn;
	interfacial->ChnContent_size = sizeof(liangdian_cn);
	interfacial->content_eng = (uint8_t *)liangdian_en;
}
/*两点校准的第二个界面*/
void generate_TwoPointSecond(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head = NULL;
	
	switch (sensor_type)
	{
		case TYPE_DO:
			LabelList_Add( 0, 36, (uint8_t *)dierdian_cn, sizeof(dierdian_cn), (uint8_t *)dierdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点
			LabelList_Add(100, 56, NULL, 0, (uint8_t *)BAIFENGHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			
		
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
			break;

		
		
		default:
			break;
	}
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)liangdian_cn;
	interfacial->ChnContent_size = sizeof(liangdian_cn);
	interfacial->content_eng = (uint8_t *)liangdian_en;
}
/*历史数据界面*/
void generate_Histor(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	OptionList_Add(0, (uint8_t *)shujubaocun_cn,  sizeof(shujubaocun_cn),  (uint8_t *)shujubaocun_en,  PAGE_3_DATASAVE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)shujuxianshi_cn, sizeof(shujuxianshi_cn), (uint8_t *)shujuxianshi_en, PAGE_3_DATASHOW,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, (uint8_t *)shujushanchu_cn, sizeof(shujushanchu_cn), (uint8_t *)shujushanchu_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);

	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_1_MENU;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lishishuju_cn;
	interfacial->ChnContent_size = sizeof(lishishuju_cn);
	interfacial->content_eng = (uint8_t *)lishishuju_en; //将标签的字赋给界面
}

void update_LogTitle(log_union* log , uint8_t index)
{
	snprintf((char *)LogTime_arr, sizeof(LogTime_arr), "%05d %02d%02d%02d %02d:%02d", index,
																																										log->log.time.years,
																																										log->log.time.month,
																																										log->log.time.day,
																																										log->log.time.hour,
																																										log->log.time.minute);
}

/*笼统的总数据显示界面*/
void generate_DataView(PtrToInterfacial interfacial, uint16_t data_index)
{
	static uint8_t use_size_arr[6] = {0};//49,152

	list_label label_head = NULL;
	
	list_option option_head = NULL;    //主标签
	
	set_RowSpacing(64);
	
	CurInterfacial_Destory();
	
	LabelList_Add( 0, 32, (uint8_t *)zuidakeyong_cn, sizeof(zuidakeyong_cn), (uint8_t *)zuidakeyong_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add( 0, 48, (uint8_t *)dangqianyiyong_cn, sizeof(dangqianyiyong_cn), (uint8_t *)dangqianyiyong_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add( 0, 64, (uint8_t *)dangqianxuanze_cn , sizeof(dangqianxuanze_cn ), (uint8_t *)dangqianxuanze_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	//snprintf((char *)interfacial_GetCurLogIndexArr(), 6, "%5d", data_index);//先初始化一下
	interfacial_UpdateCurLogIndexArr(data_index);
	snprintf((char *)use_size_arr, sizeof(use_size_arr), "%5d", log_GetLogCount());
	
	LabelList_Add( 80, 32, NULL, 0, (uint8_t *)maxsize,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //最大可用
	LabelList_Add( 80, 48, NULL, 0, use_size_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //已用大小
	LabelList_Add( 80, 64, NULL, 0, interfacial_GetCurLogIndexArr(),  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第几条记录
	
	
	if(data_index != 0)
	{
		log_ReadData(&log_u_generate, data_index - 1);
		
		update_LogTitle(&log_u_generate, data_index);
		
		OptionList_Add(0, NULL,  0,  LogTime_arr,  PAGE_4_DATALOG,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);//具体的记录
		
	}
	else
	{
		OptionList_Add(0, (uint8_t *)nodata_cn,  sizeof(nodata_cn),  (uint8_t *)nodata_en,  NONE_PAGE, OPTION_LARGE, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);//具体的记录
	}
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_2_HISTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)shujuxianshi_cn;
	interfacial->ChnContent_size = sizeof(shujuxianshi_cn);
	interfacial->content_eng = (uint8_t *)shujuxianshi_en;
}


/*删除数据*/
void generate_DataDelete(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_ONE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)&shi_cn, 1, (uint8_t *)shi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
//	OptionList_Add(1, (uint8_t *)&fou_cn, 1, (uint8_t *)fou_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NULL, &option_head);	
	
	interfacial->page_father = PAGE_2_HISTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)shujushanchu_cn ;
	interfacial->ChnContent_size = sizeof(shujushanchu_cn);
	interfacial->content_eng = (uint8_t *)shujushanchu_en;
	
	interfacial->option_head = option_head;
}

/*保存数据*/
void generate_DataStore(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_ONE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)&baocun_cn, sizeof(baocun_cn), (uint8_t *)baocun_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	
	interfacial->page_father = PAGE_2_HISTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)shujubaocun_cn;
	interfacial->ChnContent_size = sizeof(shujubaocun_cn);
	interfacial->content_eng = (uint8_t *)shujubaocun_en;
	
	interfacial->option_head = option_head;
}

/*具体的数据显示界面总数据界面*/
void generate_DataLog(PtrToInterfacial interfacial)
{
	static uint8_t press[7];
	static uint8_t sal[7]; 
	static uint8_t value1_arr[7] = {0};//设备的第一个值
	static uint8_t value2_arr[7] = {0};//设备的第二个值
	static uint8_t value3_arr[7] = {0};//设备的第三个值
	
	list_label label_head = NULL;
	
	CurInterfacial_Destory(); //销毁界面
	
	snprintf((char *)press, 7, "%6.2f", log_u_generate.log.log_data.pressure);//先初始化一下
	snprintf((char *)sal, 7, "%6.2f", log_u_generate.log.log_data.salinity);
	
	LabelList_Add( 0, 32, (uint8_t *)leixing_cn, sizeof(leixing_cn), (uint8_t *)leixing_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//类型
	LabelList_Add( 48, 32, NULL, 0, (uint8_t *)log_u_generate.log.type_str,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	LabelList_Add( 0, 48, NULL, 0, (uint8_t *)SN_str,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//SN
	LabelList_Add( 24, 48, NULL, 0, (uint8_t *)log_u_generate.log.sn,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	LabelList_Add( 0, 64, NULL, 0, (uint8_t *)DT_str,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//DT
	LabelList_Add( 24, 64, NULL, 0, (uint8_t *)(&LogTime_arr[6]),  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	
	LabelList_Add( 0, 80, (uint8_t *)qiya_cn, sizeof(qiya_cn), (uint8_t *)qiya_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//气压
	LabelList_Add( setting_GetIsChn() ? 32 : 64, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_KPA, HAVE_PARENTHESIS, &label_head);//(kpa):
	LabelList_Add( 112, 80, NULL, 0, (uint8_t *)press,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	
	LabelList_Add( 0, 96, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//盐度
	LabelList_Add( setting_GetIsChn() ? 32 : 64, 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, HAVE_PARENTHESIS, &label_head);//(ppt):
	LabelList_Add( 112, 96, NULL, 0, (uint8_t *)sal,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	switch(log_u_generate.log.type_str[0])//判断设备类型的第一位的字符
	{
		case 'D'://DO
			snprintf((char *)value1_arr, 7, "%6.2f", log_u_generate.log.log_data.temperature);//先初始化一下
			snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.DO_percent);
			snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.DO_mg_L);
		
			LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			LabelList_Add( 0, 128, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//DO %
			LabelList_Add( setting_GetIsChn() ? 48 : 16, 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, HAVE_PARENTHESIS, &label_head);//(%):
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value2_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			LabelList_Add( 0, 144, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//DO mg/L
			LabelList_Add( setting_GetIsChn() ? 48 : 16, 144, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, HAVE_PARENTHESIS, &label_head);//(mg/L):
			LabelList_Add( 112, 144, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			
			break;
		
		default:
			break;
		
	}
	
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_3_DATASHOW;
	
	interfacial->content_chn = (uint8_t *)shujuxianshi_cn;
	interfacial->ChnContent_size = sizeof(shujuxianshi_cn);
	interfacial->content_eng = (uint8_t *)shujuxianshi_en;
}

/*具体的数据显示界面GPS界面*/
void generate_DataLogGPS(PtrToInterfacial interfacial)
{
	list_label label_head = NULL;
	
	CurInterfacial_Destory();
	
	LabelList_Add( 0,  32, (uint8_t *)weidu_cn, sizeof(weidu_cn), (uint8_t *)weidu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//纬度
	LabelList_Add( 0,  48, NULL, 0, (uint8_t *)log_u_generate.log.N_S,           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//南北
	LabelList_Add(24,  48, NULL, 0, (uint8_t *)log_u_generate.log.latitude,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//纬度
	
	LabelList_Add( 0,  64, (uint8_t *)jingdu_cn, sizeof(jingdu_cn), (uint8_t *)jingdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//经度
	LabelList_Add( 0,  80, NULL, 0, (uint8_t *)log_u_generate.log.E_W,           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//东西
	LabelList_Add(24,  80, NULL, 0, (uint8_t *)log_u_generate.log.longitude, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//经度
	
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_3_DATASHOW;
	
	interfacial->content_chn = (uint8_t *)shujuxianshi_cn;
	interfacial->ChnContent_size = sizeof(shujuxianshi_cn);
	interfacial->content_eng = (uint8_t *)shujuxianshi_en;
}





