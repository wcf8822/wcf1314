//用来生成界面 各个界面所包含的元素都写在这里

#include "generate_page.h"

#include "lcd_drive.h"
#include "loadbit.h"

#include <math.h>
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

	uint8_t option_index = 0;
	
	
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签

  if(get_COMADo() !=NULL){
		OptionList_Add(option_index++, NULL, 0, (uint8_t *)(get_COMADo()->name), PAGE_3_SENSORS, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);	
	}				
  if(get_COMBDo() !=NULL){
		OptionList_Add(option_index++, NULL, 0, (uint8_t *)(get_COMBDo()->name), PAGE_3_SENSORS, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);	
	}	
  if(get_COMADo() ==NULL && get_COMBDo() ==NULL){	
	  list_label label_head = NULL;			
   	LabelList_Add(50, 80,  (uint8_t *)wuchuanganqi_cn, sizeof(wuchuanganqi_cn), (uint8_t *)wuchuanganqi_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		

		interfacial->label_head = label_head;
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
	uint8_t index = 0; //因为gps不一定有所以序号要自动增加不能直接写死
	set_RowSpacing(ROWSPACING_MORE);//设置选项行间距为0
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	if(setting_GetHaveGps())
	{
		OptionList_Add(index++,  (uint8_t *)zuobiaochaxun_cn,  sizeof(zuobiaochaxun_cn),  (uint8_t *)zuobiaochaxun_en,  PAGE_3_GPS,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//坐标查询
	}
	

	OptionList_Add(index++,  (uint8_t *)zidongguanji_cn,   sizeof(zidongguanji_cn),   (uint8_t *)zidongguanji_en,   PAGE_3_AUTOSHUT,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//自动关机
	OptionList_Add(index++,  (uint8_t *)zidongbaocun_cn,   sizeof(zidongbaocun_cn),   (uint8_t *)zidongbaocun_en,   PAGE_3_AUTOSAVE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//自动保存
	OptionList_Add(index++,  (uint8_t *)tishiyinshezhi_cn, sizeof(tishiyinshezhi_cn), (uint8_t *)tishiyinshezhi_en, PAGE_3_BEEP,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//按键音设置
	OptionList_Add(index++,  (uint8_t *)shijianshezhi_cn,  sizeof(shijianshezhi_cn),  (uint8_t *)shijianshezhi_en,  PAGE_3_TIME,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //时间设置
	OptionList_Add(index++,  (uint8_t *)yuyanshezhi_cn,    sizeof(yuyanshezhi_cn),    (uint8_t *)yuyanshezhi_en,    PAGE_3_LANGUAGE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//语言设置
	OptionList_Add(index++,  (uint8_t *)danweishezhi_cn,    sizeof(danweishezhi_cn),     (uint8_t *)danweishezhi_en,    PAGE_3_UNIT,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//单位设置
	OptionList_Add(index++,  (uint8_t *)yibiaoxinxi_cn,    sizeof(yibiaoxinxi_cn),    (uint8_t *)yibiaoxinxi_en,    PAGE_3_INFO,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//仪表信息
	OptionList_Add(index++,  (uint8_t *)chuchangshezhi_cn, sizeof(chuchangshezhi_cn), (uint8_t *)chuchangshezhi_en, PAGE_3_RESERT,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //恢复出厂设置
	
	
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
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
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
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)yandushezhi_cn  ;
	interfacial->ChnContent_size = sizeof(yandushezhi_cn );
	interfacial->content_eng = (uint8_t *)yandushezhi_en;
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
}

/*查询gps信息*/
void generate_GpsInfo(PtrToInterfacial interfacial)
{
	
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();

	list_option option_head  = NULL;	
	list_label label_head = NULL;      //标签项  显示单位 实时数值 和一些标签

	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //按键音
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	
	if(setting_GetIsKeyGps())
	{
		if(get_LatitudeArr()[0] != 0) //有数据的话
		{
			LabelList_Add( 0,  126, (uint8_t *)jingdu_cn, sizeof(jingdu_cn), (uint8_t *)jingdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//经度
			LabelList_Add( 0,  78, (uint8_t *)weidu_cn, sizeof(weidu_cn), (uint8_t *)weidu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//纬度
		}
		
		
		LabelList_Add( 0,  94, NULL, 0,  (uint8_t *)get_ns(),           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//南北
		LabelList_Add(24,  94, NULL, 0, (uint8_t *)get_LatitudeArr(),  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//纬度
		
		
		LabelList_Add( 0, 142, NULL, 0, (uint8_t *)get_ew(),           LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//东西
		LabelList_Add(24, 142, NULL, 0, (uint8_t *)get_LongitudeArr(), LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//经度
	
	}
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)zuobiaochaxun_cn ;
	interfacial->ChnContent_size = sizeof(zuobiaochaxun_cn);
	interfacial->content_eng = (uint8_t *)zuobiaochaxun_en;
	
	interfacial->label_head = label_head;
	interfacial->option_head = option_head;
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
	set_RowSpacing(ROWSPACING_MORE);//两个
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
	set_RowSpacing(ROWSPACING_MORE);//两个
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


/*设置测量模式*/
void generate_Set_Mes_mode(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)lianxuceliang_cn, sizeof(lianxuceliang_cn), (uint8_t *)lianxuceliang_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警音
	OptionList_Add(1, (uint8_t *)danciceliang_cn,  sizeof(danciceliang_cn),  (uint8_t *)danciceliang_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //按键音

	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)celiangmoshi_cn;
	interfacial->ChnContent_size = sizeof(celiangmoshi_cn);
	interfacial->content_eng = (uint8_t *)celiangmoshi_en;
	
	interfacial->option_head = option_head;
}

/*温度系数设置和清洁周期设置*/
void generate_Temp_Clear_Time(PtrToInterfacial interfacial, PAGE_NUM cur_PageNum)
{
	double temp;
	int temp_int;
	uint8_t other_y;
	PtrToDOProbe p;
	set_RowSpacing(ROWSPACING_THREE);//设置行间距
	other_y = get_RowSpacing() + OPTION_STARTY;//用于子标签或者别的标签定位用y值
	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;    //子选项  就一串数字
	list_label label_head     = NULL;    //标签项  显示单位 实时数值 和一些标签

	if(get_CurDo()->modbus_id == LH_DX01_ModbusID)
	{
		LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_Vol_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	}
	else
	{
		if(cur_PageNum == PAGE_4_Clear_time)
		{
			LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_Vol_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		}
		else
		{
			LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_CurDo()->tocmgl_Vol_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		}
	}
	OptionList_Add(0, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//当前值
	
	other_y += 16 + get_RowSpacing();

	if(get_CurDo()->modbus_id == LH_DX01_ModbusID)
	{
		temp =  get_CurDo()->Measure_Range.value_f;
		temp_int = (int)((temp) + 0.5);
		NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 1, IS_SINGLE,  &NanoPress);
		NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 3, IS_SINGLE,  &NanoPress);
		NanoOptionList_Add(96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
		NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位

		interfacial->content_chn = (uint8_t *)qingjiezhouqishezhi_cn ;
		interfacial->ChnContent_size = sizeof(qingjiezhouqishezhi_cn);
		interfacial->content_eng = (uint8_t *)qingjiezhouqishezhi_en;
	}
	else
	{
		if(cur_PageNum == PAGE_4_Clear_time)
		{
			temp = Get_OIW_Clear_Time();
			temp_int = (int)((temp) + 0.5);
			NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 1, IS_SINGLE,  &NanoPress);
			NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 3, IS_SINGLE,  &NanoPress);
			NanoOptionList_Add(96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
			NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位

			interfacial->content_chn = (uint8_t *)qingjiezhouqishezhi_cn ;
			interfacial->ChnContent_size = sizeof(qingjiezhouqishezhi_cn);
			interfacial->content_eng = (uint8_t *)qingjiezhouqishezhi_en;
		}
		else
		{
			temp = atof(get_CurDo()->tocmgl_Vol_arr);
			temp_int = (int)((temp*100) + 0.5);
			NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  68, 67, 68, IS_SINGLE,  &NanoPress);
			NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 1, IS_SINGLE,  &NanoPress);
			NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
			NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
			LabelList_Add(112, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			interfacial->content_chn = (uint8_t *)wendubuchangxishushezhi_cn ;
			interfacial->ChnContent_size = sizeof(wendubuchangxishushezhi_cn);
			interfacial->content_eng = (uint8_t *)wendubuchangxishushezhi_en;
		}
	}
	OptionList_Add(1, (uint8_t *)shezhizhi_cn, sizeof(shezhizhi_cn), (uint8_t *)shezhizhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
}


/*DX01温度精度设置*/
void generate_DX01_Temp_xiaoshu(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, NULL, 0,   (uint8_t *)wendujingdu_0_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, NULL, 0,  (uint8_t *)wendujingdu_1_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, NULL, 0, (uint8_t *)wendujingdu_2_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)wendujingdushezhi_cn ;
	interfacial->ChnContent_size = sizeof(wendujingdushezhi_cn);
	interfacial->content_eng = (uint8_t *)wendujingdushezhi_en;
	
	interfacial->option_head = option_head;
}

/*生成单次测量时间设置界面*/
void generate_Set_Mes_Time(PtrToInterfacial interfacial)
{
	double temp;
	int temp_int;
	uint8_t other_y;
	PtrToDOProbe p;
	
	set_RowSpacing(ROWSPACING_THREE);//设置行间距
	other_y = get_RowSpacing() + OPTION_STARTY;//用于子标签或者别的标签定位用y值
	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;    //子选项  就一串数字
	list_label label_head     = NULL;    //标签项  显示单位 实时数值 和一些标签
	
	p = get_CurDo();
	temp = p->DC17_Mes_Time.value_f;
	
	LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_CurDo()->Mes_Timearr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//显示测量时间间隔
	OptionList_Add(0, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//当前值
	
	other_y += 16 + get_RowSpacing();

	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add(96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
	
	OptionList_Add(1, (uint8_t *)shezhizhi_cn, sizeof(shezhizhi_cn), (uint8_t *)shezhizhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)danciceliangshijianshezhi_cn ;
	interfacial->ChnContent_size = sizeof(danciceliangshijianshezhi_cn);
	interfacial->content_eng = (uint8_t *)danciceliangshijianshezhi_en;
}

/*设置报警音*/
void generate_AlarmTone(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);//两个
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
/*DZ09温度精度设置*/
void generate_DZ09_Temp_xiaoshu(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, NULL, 0,   (uint8_t *)wendujingdu_0_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, NULL, 0,  (uint8_t *)wendujingdu_1_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, NULL, 0, (uint8_t *)wendujingdu_2_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)wendujingdushezhi_cn ;
	interfacial->ChnContent_size = sizeof(wendujingdushezhi_cn);
	interfacial->content_eng = (uint8_t *)wendujingdushezhi_en;
	
	interfacial->option_head = option_head;
}
/*自动保存*/
void generate_AutoSave(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);
//	set_RowSpacing(ROWSPACING_MORE);//设置选项行间距为0
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0,  (uint8_t *)jiangebaocun_cn,   sizeof(jiangebaocun_cn),   (uint8_t *)jiangebaocun_en,   PAGE_4_INTERVALSAVE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//间隔保存
	OptionList_Add(1,  (uint8_t *)suodingbaocun_cn,  sizeof(suodingbaocun_cn),  (uint8_t *)suodingbaocun_en,  PAGE_4_LOCKSAVE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//锁定保存

	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)zidongbaocun_cn;
	interfacial->ChnContent_size = sizeof(zidongbaocun_cn);
	interfacial->content_eng = (uint8_t *)zidongbaocun_en;
	
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

/*单位设置*/
void generate_Unit(PtrToInterfacial interfacial, uint8_t Unit)
{
	uint8_t other_y;
	
	set_RowSpacing(ROWSPACING_TWO);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head  = NULL;//标签 次数 保存
	list_NanoOption NanoTimes  = NULL;//就次数设置
	list_label label_head = NULL;
	
	NanoOptionList_Add( 88, other_y,(uint8_t *)sheshidu_cn,  1,  NULL, NANOOPTION_STRING,Unit, 0, 1, IS_SINGLE, &NanoTimes);//低门限		
	
	OptionList_Add(0, (uint8_t *)wendudanwei_cn,  sizeof(wendudanwei_cn),   (uint8_t *)wendudanwei_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoTimes, &option_head);	//报警音
	OptionList_Add(1, (uint8_t *)baocun_cn, sizeof(baocun_cn),  (uint8_t *)baocun_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存

	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)danweishezhi_cn ;
	interfacial->ChnContent_size = sizeof(danweishezhi_cn);
	interfacial->content_eng = (uint8_t *)danweishezhi_en;
	
	interfacial->option_head = option_head;
}


/*报警设置是否开启*/
void generate_AlarmSetting(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  PAGE_5_ALARMVALUE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //开启
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE,         OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //关闭
	
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)baojingshezhi_cn ;
	interfacial->ChnContent_size = sizeof(baojingshezhi_cn);
	interfacial->content_eng = (uint8_t *)baojingshezhi_en;
	
	interfacial->option_head = option_head;
}

/*高低门限阈值设置*/
void generate_AlarmValueSetting(PtrToInterfacial interfacial)
{
	uint8_t other_y;//y临时变量 用于设定位置方便生成数值标签
	
	value_type temp_high;
	value_type temp_low;
	
	set_RowSpacing(ROWSPACING_THREE);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	list_option option_head  = NULL;
	list_NanoOption NanoHigh  = NULL;
	list_NanoOption NanoLow  = NULL;
	list_label label_head    = NULL;      //标签项  显示小数点
	
	switch(rs485_GetSensorType())
	{
		case TYPE_DO:
			temp_high = setting_GetHighThreshold_DO();	
			temp_low = setting_GetLowThreshold_DO();
			break;
		
		case TYPE_pH:
			temp_high = float_format(setting_GetHighThreshold_pH());
			temp_low = float_format(setting_GetLowThreshold_pH());			
			break;
		
		case TYPE_Tur:
			temp_high = setting_GetHighThreshold_Tur();
			temp_low = setting_GetLowThreshold_Tur();		
			break;
		
		case TYPE_FCL:
			temp_high =setting_GetHighThreshold_FCL();
			temp_low = setting_GetLowThreshold_FCL();		
			break;
		
		case TYPE_EC:
			temp_high = setting_GetHighThreshold_EC();
			temp_low = setting_GetLowThreshold_EC();		
			break;
		
		case TYPE_ORP:
			temp_high = fabs(setting_GetHighThreshold_ORP());
			temp_low =  fabs(setting_GetLowThreshold_ORP());		
			break;
		
		case TYPE_NH4:
			temp_high = setting_GetHighThreshold_NH4();
			temp_low = setting_GetLowThreshold_NH4();							
			break;
		
		case TYPE_F:
			temp_high = setting_GetHighThreshold_F();
			temp_low = setting_GetLowThreshold_F();		
			break;
		
		case TYPE_CL:
			temp_high = setting_GetHighThreshold_CL();
			temp_low = setting_GetLowThreshold_CL();		
			break;
		
		case TYPE_Chl:
			temp_high = setting_GetHighThreshold_Chl();
			temp_low = setting_GetLowThreshold_Chl();		
			break;
		
		case TYPE_Bga:
			temp_high = setting_GetHighThreshold_Bga();
			temp_low = setting_GetLowThreshold_Bga();		
			break;
		
		case TYPE_CODuv:
			temp_high = setting_GetHighThreshold_COD();
			temp_low = setting_GetLowThreshold_COD();		
			break;

		case TYPE_MLSS:
			temp_high = setting_GetHighThreshold_MLSS();
			temp_low = setting_GetLowThreshold_MLSS();		
			break;

		case TYPE_Oiw:
			if(cur_DO.DO_list->modbus_id == OiW_yushan_ModbusID)
			{
				temp_high = setting_GetHighThreshold_OIW();
				temp_low = setting_GetLowThreshold_OIW();
			}
			else
			{
				temp_high = setting_GetHighThreshold_OIW_ppm();
				temp_low = setting_GetLowThreshold_OIW_ppm();
			}
			break;

		case TYPE_TSS:
			temp_high = setting_GetHighThreshold_TSS();
			temp_low = setting_GetLowThreshold_TSS();
			break;
		case TYPE_SAL:
			temp_high = setting_GetHighThreshold_SAL();
			temp_low = setting_GetLowThreshold_SAL();
			break;
		default:
			break;
	}		

	if(get_CurDo()->modbus_id == Bga_shenghui_ModbusID 
	|| get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID
	|| get_CurDo()->modbus_id == LH_DX01_ModbusID)
	{
		NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/100000),0, 9, IS_SINGLE, &NanoLow);//十万
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/10000)%10,0, 9, IS_SINGLE, &NanoLow);//万
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/1000)%10,0, 9, IS_SINGLE, &NanoLow);//千
		NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/100)%10, 0, 9, IS_SINGLE, &NanoLow);//百
		NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/10)%10, 0, 9, IS_SINGLE, &NanoLow);//十
		NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);//个
		temp_low *= 10;
		NanoOptionList_Add(144, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);//小数后一位
		LabelList_Add(136, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		OptionList_Add(0, (uint8_t *)dimenxianyuzhi_cn,  sizeof(dimenxianyuzhi_cn),  (uint8_t *)dimenxianyuzhi_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoLow, &option_head);	//报警音
		
		other_y += 16 + get_RowSpacing();
		NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)temp_high/100000,0, 9, IS_SINGLE,  &NanoHigh);//十万
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/10000)%10,0, 9, IS_SINGLE,  &NanoHigh);//万
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/1000)%10,0, 9, IS_SINGLE,  &NanoHigh);//千
		NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/100)%10, 0, 9, IS_SINGLE,  &NanoHigh);//百
		NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/10)%10, 0, 9, IS_SINGLE,  &NanoHigh);//十
		NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);//个
		temp_high *= 10;
		NanoOptionList_Add(144, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  ((uint32_t)temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);//小数后一位
		LabelList_Add(136, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		OptionList_Add(1, (uint8_t *)gaomenxianyuzhi_cn, sizeof(gaomenxianyuzhi_cn), (uint8_t *)gaomenxianyuzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoHigh, &option_head);	//报警音
		OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);//保存

	}
	else if(get_CurDo()->modbus_id == ORP_DR31_ModbusID)
	{
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, setting_GetOrp_low_Threshold_pn(),67, 68, IS_SINGLE, &NanoLow);//万
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/1000)%10,0, 9, IS_SINGLE, &NanoLow);//千
		NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/100)%10, 0, 9, IS_SINGLE, &NanoLow);//百
		NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low/10)%10, 0, 9, IS_SINGLE, &NanoLow);//十
		NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint32_t)(temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);//个
		OptionList_Add(0, (uint8_t *)dimenxianyuzhi_cn,  sizeof(dimenxianyuzhi_cn),  (uint8_t *)dimenxianyuzhi_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoLow, &option_head);	//报警音
		
		other_y += 16 + get_RowSpacing();
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  setting_GetOrp_high_Threshold_pn(),67, 68, IS_SINGLE,  &NanoHigh);//万
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/1000)%10,0, 9, IS_SINGLE,  &NanoHigh);//千
		NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/100)%10, 0, 9, IS_SINGLE,  &NanoHigh);//百
		NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high/10)%10, 0, 9, IS_SINGLE,  &NanoHigh);//十
		NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint32_t)(temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);//个
		OptionList_Add(1, (uint8_t *)gaomenxianyuzhi_cn, sizeof(gaomenxianyuzhi_cn), (uint8_t *)gaomenxianyuzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoHigh, &option_head);	//报警音
		OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);//保存

	}
	else
	{
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint16_t)temp_low/100,0, 9, IS_SINGLE, &NanoLow);//低门限
		temp_low /= 10.00;
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (uint16_t)temp_low%10,0, 9, IS_SINGLE, &NanoLow);
		temp_low *= 10;
		NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, ((uint16_t)temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);
		temp_low *= 10;
		NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, ((uint16_t)temp_low)%10, 0, 9, IS_SINGLE, &NanoLow);
		LabelList_Add(120, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		OptionList_Add(0, (uint8_t *)dimenxianyuzhi_cn,  sizeof(dimenxianyuzhi_cn),  (uint8_t *)dimenxianyuzhi_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoLow, &option_head);	//报警音
		
		other_y += 16 + get_RowSpacing();
		NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint16_t)temp_high/100,0, 9, IS_SINGLE,  &NanoHigh);//高门限
		temp_high /= 10.00;
		NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint16_t)temp_high%10,0, 9, IS_SINGLE,  &NanoHigh);
		temp_high *= 10;
		NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  ((uint16_t)temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);
		temp_high *= 10;
		NanoOptionList_Add(128, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  ((uint16_t)temp_high)%10, 0, 9, IS_SINGLE,  &NanoHigh);
		LabelList_Add(120, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		OptionList_Add(1, (uint8_t *)gaomenxianyuzhi_cn, sizeof(gaomenxianyuzhi_cn), (uint8_t *)gaomenxianyuzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoHigh, &option_head);	//报警音
		OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);//保存
	}
		
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



/*就是具体传感器点进去*/
void generate_Sensors(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;
	uint8_t* sensor_name = interfacial_GetOptionSensorName();
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	if(rs485_GetSensorType() == TYPE_ORP)
	{
		OptionList_Add(option_index++, (uint8_t *)chuanganqixiaozhun_cn, sizeof(chuanganqixiaozhun_cn), (uint8_t *)chuanganqixiaozhun_en,     PAGE_4_CAL,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
		OptionList_Add(option_index++, (uint8_t *)canshushezhi_cn,       sizeof(canshushezhi_cn),       (uint8_t *)chuanganqicanshushezhi_en, PAGE_4_PARASET,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	else if (rs485_GetSensorType() == TYPE_CODuv)
	{
		if(get_CurDo()->modbus_id == COD_DC17_ModbusID && get_CurDo()->DC17_Mes_Para.value_f == 2)//单次测量没有--弹窗提示
		{
			OptionList_Add(option_index++, (uint8_t *)chuanganqixiaozhun_cn, sizeof(chuanganqixiaozhun_cn), (uint8_t *)chuanganqixiaozhun_en,     NONE_PAGE,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
			OptionList_Add(option_index++, (uint8_t *)canshushezhi_cn,       sizeof(canshushezhi_cn),       (uint8_t *)chuanganqicanshushezhi_en, PAGE_4_PARASET,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
			OptionList_Add(option_index++, (uint8_t *)chuanganqixinxi_cn,    sizeof(chuanganqixinxi_cn),    (uint8_t *)chuanganqixinxi_en,        PAGE_4_SENSORINFO, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
		}	
		else
		{
			OptionList_Add(option_index++, (uint8_t *)chuanganqixiaozhun_cn, sizeof(chuanganqixiaozhun_cn), (uint8_t *)chuanganqixiaozhun_en,     PAGE_4_CAL,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
			OptionList_Add(option_index++, (uint8_t *)canshushezhi_cn,       sizeof(canshushezhi_cn),       (uint8_t *)chuanganqicanshushezhi_en, PAGE_4_PARASET,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
			OptionList_Add(option_index++, (uint8_t *)chuanganqixinxi_cn,    sizeof(chuanganqixinxi_cn),    (uint8_t *)chuanganqixinxi_en,        PAGE_4_SENSORINFO, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
		}
	}
	else
	{
		OptionList_Add(option_index++, (uint8_t *)chuanganqixiaozhun_cn, sizeof(chuanganqixiaozhun_cn), (uint8_t *)chuanganqixiaozhun_en,     PAGE_4_CAL,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
		OptionList_Add(option_index++, (uint8_t *)canshushezhi_cn,       sizeof(canshushezhi_cn),       (uint8_t *)chuanganqicanshushezhi_en, PAGE_4_PARASET,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
		OptionList_Add(option_index++, (uint8_t *)chuanganqixinxi_cn,    sizeof(chuanganqixinxi_cn),    (uint8_t *)chuanganqixinxi_en,        PAGE_4_SENSORINFO, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
	}

	switch(rs485_GetSensorType())
	{			
		case TYPE_pH:
	    OptionList_Add(option_index++, (uint8_t *)biaoyezuguanli_cn,       sizeof(biaoyezuguanli_cn),       (uint8_t *)biaoyezuguanli_en, PAGE_4_CAL_GROUP,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
			break;

		default:
			break;
	}			

	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_2_SENSORMANAGE;//设定父界面
	
	interfacial->content_chn = (uint8_t *)NULL;
	interfacial->ChnContent_size = 0;
	interfacial->content_eng = interfacial_GetOptionSensorName(); //将标签的字赋给界面
}


/*传感器界面里的参数设置*/
void generate_ParameterSetting(PtrToInterfacial interfacial)
{
	uint8_t index = 0;
	uint8_t* sensor_name = interfacial_GetOptionSensorName();
	
	set_RowSpacing(ROWSPACING_MORE);
	CurInterfacial_Destory();
	
	list_option option_head      = NULL;	
	switch(rs485_GetSensorType())
	{
			case TYPE_DO:	
			  	OptionList_Add(index++,  (uint8_t *)yandushezhi_cn,    sizeof(yandushezhi_cn),    (uint8_t *)yandushezhi_en,    PAGE_3_SALT,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//盐度设置
	      		OptionList_Add(index++,  (uint8_t *)qiyashezhi_cn,     sizeof(qiyashezhi_cn),     (uint8_t *)qiyashezhi_en,     PAGE_3_PRESSURE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//气压设置				
				break;
			
			case TYPE_pH:

				break;
			
			case TYPE_Tur:
				switch(get_CurDo()->modbus_id)
				{
					case ZS_DZ09_ModbusID:
						OptionList_Add(index++,  (uint8_t *)wendujingdushezhi_cn,     sizeof(wendujingdushezhi_cn),     (uint8_t *)wendujingdushezhi_en,     PAGE_3_Temp_jingdu,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);//温度小数设置
						break;
				}
				break;
			
			case TYPE_FCL:

				break;
			
			case TYPE_EC:
				switch(get_CurDo()->modbus_id)
				{
					case EC_DE26_ModbusID:  
					case EC_DE40_ModbusID:
						OptionList_Add(index++,  (uint8_t *)tds_trans_value_cn,    sizeof(tds_trans_value_cn),    (uint8_t *)tds_trans_value_en,    PAGE_3_Set_TDS_coefficient,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);		//tds系数设置
						OptionList_Add(index++,  (uint8_t *)wenduxishushezhi_cn,    sizeof(wenduxishushezhi_cn),    (uint8_t *)wenduxishushezhi_en,    PAGE_3_Set_Temp_coefficient,       OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//温度设置
					break;	
					
					default:
						break;
				}
				break;
			
			case TYPE_SAL:
				OptionList_Add(index++,  (uint8_t *)yandudanweishezhi_cn,     sizeof(yandudanweishezhi_cn),     (uint8_t *)yandudanweishezhi_en,     PAGE_3_Set_Sal_uint,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
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
	      OptionList_Add(index++,  (uint8_t *)liangchengshezhi_cn,     sizeof(liangchengshezhi_cn),     (uint8_t *)liangchengshezhi_en,     PAGE_4_Measure_Switch,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//量程设置		
				break;
			
			case TYPE_Bga:

				break;
			
			case TYPE_CODuv:
				
				switch(get_CurDo()->modbus_id)
				{
					
					case COD_DC17_ModbusID:  
	          OptionList_Add(index++,  (uint8_t *)celiangmoshi_cn,     sizeof(celiangmoshi_cn),     (uint8_t *)celiangmoshi_en,     PAGE_4_Mes_Set,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//测量模式
	          OptionList_Add(index++,  (uint8_t *)danciceliangshijianshezhi_cn,     sizeof(danciceliangshijianshezhi_cn),     (uint8_t *)danciceliangshijianshezhi_en,     PAGE_4_Mes_Time_Set,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//测量时间
						break;
					
					case COD_shenghui_ModbusID:  
	          OptionList_Add(index++,  (uint8_t *)dangqianxiguangdu_cn,     sizeof(dangqianxiguangdu_cn),     (uint8_t *)dangqianxiguangdu_en,     PAGE_4_Current_absorbance,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//当前吸光度
						break;	
					
					default:
						break;
				}

			
				break;

			case TYPE_MLSS:
	      		OptionList_Add(index++,  (uint8_t *)celiangdanweishezhi_cn,     sizeof(celiangdanweishezhi_cn),     (uint8_t *)celiangdanweishezhi_en,     PAGE_4_MesUNIT,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	      		OptionList_Add(index++,  (uint8_t *)biaodingdianshushezhi_cn,     sizeof(biaodingdianshushezhi_cn),     (uint8_t *)biaodingdianshushezhi_en,     PAGE_4_Cal_Num_Set,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	      		OptionList_Add(index++,  (uint8_t *)lingdianshezhi_cn,     sizeof(lingdianshezhi_cn),     (uint8_t *)lingdianshezhi_en,     PAGE_5_DE26_EC_Zero,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	      		OptionList_Add(index++,  (uint8_t *)xielvshezhi_cn,     sizeof(xielvshezhi_cn),     (uint8_t *)xielvshezhi_en,     PAGE_5_DR31_ORP_ONE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
				break;

			case TYPE_Oiw:
				switch(get_CurDo()->modbus_id)
				{
					case OiW_guohong_ModbusID:
						OptionList_Add(index++,  (uint8_t *)wendubuchangxishushezhi_cn,     sizeof(wendubuchangxishushezhi_cn),     (uint8_t *)wendubuchangxishushezhi_en,     PAGE_4_Temp_k,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
						OptionList_Add(index++,  (uint8_t *)celiangdangweishezhi_cn,     sizeof(celiangdangweishezhi_cn),     (uint8_t *)celiangdangweishezhi_en,     PAGE_4_Mes_Mode,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
						OptionList_Add(index++,  (uint8_t *)qingjiezhouqishezhi_cn,     sizeof(qingjiezhouqishezhi_cn),     (uint8_t *)qingjiezhouqishezhi_en,     PAGE_4_Clear_time,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
						OptionList_Add(index++,  (uint8_t *)wendumoshishezhi_cn,     sizeof(wendumoshishezhi_cn),     (uint8_t *)wendumoshishezhi_en,     PAGE_4_Set_Mode_Temp,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
						OptionList_Add(index++,  (uint8_t *)jiaozhunxishu_cn,     sizeof(jiaozhunxishu_cn),     (uint8_t *)jiaozhunxishu_en,     PAGE_4_Cal_Info,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
						
						OptionList_Add(index++, (uint8_t *)biaoyejiaozhun_one_cn,   sizeof(biaoyejiaozhun_one_cn),   (uint8_t *)biaoyejiaozhun_one_en,      PAGE_5_shenghui_Tur_ONE,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
						OptionList_Add(index++, (uint8_t *)biaoyejiaozhun_Two_cn,  	sizeof(biaoyejiaozhun_Two_cn),   (uint8_t *)biaoyejiaozhun_Two_en,      PAGE_5_shenghui_Tur_TWO,      	OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
						OptionList_Add(index++, (uint8_t *)biaoyejiaozhun_Three_cn, sizeof(biaoyejiaozhun_Three_cn), (uint8_t *)biaoyejiaozhun_Three_en,    PAGE_5_shenghui_Tur_THREE,      	OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	
						break;
					case OiW_yushan_ModbusID:
					case OiW_yushan_DA511_ModbusID:
						OptionList_Add(index++,  (uint8_t *)jiaozhunxishu_cn,     sizeof(jiaozhunxishu_cn),     (uint8_t *)jiaozhunxishu_en,     PAGE_4_Cal_Info,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);

						break;
					default:
						break;
				}
				break;
			case TYPE_TSS:
				OptionList_Add(index++,  (uint8_t *)zidongqingxijiange_cn,     sizeof(zidongqingxijiange_cn),     (uint8_t *)zidongqingxijiange_en,     PAGE_4_Clear_time,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
				OptionList_Add(index++,  (uint8_t *)wendujingdushezhi_cn,     sizeof(wendujingdushezhi_cn),     (uint8_t *)wendujingdushezhi_en,     PAGE_4_Temp_xiaoshu,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
				// OptionList_Add(index++,  (uint8_t *)lvbocishu_cn,     sizeof(lvbocishu_cn),     (uint8_t *)lvbocishu_en,     PAGE_4_SENSOR_SLIDEAVG,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
				break;
			default:
				break;
	}
	
	OptionList_Add(index++,  (uint8_t *)baojingshezhi_cn,  sizeof(baojingshezhi_cn),  (uint8_t *)baojingshezhi_en,  PAGE_4_ALARM, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//报警设置
	OptionList_Add(index++,  (uint8_t *)lvboshezhi_cn, sizeof(lvboshezhi_cn), (uint8_t *)lvboshezhi_en, PAGE_4_SLIDEAVG,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//滑动平均
	OptionList_Add(index++,  (uint8_t *)dushusuoding_cn,   sizeof(dushusuoding_cn),   (uint8_t *)dushusuoding_en,   PAGE_4_AUTOLOCK,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//读书锁定	
	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_3_SENSORS;//设定父界面
	
	interfacial->content_chn = (uint8_t *)canshushezhi_cn ;
	interfacial->ChnContent_size = sizeof(canshushezhi_cn);
	interfacial->content_eng = (uint8_t *)chuanganqicanshushezhi_en;
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
	

	PtrToDOProbe probe_do = DO_FindByName(sensor_name, rs485_GetDoList());

	LabelList_Add( 32, other_y, NULL, 0, (uint8_t *)probe_do->SN,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//SN
	
	other_y += 16 + get_RowSpacing();
	LabelList_Add( 96, other_y, NULL, 0, (uint8_t *)probe_do->HWV, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//HW
	
	other_y += 16 + get_RowSpacing();
	LabelList_Add( 96, other_y, NULL, 0, (uint8_t *)probe_do->SWV, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//SW
	
	other_y += 16 + get_RowSpacing();

	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, ((probe_do->modbus_id)/10), 0, 9, IS_SINGLE, &NanoModbusId);//modbusid   F0
	NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, (probe_do->modbus_id)%10, 0, 9, IS_SINGLE, &NanoModbusId);
	
	OptionList_Add(0, (uint8_t *)NULL,              0,                         (uint8_t *)chuchangxulie_en,  NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY,  NULL,         &option_head);//SN
	OptionList_Add(1, (uint8_t *)yingjianbanben_cn, sizeof(yingjianbanben_cn), (uint8_t *)yingjianbanben_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);//HW
	OptionList_Add(2, (uint8_t *)ruanjianbanben_cn, sizeof(ruanjianbanben_cn), (uint8_t *)ruanjianbanben_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);//SW
	OptionList_Add(3, (uint8_t *)NULL,              0,                         (uint8_t *)dizhi_en,          NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED,    NOT_LANGUAGE_OPTION, IS_ENG_ONLY,  NanoModbusId, &option_head);//ID
	//OptionList_Add(4, (uint8_t *)baocun_cn,         sizeof(baocun_cn),         (uint8_t *)baocun_en,         NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);//保存
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_3_SENSORS;//设定父界面
	
	interfacial->content_chn = (uint8_t *)chuanganqixinxi_cn ;
	interfacial->ChnContent_size = sizeof(chuanganqixinxi_cn);
	interfacial->content_eng = (uint8_t *)chuanganqixinxi_en;
}


void generate_Sensor_Cal_Info(PtrToInterfacial interfacial)
{
	uint8_t other_y;
	static char Cal_Info[7];
	static char Cal_Info1[7];
	static char Cal_Info2[7];
	static char Cal_Info3[7];
	static char Cal_Info4[7];
	uint8_t* sensor_name = interfacial_GetOptionSensorName();
	
	set_RowSpacing(ROWSPACING_FIVE);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head      = NULL;
	list_NanoOption NanoModbusId = NULL;   //子选项  就一串数字
	list_label label_head        = NULL;      //标签项  显示单位 实时数值
	
	PtrToDOProbe probe_do = DO_FindByName(sensor_name, rs485_GetDoList());

	//国宏
	if(probe_do->modbus_id == OiW_guohong_ModbusID)
	{
		//水中油b值
		snprintf(Cal_Info,       7, "%6.1f", probe_do->NH4_Vol.value_f);
		
		//温度b值
		snprintf(Cal_Info1,       7, "%6.1f", probe_do->pH_Vol.value_f);

		//水中油零点系数
		snprintf(Cal_Info2,       7, "%6.1f", probe_do->compensate_k.value_f);

		//水中油斜率系数
		snprintf(Cal_Info3,       7, "%6.1f", probe_do->compensate_b.value_f);

		//水中油k系数
		snprintf(Cal_Info4,       7, "%6.1f", probe_do->press.value_f);

		//空白电位值
		// ptd->DOpercent_Vol_arr

		LabelList_Add( 56, other_y, NULL, 0, (uint8_t *)K_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)Cal_Info4,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		other_y += 16 + get_RowSpacing();
		LabelList_Add( 56, other_y, NULL, 0, (uint8_t *)B_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)Cal_Info,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
		other_y += 16 + get_RowSpacing();
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)Cal_Info2, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
		other_y += 16 + get_RowSpacing();
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)Cal_Info3, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		other_y += 16 + get_RowSpacing();
		LabelList_Add( 56, other_y, NULL, 0, (uint8_t *)B_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)Cal_Info1,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
		other_y += 16 + get_RowSpacing();
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)probe_do->DOpercent_Vol_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		OptionList_Add(0, (uint8_t *)Oiw_cn,              sizeof(Oiw_cn),          (uint8_t *)Oiw_en,  NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY,  NULL,         &option_head);
		OptionList_Add(1, (uint8_t *)Oiw_cn,              sizeof(Oiw_cn),          (uint8_t *)Oiw_en,  NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY,  NULL,         &option_head);
		OptionList_Add(2, (uint8_t *)lingdianxishu_cn, sizeof(lingdianxishu_cn), (uint8_t *)lingdianxishu_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);
		OptionList_Add(3, (uint8_t *)xielvxishu_cn, sizeof(xielvxishu_cn), (uint8_t *)xielvxishu_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,         &option_head);
		OptionList_Add(4, (uint8_t *)wendu_cn,     sizeof(wendu_cn),             (uint8_t *)wendu_en,          NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY,  NanoModbusId, &option_head);
		OptionList_Add(5, (uint8_t *)kongbaidianweizhi_cn,     sizeof(kongbaidianweizhi_cn),             (uint8_t *)kongbaidianweizhi_en,          NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY,  NanoModbusId, &option_head);
	}
	else if(probe_do->modbus_id == OiW_yushan_ModbusID||probe_do->modbus_id == OiW_yushan_DA511_ModbusID)
	{
		//水中油b值
		// probe_do->DOmgl_Vol_arr
		//水中油k系数
		// probe_do->tocmgl_Vol_arr

		LabelList_Add( 56, other_y, NULL, 0, (uint8_t *)K_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)probe_do->tocmgl_Vol_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		other_y += 16 + get_RowSpacing();
		LabelList_Add( 56, other_y, NULL, 0, (uint8_t *)B_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		LabelList_Add( 102, other_y, NULL, 0, (uint8_t *)probe_do->DOmgl_Vol_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
		OptionList_Add(0, (uint8_t *)Oiw_cn,              sizeof(Oiw_cn),          (uint8_t *)Oiw_en,  NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY,  NULL,         &option_head);
		OptionList_Add(1, (uint8_t *)Oiw_cn,              sizeof(Oiw_cn),          (uint8_t *)Oiw_en,  NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY,  NULL,         &option_head);

	}



	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)jiaozhunxishu_cn ;
	interfacial->ChnContent_size = sizeof(jiaozhunxishu_cn);
	interfacial->content_eng = (uint8_t *)jiaozhunxishu_en;
}

void generate_cal_DO(PtrToInterfacial interfacial, list_option* option_head, manufacturer_enum factory)
{//校准界面
	uint8_t option_index = 0;//标签下标

	switch(rs485_GetSensorType())
		{
			case TYPE_DO:
					
				if(factory == manufacturer_hyphive)
				{//海发的显示温度校准
					OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				}
				OptionList_Add(option_index++, (uint8_t *)manyang_cn,   sizeof(manyang_cn),   (uint8_t *)manyang_en,       PAGE_5_DO_ONE_First,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				if(get_CurDo()->modbus_id == DO_HF_DY12_ModbusID)
				{
					OptionList_Add(option_index++, (uint8_t *)lingdian_cn, sizeof(lingdian_cn), (uint8_t *)lingdian_en,     PAGE_5_DO_TWO_FIRST, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				}
				else
				{
					OptionList_Add(option_index++, (uint8_t *)liangdian_cn, sizeof(liangdian_cn), (uint8_t *)liangdian_en,     PAGE_5_DO_TWO_FIRST, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				}
				interfacial->content_chn = (uint8_t *)rongjieyangjiaozhun_cn;
				interfacial->ChnContent_size = sizeof(rongjieyangjiaozhun_cn);
				interfacial->content_eng = (uint8_t *)rongjieyangjiaozhun_en;								
				break;
			
			case TYPE_pH:
				OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
			  if(!setting_GetIs_pH_Group()){
					OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun1_cn,   sizeof(andan_pH_jiaozhun1_cn),   (uint8_t *)andan_pH_jiaozhun1_en,       PAGE_5_NH3N_pH_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun2_cn,   sizeof(andan_pH_jiaozhun2_cn),   (uint8_t *)andan_pH_jiaozhun2_en,       PAGE_5_NH3N_pH_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun3_cn,   sizeof(andan_pH_jiaozhun3_cn),   (uint8_t *)andan_pH_jiaozhun3_en,       PAGE_5_NH3N_pH_THREE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				}else{
					OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun1_USA_cn,   sizeof(andan_pH_jiaozhun1_USA_cn),   (uint8_t *)andan_pH_jiaozhun1_USA_en,       PAGE_5_NH3N_pH_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun2_USA_cn,   sizeof(andan_pH_jiaozhun2_USA_cn),   (uint8_t *)andan_pH_jiaozhun2_USA_en,       PAGE_5_NH3N_pH_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun3_USA_cn,   sizeof(andan_pH_jiaozhun3_USA_cn),   (uint8_t *)andan_pH_jiaozhun3_USA_en,       PAGE_5_NH3N_pH_THREE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));					
				}

				
				interfacial->content_chn = (uint8_t *)pH_jiaozhun_cn;
				interfacial->ChnContent_size = sizeof(pH_jiaozhun_cn);
				interfacial->content_eng = (uint8_t *)pH_jiaozhun_en;
				break;
			
			case TYPE_Tur:
				switch (get_CurDo()->modbus_id)
				{
				case ZS_DZ09_ModbusID:
					OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
					OptionList_Add(option_index++, (uint8_t *)lingdian_cn,   sizeof(lingdian_cn),   (uint8_t *)lingdian_en,       PAGE_5_shenghui_Tur_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)xielvjiaozhun_cn,   sizeof(xielvjiaozhun_cn),   (uint8_t *)xielvjiaozhun_en,       PAGE_5_shenghui_Tur_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					interfacial->content_chn = (uint8_t *)zhuodu_jiaozhun_cn;
					interfacial->ChnContent_size = sizeof(zhuodu_jiaozhun_cn);
					interfacial->content_eng = (uint8_t *)zhuodu_jiaozhun_en;
					break;
				
				default:
					OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_jiaozhun1_cn,   sizeof(COD_zhuodu_jiaozhun1_cn),   (uint8_t *)COD_zhuodu_jiaozhun1_en,       PAGE_5_shenghui_Tur_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_jiaozhun2_cn,   sizeof(COD_zhuodu_jiaozhun2_cn),   (uint8_t *)COD_zhuodu_jiaozhun2_en,       PAGE_5_shenghui_Tur_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_jiaozhun3_cn,   sizeof(COD_zhuodu_jiaozhun3_cn),   (uint8_t *)COD_zhuodu_jiaozhun3_en,       PAGE_5_shenghui_Tur_THREE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					interfacial->content_chn = (uint8_t *)zhuodu_jiaozhun_cn;
					interfacial->ChnContent_size = sizeof(zhuodu_jiaozhun_cn);
					interfacial->content_eng = (uint8_t *)zhuodu_jiaozhun_en;
					break;
				}
				break;
			
			case TYPE_FCL:

				break;
			
			case TYPE_EC:
				OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
			
				switch(get_CurDo()->modbus_id)
				{								
					case EC_DE40_ModbusID:		
					case EC_DE26_ModbusID:  
					  OptionList_Add(option_index++, (uint8_t *)COD_Zero_cn,   sizeof(COD_Zero_cn),   (uint8_t *)COD_Zero_en,       PAGE_5_DE26_EC_Zero,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
					  OptionList_Add(option_index++, (uint8_t *)dandian_cn,   sizeof(dandian_cn),   (uint8_t *)dandian_en,       PAGE_5_shenghui_EC_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						break;	
					
					case EC_shenghui_ModbusID:  
					 OptionList_Add(option_index++, (uint8_t *)diandaolv_cn,   sizeof(diandaolv_cn),   (uint8_t *)diandaolv_en,       PAGE_5_shenghui_EC_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						break;	
					
					default:
						break;

				}

				interfacial->content_chn = (uint8_t *)diandaolv_jiaozhun_cn;
				interfacial->ChnContent_size = sizeof(diandaolv_jiaozhun_cn);
				interfacial->content_eng = (uint8_t *)diandaolv_jiaozhun_en;
				break;
			
			case TYPE_SAL:
				OptionList_Add(option_index++, (uint8_t *)wendujiaozhun_cn,     sizeof(wendujiaozhun_cn),     (uint8_t *)wendujiaozhun_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)yandujiaozhun_cn,     sizeof(yandujiaozhun_cn),     (uint8_t *)yandujiaozhun_en, PAGE_5_shenghui_EC_ONE,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));

				break;

			case TYPE_ORP:
				OptionList_Add(option_index++, (uint8_t *)dandian_cn,   sizeof(dandian_cn),   (uint8_t *)dandian_en,       PAGE_5_DR31_ORP_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				break;
			
			case TYPE_NH4:
				OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)andan_jiaozhun1_cn,   sizeof(andan_jiaozhun1_cn),   (uint8_t *)andan_jiaozhun1_en,       PAGE_5_NH3N_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)andan_jiaozhun2_cn,   sizeof(andan_jiaozhun2_cn),   (uint8_t *)andan_jiaozhun2_en,       PAGE_5_NH3N_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun1_cn,   sizeof(andan_pH_jiaozhun1_cn),   (uint8_t *)andan_pH_jiaozhun1_en,       PAGE_5_NH3N_pH_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun2_cn,   sizeof(andan_pH_jiaozhun2_cn),   (uint8_t *)andan_pH_jiaozhun2_en,       PAGE_5_NH3N_pH_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				OptionList_Add(option_index++, (uint8_t *)andan_pH_jiaozhun3_cn,   sizeof(andan_pH_jiaozhun3_cn),   (uint8_t *)andan_pH_jiaozhun3_en,       PAGE_5_NH3N_pH_THREE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				
				interfacial->content_chn = (uint8_t *)andan_jiaozhun_cn;
				interfacial->ChnContent_size = sizeof(andan_jiaozhun_cn);
				interfacial->content_eng = (uint8_t *)andan_jiaozhun_en;
				break;
			
			case TYPE_F:

				break;
			
			case TYPE_CL:

				break;
			
			case TYPE_Chl:
				OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)COD_Zero_cn,   sizeof(COD_Zero_cn),   (uint8_t *)COD_Zero_en,       PAGE_5_DE26_EC_Zero,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				OptionList_Add(option_index++, (uint8_t *)gaodian_cn,   sizeof(gaodian_cn),   (uint8_t *)gaodian_en,       PAGE_5_shenghui_EC_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
			
				interfacial->content_chn = (uint8_t *)Chl_jiaozhun_cn;
				interfacial->ChnContent_size = sizeof(Chl_jiaozhun_cn);
				interfacial->content_eng = (uint8_t *)Chl_jiaozhun_en;
				break;
			
			case TYPE_Bga:
				OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)lingdianjiaozhun_cn,   sizeof(lingdianjiaozhun_cn),   (uint8_t *)lingdianjiaozhun_en,       PAGE_5_shenghui_BGA_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)xielvjiaozhun_cn,   sizeof(xielvjiaozhun_cn),   (uint8_t *)xielvjiaozhun_en,       PAGE_5_shenghui_BGA_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
				
				interfacial->content_chn = (uint8_t *)Bga_cn;
				interfacial->ChnContent_size = sizeof(Bga_cn);
				interfacial->content_eng = (uint8_t *)Bga_en;
				break;
			
			case TYPE_CODuv:
				
				switch(get_CurDo()->modbus_id)
				{
					
					case COD_DC18_ModbusID:  
					case COD_DC17_ModbusID:
						OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						if(setting_GetIsKey_DC18_Set())
						{			  
							OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_zero_cn,   sizeof(COD_zhuodu_zero_cn),   (uint8_t *)COD_zhuodu_zero_en,       PAGE_5_COD_DC18_Tur_Zero,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
							OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_slope_cn,   sizeof(COD_zhuodu_slope_cn),   (uint8_t *)COD_zhuodu_slope_en,       PAGE_5_COD_DC18_Tur_Slope,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));						
						}
						OptionList_Add(option_index++, (uint8_t *)COD_zero_cn,   sizeof(COD_zero_cn),   (uint8_t *)COD_zero_en,       PAGE_5_COD_DC18_Zero,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						OptionList_Add(option_index++, (uint8_t *)COD_slope_cn,   sizeof(COD_slope_cn),   (uint8_t *)COD_slope_en,       PAGE_5_COD_DC18_Slope,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						if(setting_GetIsKey_DC18_Set())
						{			  
						OptionList_Add(option_index++, (uint8_t *)COD_Coefficient_cn,   sizeof(COD_Coefficient_cn),   (uint8_t *)COD_Coefficient_en,       PAGE_5_COD_DC18_Coefficient,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));					
						}
						//OptionList_Add(option_index++, (uint8_t *)Cleanse_cn,   sizeof(Cleanse_cn),   (uint8_t *)Cleanse_en,       PAGE_5_COD_Cleanse,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));		
						break;
					
					case COD_shenghui_ModbusID:  
						OptionList_Add(option_index++, (uint8_t *)wendu_cn,     sizeof(wendu_cn),     (uint8_t *)wendu_en, PAGE_5_TEMP,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						OptionList_Add(option_index++, (uint8_t *)COD_Zero_cn,   sizeof(COD_Zero_cn),   (uint8_t *)COD_Zero_en,       PAGE_5_COD_shenghui_Zero,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));		
						OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_jiaozhun1_cn,   sizeof(COD_zhuodu_jiaozhun1_cn),   (uint8_t *)COD_zhuodu_jiaozhun1_en,       PAGE_5_COD_shenghui_Tur_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_jiaozhun2_cn,   sizeof(COD_zhuodu_jiaozhun2_cn),   (uint8_t *)COD_zhuodu_jiaozhun2_en,       PAGE_5_COD_shenghui_Tur_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						OptionList_Add(option_index++, (uint8_t *)COD_jiaozhun1_cn,   sizeof(COD_jiaozhun1_cn),   (uint8_t *)COD_jiaozhun1_en,       PAGE_5_COD_shenghui_ONE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						OptionList_Add(option_index++, (uint8_t *)COD_jiaozhun2_cn,   sizeof(COD_jiaozhun2_cn),   (uint8_t *)COD_jiaozhun2_en,       PAGE_5_COD_shenghui_TWO,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						OptionList_Add(option_index++, (uint8_t *)COD_jiaozhun3_cn,   sizeof(COD_jiaozhun3_cn),   (uint8_t *)COD_jiaozhun3_en,       PAGE_5_COD_shenghui_THREE,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));	
						OptionList_Add(option_index++, (uint8_t *)Cleanse_cn,   sizeof(Cleanse_cn),   (uint8_t *)Cleanse_en,       PAGE_5_COD_Cleanse,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));		
						break;	
					
					default:
						break;

				}

			
				interfacial->content_chn = (uint8_t *)COD_jiaozhun_cn;
				interfacial->ChnContent_size = sizeof(COD_jiaozhun_cn);
				interfacial->content_eng = (uint8_t *)COD_jiaozhun_en;
				break;

			case TYPE_MLSS:
				OptionList_Add(option_index++, (uint8_t *)lingdian_cn,   sizeof(lingdian_cn),   (uint8_t *)lingdian_en,       PAGE_5_MLSS_zero_signal,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)diyidian_cn,   sizeof(diyidian_cn),   (uint8_t *)diyidian_en,       PAGE_5_DO_ONE_First,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				if(get_CurDo()->compensate_b.value_f == 3)
				{
					OptionList_Add(option_index++, (uint8_t *)dierdian_cn,   sizeof(dierdian_cn),   (uint8_t *)dierdian_en,       PAGE_5_DO_TWO_FIRST,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				}

				interfacial->content_chn = (uint8_t *)MLSS_cn;
				interfacial->ChnContent_size = sizeof(MLSS_cn);
				interfacial->content_eng = (uint8_t *)MLSS_en;
				break;

			case TYPE_Oiw:
				switch(get_CurDo()->modbus_id)
				{									
					case OiW_yushan_ModbusID:  
					case OiW_yushan_DA511_ModbusID:
						OptionList_Add(option_index++, (uint8_t *)dandianjiaozhun_cn,   	sizeof(dandianjiaozhun_cn),   	(uint8_t *)dandianjiaozhun_en,       	PAGE_5_DR31_ORP_ONE,      	OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						OptionList_Add(option_index++, (uint8_t *)xielvjiaozhun_cn,   		sizeof(xielvjiaozhun_cn),   	(uint8_t *)xielvjiaozhun_en,       		PAGE_5_OIW_K_CAL,      		OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						break;	
					
					case OiW_guohong_ModbusID:
						interfacial_SetOptionSensorName((uint8_t *)Oiw_en);
						OptionList_Add(option_index++, (uint8_t *)lingdianjiaozhun_cn,		sizeof(lingdianjiaozhun_cn),   	(uint8_t *)lingdianjiaozhun_en,      	PAGE_5_MLSS_zero_signal,  	OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						OptionList_Add(option_index++, (uint8_t *)dandianjiaozhun_cn,   	sizeof(dandianjiaozhun_cn),   	(uint8_t *)dandianjiaozhun_en,       	PAGE_5_DR31_ORP_ONE,      	OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						OptionList_Add(option_index++, (uint8_t *)xielvjiaozhun_cn,   		sizeof(xielvjiaozhun_cn),   	(uint8_t *)xielvjiaozhun_en,       		PAGE_5_OIW_K_CAL,      		OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
						break;
					default:
						break;

				}
								
				interfacial->content_chn = (uint8_t *)Oiw_cn;
				interfacial->ChnContent_size = sizeof(Oiw_cn);
				interfacial->content_eng = (uint8_t *)Oiw_en;
				break;

			case TYPE_TSS:
				OptionList_Add(option_index++, (uint8_t *)wendu_cn,   sizeof(wendu_cn),   (uint8_t *)wendu_en,       PAGE_5_TEMP,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)lingdianjiaozhun_cn,   sizeof(lingdianjiaozhun_cn),   (uint8_t *)lingdianjiaozhun_en,       PAGE_5_MLSS_zero_signal,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));
				OptionList_Add(option_index++, (uint8_t *)xielvjiaozhun_cn,   sizeof(xielvjiaozhun_cn),   (uint8_t *)xielvjiaozhun_en,       PAGE_5_OIW_K_CAL,      OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &(*option_head));

				interfacial->content_chn = (uint8_t *)xuanfuwu_cn;
				interfacial->ChnContent_size = sizeof(xuanfuwu_cn);
				interfacial->content_eng = (uint8_t *)xuanfuwu_en;
				break;
			
			default:
				break;
			
		}
		interfacial->content_chn = (uint8_t *)chuanganqixiaozhun_cn;
		interfacial->ChnContent_size = sizeof(chuanganqixiaozhun_cn);
		interfacial->content_eng = (uint8_t *)chuanganqixiaozhun_en;		

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
			PtrToDOProbe probe_do = DO_FindByName(sensor_name, rs485_GetDoList());//通过名字查找设备
			generate_cal_DO(interfacial, &option_head, probe_do->manufacturer);//将厂商带入
			
			break;
		
		case PAGE_0_START:
	  	generate_cal_DO(interfacial, &option_head, get_CurDo()->manufacturer);
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
		
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值 直接显示100.0%
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
			break;

		case TYPE_MLSS:
			LabelList_Add( 0, 36, (uint8_t *)diyidian_cn, sizeof(diyidian_cn), (uint8_t *)diyidian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点

			if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
			{
				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
			else if(setting_GetMesUnit() == 5)
			{
				LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
			else
			{
				LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(108, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
			break;

		default:
			break;
	}
	
	if(sensor_type == TYPE_MLSS)
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
		
		interfacial->content_chn = (uint8_t *)diyidian_cn;
		interfacial->ChnContent_size = sizeof(diyidian_cn);
		interfacial->content_eng = (uint8_t *)diyidian_en;
	}
	else if(sensor_type == TYPE_DO)
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
		
		interfacial->content_chn = (uint8_t *)manyang_cn;
		interfacial->ChnContent_size = sizeof(manyang_cn);
		interfacial->content_eng = (uint8_t *)manyang_en;
	}
	else
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
		
		interfacial->content_chn = (uint8_t *)dandian_cn;
		interfacial->ChnContent_size = sizeof(dandian_cn);
		interfacial->content_eng = (uint8_t *)dandian_en;
	}
}

void generate_MLSS_ZERO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;
	
	switch (sensor_type)
	{
		case TYPE_MLSS:
			LabelList_Add( 0, 36, (uint8_t *)lingdian_cn, sizeof(lingdian_cn), (uint8_t *)lingdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点

			if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
			{
				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
			else if(setting_GetMesUnit() == 5)
			{
				LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
			else
			{
				LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(108, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
			break;

		case TYPE_Oiw:
			LabelList_Add( 0, 36, (uint8_t *)lingdian_cn, sizeof(lingdian_cn), (uint8_t *)lingdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点

			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			LabelList_Add(60, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);

			break;

		case TYPE_TSS:
			LabelList_Add( 0, 36, (uint8_t *)lingdian_cn, sizeof(lingdian_cn), (uint8_t *)lingdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点

			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);

			break;
		default:
			break;
	}
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	if(sensor_type == TYPE_Oiw)
	{
		interfacial->content_chn = (uint8_t *)lingdianjiaozhun_cn;
		interfacial->ChnContent_size = sizeof(lingdianjiaozhun_cn);
		interfacial->content_eng = (uint8_t *)lingdianjiaozhun_en;
	}
	else
	{
		interfacial->content_chn = (uint8_t *)lingdian_cn;
		interfacial->ChnContent_size = sizeof(lingdian_cn);
		interfacial->content_eng = (uint8_t *)lingdian_en;
	}
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
			if(get_CurDo()->modbus_id == DO_HF_DY12_ModbusID)
			{
				LabelList_Add( 0, 36, (uint8_t *)lingdian_cn, sizeof(lingdian_cn), (uint8_t *)lingdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点
			}
			else
			{
				LabelList_Add( 0, 36, (uint8_t *)diyidian_cn, sizeof(diyidian_cn), (uint8_t *)diyidian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点
			}
			LabelList_Add(100, 56, NULL, 0, (uint8_t *)BAIFENGHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
			break;

		case TYPE_MLSS:
			LabelList_Add( 0, 36, (uint8_t *)dierdian_cn, sizeof(dierdian_cn), (uint8_t *)dierdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点

			if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
			{
				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
			else if(setting_GetMesUnit() == 5)
			{
				LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
			else
			{
				LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

				NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
				NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
				NanoOptionList_Add(108, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

			}
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
			break;
		default:
			break;
	}
	
	if(sensor_type == TYPE_MLSS)
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
		
		interfacial->content_chn = (uint8_t *)dierdian_cn;
		interfacial->ChnContent_size = sizeof(dierdian_cn);
		interfacial->content_eng = (uint8_t *)dierdian_en;
	}
	else if((sensor_type == TYPE_DO) && (get_CurDo()->modbus_id == DO_HF_DY12_ModbusID))
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
		
		interfacial->content_chn = (uint8_t *)lingdian_cn;
		interfacial->ChnContent_size = sizeof(lingdian_cn);
		interfacial->content_eng = (uint8_t *)lingdian_en;
	}
	else
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
		
		interfacial->content_chn = (uint8_t *)liangdian_cn;
		interfacial->ChnContent_size = sizeof(liangdian_cn);
		interfacial->content_eng = (uint8_t *)liangdian_en;
	}
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
			
		
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
			OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
			break;

		case TYPE_MLSS:
			LabelList_Add( 0, 36, (uint8_t *)dierdian_cn, sizeof(dierdian_cn), (uint8_t *)dierdian_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第一点

			LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			NanoOptionList_Add(44, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		
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
	
	//OptionList_Add(0, (uint8_t *)shujubaocun_cn,  sizeof(shujubaocun_cn),  (uint8_t *)shujubaocun_en,  PAGE_3_DATASAVE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)shujuxianshi_cn, sizeof(shujuxianshi_cn), (uint8_t *)shujuxianshi_en, PAGE_2_HISTORY_ShowOPTION,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, (uint8_t *)shujushanchu_cn, sizeof(shujushanchu_cn), (uint8_t *)shujushanchu_en, PAGE_2_HISTORY_DeleteOPTION, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);

	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_1_MENU;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lishishuju_cn;
	interfacial->ChnContent_size = sizeof(lishishuju_cn);
	interfacial->content_eng = (uint8_t *)lishishuju_en; //将标签的字赋给界面
}

/*  根据电极型号选择查看历史数据界面*/
void generate_Histor_ShowOption(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;//标签下标
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
  datashow_SensorType=TYPE_NONE;	
	if(log_GetLogCount(TYPE_DO)	> 0)
	{ 
		datashow_SensorType=TYPE_DO;	
	  OptionList_Add(option_index++, (uint8_t *)rongjieyang_cn,  sizeof(rongjieyang_cn),  (uint8_t *)rongjieyang_en,  PAGE_3_DATASHOW,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
  if(log_GetLogCount(TYPE_pH)	> 0) 
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_pH;
	  OptionList_Add(option_index++, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en, PAGE_3_DATASHOW,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_Tur)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Tur;
	  OptionList_Add(option_index++, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_FCL)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_FCL;
	  OptionList_Add(option_index++, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);		
	}
	if(log_GetLogCount(TYPE_EC)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_EC;
	  OptionList_Add(option_index++, (uint8_t *)diandaolv_cn, sizeof(diandaolv_cn), (uint8_t *)diandaolv_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_ORP)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
    datashow_SensorType=TYPE_ORP;		
	  OptionList_Add(option_index++, (uint8_t *)ORP_cn, sizeof(ORP_cn), (uint8_t *)ORP_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
  }
	if(log_GetLogCount(TYPE_NH4)	> 0)	
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_NH4;
	  OptionList_Add(option_index++, (uint8_t *)andan_cn, sizeof(andan_cn), (uint8_t *)andan_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  
	}
	if(log_GetLogCount(TYPE_F)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_F;
	  OptionList_Add(option_index++, (uint8_t *)F_cn, sizeof(F_cn), (uint8_t *)F_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  }
	if(log_GetLogCount(TYPE_CL)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_CL;
		OptionList_Add(option_index++, (uint8_t *)Oiw_ppm_cn, sizeof(Oiw_ppm_cn), (uint8_t *)Oiw_ppm_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	//   OptionList_Add(option_index++, (uint8_t *)CL_cn, sizeof(CL_cn), (uint8_t *)CL_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
  }
	if(log_GetLogCount(TYPE_Chl)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Chl;
		OptionList_Add(option_index++, (uint8_t *)Chl_cn, sizeof(Chl_cn), (uint8_t *)Chl_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  }
	if(log_GetLogCount(TYPE_Bga)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Bga;
		OptionList_Add(option_index++, (uint8_t *)Bga_cn, sizeof(Bga_cn), (uint8_t *)Bga_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  }
	if(log_GetLogCount(TYPE_CODuv)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_CODuv;
		OptionList_Add(option_index++, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_MLSS)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType = TYPE_MLSS;
		OptionList_Add(option_index++, (uint8_t *)MLSS_cn, sizeof(MLSS_cn), (uint8_t *)MLSS_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_Oiw)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType = TYPE_Oiw;
		OptionList_Add(option_index++, (uint8_t *)Oiw_ppb_cn, sizeof(Oiw_ppb_cn), (uint8_t *)Oiw_ppb_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_TSS)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType = TYPE_TSS;
		OptionList_Add(option_index++, (uint8_t *)xuanfuwu_cn, sizeof(xuanfuwu_cn), (uint8_t *)xuanfuwu_en, PAGE_3_DATASHOW, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	}

	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_2_HISTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lishishuju_cn;
	interfacial->ChnContent_size = sizeof(lishishuju_cn);
	interfacial->content_eng = (uint8_t *)lishishuju_en; //将标签的字赋给界面
}


/*  根据电极型号选择删除历史数据界面*/
void generate_Histor_DeleteOption(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;//标签下标
	set_RowSpacing(ROWSPACING_MORE);
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	
	if(log_GetLogCount(TYPE_DO)	> 0)
	{ 
		datashow_SensorType=TYPE_DO;	
	  OptionList_Add(option_index++, (uint8_t *)rongjieyang_cn,  sizeof(rongjieyang_cn),  (uint8_t *)rongjieyang_en,  PAGE_3_DATADELETE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
  if(log_GetLogCount(TYPE_pH)	> 0) 
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_pH;
	  OptionList_Add(option_index++, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en, PAGE_3_DATADELETE,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_Tur)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Tur;
	  OptionList_Add(option_index++, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_FCL)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_FCL;
	  OptionList_Add(option_index++, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);		
	}
	if(log_GetLogCount(TYPE_EC)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_EC;
	  OptionList_Add(option_index++, (uint8_t *)diandaolv_cn, sizeof(diandaolv_cn), (uint8_t *)diandaolv_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_ORP)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
    datashow_SensorType=TYPE_ORP;		
	  OptionList_Add(option_index++, (uint8_t *)ORP_cn, sizeof(ORP_cn), (uint8_t *)ORP_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
  }
	if(log_GetLogCount(TYPE_NH4)	> 0)	
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_NH4;
	  OptionList_Add(option_index++, (uint8_t *)andan_cn, sizeof(andan_cn), (uint8_t *)andan_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  
	}
	if(log_GetLogCount(TYPE_F)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_F;
	  OptionList_Add(option_index++, (uint8_t *)F_cn, sizeof(F_cn), (uint8_t *)F_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  }
	if(log_GetLogCount(TYPE_CL)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_CL;
		OptionList_Add(option_index++, (uint8_t *)Oiw_ppm_cn, sizeof(Oiw_ppm_cn), (uint8_t *)Oiw_ppm_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	//   OptionList_Add(option_index++, (uint8_t *)CL_cn, sizeof(CL_cn), (uint8_t *)CL_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
  }
	if(log_GetLogCount(TYPE_Chl)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Chl;
		OptionList_Add(option_index++, (uint8_t *)Chl_cn, sizeof(Chl_cn), (uint8_t *)Chl_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  }
	if(log_GetLogCount(TYPE_Bga)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Bga;
		OptionList_Add(option_index++, (uint8_t *)Bga_cn, sizeof(Bga_cn), (uint8_t *)Bga_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
  }
	if(log_GetLogCount(TYPE_CODuv)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_CODuv;
		OptionList_Add(option_index++, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_MLSS)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_MLSS;
		OptionList_Add(option_index++, (uint8_t *)MLSS_cn, sizeof(MLSS_cn), (uint8_t *)MLSS_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_Oiw)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_Oiw;
		OptionList_Add(option_index++, (uint8_t *)Oiw_ppb_cn, sizeof(Oiw_ppb_cn), (uint8_t *)Oiw_ppb_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	}
	if(log_GetLogCount(TYPE_TSS)	> 0)
	{
		if(datashow_SensorType == TYPE_NONE)
		datashow_SensorType=TYPE_TSS;
		OptionList_Add(option_index++, (uint8_t *)xuanfuwu_cn, sizeof(xuanfuwu_cn), (uint8_t *)xuanfuwu_en, PAGE_3_DATADELETE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	}
	
	interfacial->option_head = option_head;
	
	interfacial->page_father = PAGE_2_HISTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lishishuju_cn;
	interfacial->ChnContent_size = sizeof(lishishuju_cn);
	interfacial->content_eng = (uint8_t *)lishishuju_en; //将标签的字赋给界面
}


void update_LogTitle(log_union* log , uint16_t index)
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
	static uint8_t use_size_arr[5] = {0};//49,152

	list_label label_head = NULL;
	
	list_option option_head = NULL;    //主标签
	
	set_RowSpacing(80);
	
	CurInterfacial_Destory();
	LabelList_Add( 0, 32, (uint8_t *)leixing_cn, sizeof(leixing_cn), (uint8_t *)leixing_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//类型	
	LabelList_Add( 0, 48, (uint8_t *)zuidakeyong_cn, sizeof(zuidakeyong_cn), (uint8_t *)zuidakeyong_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add( 0, 64, (uint8_t *)dangqianyiyong_cn, sizeof(dangqianyiyong_cn), (uint8_t *)dangqianyiyong_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add( 0, 80, (uint8_t *)dangqianxuanze_cn , sizeof(dangqianxuanze_cn ), (uint8_t *)dangqianxuanze_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
//	
//	LabelList_Add( 0, 32, (uint8_t *)zuidakeyong_cn, sizeof(zuidakeyong_cn), (uint8_t *)zuidakeyong_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
//	LabelList_Add( 0, 48, (uint8_t *)dangqianyiyong_cn, sizeof(dangqianyiyong_cn), (uint8_t *)dangqianyiyong_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
//	LabelList_Add( 0, 64, (uint8_t *)dangqianxuanze_cn , sizeof(dangqianxuanze_cn ), (uint8_t *)dangqianxuanze_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
	interfacial_UpdateCurLogIndexArr(data_index);

	snprintf((char *)use_size_arr, sizeof(use_size_arr), "%4d", log_GetLogCount(datashow_SensorType));   

	switch(datashow_SensorType)
	{
		case TYPE_DO:							
	    LabelList_Add( 48, 32, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);	
			break;				
		case TYPE_pH:
	    LabelList_Add( 48, 32, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			
			break;
		
		case TYPE_Tur:
	    LabelList_Add( 48, 32, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_FCL:	
	    LabelList_Add( 48, 32, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_EC:
	    LabelList_Add( 48, 32, (uint8_t *)diandaolv_cn, sizeof(diandaolv_cn), (uint8_t *)diandaolv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_ORP:
	    LabelList_Add( 48, 32, (uint8_t *)ORP_cn, sizeof(ORP_cn), (uint8_t *)ORP_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_NH4:
	    LabelList_Add( 48, 32, (uint8_t *)andan_cn, sizeof(andan_cn), (uint8_t *)andan_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		
			break;		
		case TYPE_F:	
	    LabelList_Add( 48, 32, (uint8_t *)F_cn, sizeof(F_cn), (uint8_t *)F_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		
			break;
		
		case TYPE_CL:	
		LabelList_Add( 48, 32, (uint8_t *)Oiw_ppm_cn, sizeof(Oiw_ppm_cn), (uint8_t *)Oiw_ppm_en,  LABEL_NORMAL, LABEL_xinziku, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
	    // LabelList_Add( 48, 32, (uint8_t *)CL_cn, sizeof(CL_cn), (uint8_t *)CL_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_Chl:
	    LabelList_Add( 48, 32, (uint8_t *)Chl_cn, sizeof(Chl_cn), (uint8_t *)Chl_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_Bga:
	    LabelList_Add( 48, 32, (uint8_t *)Bga_cn, sizeof(Bga_cn), (uint8_t *)Bga_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		case TYPE_CODuv:
	    LabelList_Add( 48, 32, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;

		case TYPE_MLSS:
	    LabelList_Add( 48, 32, (uint8_t *)MLSS_cn, sizeof(MLSS_cn), (uint8_t *)MLSS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;

		case TYPE_Oiw:
	    LabelList_Add( 48, 32, (uint8_t *)Oiw_ppb_cn, sizeof(Oiw_ppb_cn), (uint8_t *)Oiw_ppb_en,  LABEL_NORMAL, LABEL_xinziku, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;

		case TYPE_TSS:
	    LabelList_Add( 48, 32, (uint8_t *)xuanfuwu_cn, sizeof(xuanfuwu_cn), (uint8_t *)xuanfuwu_en,  LABEL_NORMAL, LABEL_xinziku, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				
			break;
		
		default:
			break;
			
	}	

	LabelList_Add( 80, 48, NULL, 0, (uint8_t *)maxsize,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //最大可用
	LabelList_Add( 88, 64, NULL, 0, use_size_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //已用大小
	LabelList_Add( 88, 80, NULL, 0, interfacial_GetCurLogIndexArr(),  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //第几条记录
	
	
	if(data_index != 0)
	{
		if(datashow_SensorType == TYPE_FCL)
		{
			log_ReadData(&log_u_generate, data_index - 1,TYPE_SAL);
		}
		else
		{
			log_ReadData(&log_u_generate, data_index - 1,datashow_SensorType);
		}
		
		update_LogTitle(&log_u_generate, data_index);
		
		OptionList_Add(0, NULL,  0,  LogTime_arr,  PAGE_4_DATALOG,   OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, IS_ENG_ONLY, NULL, &option_head);//具体的记录
		
	}
	else
	{
		OptionList_Add(0, (uint8_t *)nodata_cn,  sizeof(nodata_cn),  (uint8_t *)nodata_en,  NONE_PAGE, OPTION_LARGE, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);//具体的记录
	}
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_2_HISTORY_ShowOPTION;//设定父界面
	
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
	
	interfacial->page_father = PAGE_2_HISTORY_DeleteOPTION;//设定父界面
	
	interfacial->content_chn = (uint8_t *)shujushanchu_cn ;
	interfacial->ChnContent_size = sizeof(shujushanchu_cn);
	interfacial->content_eng = (uint8_t *)shujushanchu_en;
	
	interfacial->option_head = option_head;
}

void generate_DE26_EC_tds_value(PtrToInterfacial interfacial)
{
	double temp;
	int temp_int;
	uint8_t other_y;
	PtrToDOProbe p;
	
	set_RowSpacing(ROWSPACING_THREE);//设置行间距
	other_y = get_RowSpacing() + OPTION_STARTY;//用于子标签或者别的标签定位用y值
	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;    //子选项  就一串数字
	list_label label_head     = NULL;    //标签项  显示单位 实时数值 和一些标签
	
	p = get_CurDo();

	temp = p->compensate_b.value_f;
	LabelList_Add( 88, other_y, NULL, 0, (uint8_t *)get_CurDo()->tocmgl_Vol_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//显示溶解氧的温度
	OptionList_Add(0, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//当前值
	
	other_y += 16 + get_RowSpacing();

	temp_int = (int)(temp*1000);	
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
	NanoOptionList_Add(120, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			

	LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		
	
	OptionList_Add(1, (uint8_t *)shezhizhi_cn, sizeof(shezhizhi_cn), (uint8_t *)shezhizhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)tds_trans_value_cn ;
	interfacial->ChnContent_size = sizeof(tds_trans_value_cn);
	interfacial->content_eng = (uint8_t *)tds_trans_value_en;
}


void generate_DE26_EC_temp_data_value(PtrToInterfacial interfacial)
{
	double temp;
	int temp_int;
	uint8_t other_y;
	PtrToDOProbe p;
	
	set_RowSpacing(ROWSPACING_THREE);//设置行间距
	other_y = get_RowSpacing() + OPTION_STARTY;//用于子标签或者别的标签定位用y值
	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;    //子选项  就一串数字
	list_label label_head     = NULL;    //标签项  显示单位 实时数值 和一些标签
	
	p = get_CurDo();

	temp = p->compensate_k.value_f;
	LabelList_Add( 88, other_y, NULL, 0, (uint8_t *)get_CurDo()->DOmgl_Vol_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//显示溶解氧的温度
	OptionList_Add(0, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//当前值
	
	LabelList_Add(128, other_y, NULL, 0, (uint8_t *)BAIFENGHAO, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		

	other_y += 16 + get_RowSpacing();
	temp_int = (int)((temp*100) + 0.5);	
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/100), 0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),   0, 9, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
	
	LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		

	LabelList_Add(128, other_y, NULL, 0, (uint8_t *)BAIFENGHAO, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		

	OptionList_Add(1, (uint8_t *)shezhizhi_cn, sizeof(shezhizhi_cn), (uint8_t *)shezhizhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)wenduxishushezhi_cn ;
	interfacial->ChnContent_size = sizeof(wenduxishushezhi_cn);
	interfacial->content_eng = (uint8_t *)wenduxishushezhi_en;
}

/*盐度单位设置*/
void generate_Sal_set_uint(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_TWO);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头

	// OptionList_Add(option_index++, (uint8_t *)LH_B580_cn, sizeof(LH_B580_cn), (uint8_t *)LH_B580_en,     NONE_PAGE,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);

	OptionList_Add(0, (uint8_t *)PPT_cn, sizeof(PPT_cn), (uint8_t *)PPT_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)baifenhao_cn,  sizeof(baifenhao_cn),  (uint8_t *)baifenhao_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head); 

	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)yandudanweishezhi_cn ;
	interfacial->ChnContent_size = sizeof(yandudanweishezhi_cn);
	interfacial->content_eng = (uint8_t *)yandudanweishezhi_en;
	
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
	static uint8_t value4_arr[8] = {0};//设备的第四个值
	
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
	

	snprintf((char *)value1_arr, 7, "%6.2f", log_u_generate.log.log_data.temperature);//先初始化一下

	switch(datashow_SensorType)
	{
		case TYPE_DO:			
			
			snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.DO_percent);
			if(log_u_generate.log.log_data.DO_mg_L >= 10000.0)
			{
			 snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.DO_mg_L);	
			}else{
			 snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.DO_mg_L);	
			}
			LabelList_Add( 0, 80, (uint8_t *)qiya_cn, sizeof(qiya_cn), (uint8_t *)qiya_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//气压
			LabelList_Add( (setting_GetIsChn() ? 32 : 64), 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_KPA, HAVE_PARENTHESIS, &label_head);//(kpa):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)press,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			
			
			LabelList_Add( 0, 96, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//盐度
			LabelList_Add( (setting_GetIsChn() ? 32 : 64), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, HAVE_PARENTHESIS, &label_head);//(ppt):
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)sal,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);	

		
			LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			LabelList_Add( 0, 128, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//DO %
			LabelList_Add( (setting_GetIsChn() ? 48 : 16), 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, HAVE_PARENTHESIS, &label_head);//(%):
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value2_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			LabelList_Add( 0, 144, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//DO mg/L
			LabelList_Add( (setting_GetIsChn() ? 48 : 16), 144, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, HAVE_PARENTHESIS, &label_head);//(mg/L):
			LabelList_Add( 112, 144, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			break;				
		case TYPE_pH:
			

			if(log_u_generate.log.log_data.pH >= 10000.0)
			{
			 snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.pH);	
			}else{
			 snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.pH);	
			}
			
			LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 128, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//pH
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			break;
		
		case TYPE_Tur:
			// if(log_u_generate.log.log_data.Tur_NTU >= 10000.0)
			// {
			//  snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.Tur_NTU);	
			// }else{
			//  snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.Tur_NTU);	
			// }		
			
			// LabelList_Add( 0, 144, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
			// LabelList_Add( (setting_GetIsChn() ? 48 : 16), 144,(uint8_t *)NTU_cn, sizeof(NTU_cn), (uint8_t *)NTU_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, HAVE_PARENTHESIS, &label_head);//NTU		
			// LabelList_Add( 112, 144, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);	
			
			

			if(log_u_generate.log.log_data.Tur_NTU >= 10000.0)
			{
				snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.Tur_NTU);	
			}
			else
			{
				snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.Tur_NTU);	
			}		
			if(get_COMADo()->modbus_id == ZS_DZ09_ModbusID || get_COMBDo()->modbus_id == ZS_DZ09_ModbusID)
			{
				if(setting_GetTemp_jingdu() == 0)
				{
					snprintf((char *)value1_arr, 7, "%6d", (uint16_t)log_u_generate.log.log_data.temperature);
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					snprintf((char *)value1_arr, 7, "%6.1f", log_u_generate.log.log_data.temperature);
				}
				else
				{
					snprintf((char *)value1_arr, 7, "%6.2f", log_u_generate.log.log_data.temperature);
				}	
			}

			LabelList_Add( 0, 80, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
			LabelList_Add( (setting_GetIsChn() ? 48 : 32), 80,(uint8_t *)NTU_cn, sizeof(NTU_cn), (uint8_t *)NTU_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, HAVE_PARENTHESIS, &label_head);//NTU		
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);	
			
			LabelList_Add( 0, 96, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			break;
		
		case TYPE_FCL:
			//盐度
			if(log_u_generate.log.log_data.EC_salinity >= 0 && log_u_generate.log.log_data.EC_salinity < 100.0)//0.00 - 99.99 us
			{
				snprintf((char *)value4_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_salinity);	
			}
			else if(log_u_generate.log.log_data.EC_salinity >= 100.0 && log_u_generate.log.log_data.EC_salinity < 1000.0)//100.0 - 999.9 us
			{
				snprintf((char *)value4_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_salinity);	
			}
			else if(log_u_generate.log.log_data.EC_salinity >= 1000.0)//1000 - 9999 
			{
				snprintf((char *)value4_arr, 7, "%6.0f", log_u_generate.log.log_data.EC_salinity);	
			}

			LabelList_Add( 0, 80, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0,96, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_zhu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, DONT_HAVE_PARENTHESIS, &label_head);//ppt
			
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			break;
		
		case TYPE_EC:

			if(get_CurDo()->modbus_id == EC_DE40_ModbusID)
			{
				log_u_generate.log.log_data.EC_us_cm /= 1000.0f; 
			}
				if(log_u_generate.log.log_data.EC_us_cm >= 0 && log_u_generate.log.log_data.EC_us_cm < 100.0)//0.00 - 99.99 us
				{
					snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_us_cm);	
					snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_TDS);	
				}
				else if(log_u_generate.log.log_data.EC_us_cm >= 100.0 && log_u_generate.log.log_data.EC_us_cm < 1000.0)//100.0 - 999.9 us
				{
					snprintf((char *)value3_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_us_cm);	
					snprintf((char *)value2_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_TDS);	
				}
				else if(log_u_generate.log.log_data.EC_us_cm >= 1000.0 && log_u_generate.log.log_data.EC_us_cm < 10000.0)//1000 - 9999 us
				{
					snprintf((char *)value3_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.EC_us_cm);
					snprintf((char *)value2_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.EC_TDS);		
				}
				else if(log_u_generate.log.log_data.EC_us_cm >= 10000.0 && log_u_generate.log.log_data.EC_us_cm < 100000.0)//10.00ms - 99.99 ms
				{
					snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_us_cm / 1000);
					snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_TDS / 1000);
				}
				else if(log_u_generate.log.log_data.EC_us_cm >= 100000.0 && log_u_generate.log.log_data.EC_us_cm < 1000000.0)//100.00ms - 999.99 ms
				{
					snprintf((char *)value3_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_us_cm / 1000);	
					snprintf((char *)value2_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_TDS / 1000);
				}
				else if(log_u_generate.log.log_data.EC_us_cm >= 1000000.0)//1000 ms
				{
					snprintf((char *)value3_arr, 7, "%6d", 1000);	
					snprintf((char *)value2_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.EC_TDS / 1000);
				}			
			// //tds
			// if(log_u_generate.log.log_data.EC_TDS >= 0 && log_u_generate.log.log_data.EC_TDS < 100.0)//0.00 - 99.99 us
			// {
			// 	snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_TDS);	
			// }
			// else if(log_u_generate.log.log_data.EC_TDS >= 100.0 && log_u_generate.log.log_data.EC_TDS < 1000.0)//100.0 - 999.9 us
			// {
			// 	snprintf((char *)value2_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_TDS);	
			// }
			// else if(log_u_generate.log.log_data.EC_TDS >= 1000.0)//1000 - 9999 
			// {
			// 	snprintf((char *)value2_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.EC_TDS);
			// }

			//盐度
			// if(setting_GetSal_Uni() == 5)
			// {
			// 	log_u_generate.log.log_data.EC_salinity = log_u_generate.log.log_data.EC_salinity / 10.0;
			// }
			if(get_CurDo()->modbus_id == EC_DE40_ModbusID)
			{
				if(log_u_generate.log.log_data.EC_salinity >= 0 && log_u_generate.log.log_data.EC_salinity < 100.0)//0.00 - 99.99 us
				{
					snprintf((char *)value4_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_salinity);	
				}
				else if(log_u_generate.log.log_data.EC_salinity >= 100.0 && log_u_generate.log.log_data.EC_salinity < 1000.0)//100.0 - 999.9 us
				{
					snprintf((char *)value4_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_salinity);	
				}
				else if(log_u_generate.log.log_data.EC_salinity >= 1000.0)//1000 - 9999 
				{
					snprintf((char *)value4_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.EC_salinity);	
				}

			LabelList_Add( 0, 80, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 96, (uint8_t *)diandaolv_cn, sizeof(diandaolv_cn), (uint8_t *)diandaolv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//电导率
      		
			// if(log_u_generate.log.log_data.EC_us_cm >= 10000.0)  
			{
				LabelList_Add( (setting_GetIsChn() ? 48 : 24), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_mSCM, DONT_HAVE_PARENTHESIS, &label_head);//ms/cm
				LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_GL, DONT_HAVE_PARENTHESIS, &label_head);//g
			}							
			// else
			// {
			// 	LabelList_Add( (setting_GetIsChn() ? 48 : 24), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_USCM, DONT_HAVE_PARENTHESIS, &label_head);//us/cm
			// 	LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg
			// }
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			
			LabelList_Add( 0, 112, (uint8_t *)TDS_cn, sizeof(TDS_cn), (uint8_t *)TDS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			// LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value2_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 128, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_zhu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			// if(setting_GetSal_Uni() == 0)
			// {
				LabelList_Add( 32, 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, DONT_HAVE_PARENTHESIS, &label_head);//ppt
			// }
			// else if(setting_GetSal_Uni() == 5)
			// {
			// 	LabelList_Add( 32, 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//%
			// }
			// else if(setting_GetSal_Uni() == 10)
			// {
			// 	LabelList_Add( 32, 128, (uint8_t *)qianfenhao1_cn, sizeof(qianfenhao1_cn), (uint8_t *)qianfenhao_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			// }
			
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			}
			else if(get_CurDo()->modbus_id == EC_DE26_ModbusID)
			{
				if(log_u_generate.log.log_data.EC_salinity >= 0 && log_u_generate.log.log_data.EC_salinity < 100.0)//0.00 - 99.99 us
			{
				snprintf((char *)value4_arr, 7, "%6.2f", log_u_generate.log.log_data.EC_salinity);	
			}
			else if(log_u_generate.log.log_data.EC_salinity >= 100.0 && log_u_generate.log.log_data.EC_salinity < 1000.0)//100.0 - 999.9 us
			{
				snprintf((char *)value4_arr, 7, "%6.1f", log_u_generate.log.log_data.EC_salinity);	
			}
			else if(log_u_generate.log.log_data.EC_salinity >= 1000.0)//1000 - 9999 
			{
				snprintf((char *)value4_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.EC_salinity);	
			}

			LabelList_Add( 0, 80, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, DONT_HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 96, (uint8_t *)diandaolv_cn, sizeof(diandaolv_cn), (uint8_t *)diandaolv_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//电导率
      		
			if(log_u_generate.log.log_data.EC_us_cm >= 10000.0)  
			{
				LabelList_Add( (setting_GetIsChn() ? 48 : 24), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_mSCM, DONT_HAVE_PARENTHESIS, &label_head);//ms/cm
				LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_GL, DONT_HAVE_PARENTHESIS, &label_head);//g
			}							
			else
			{
				LabelList_Add( (setting_GetIsChn() ? 48 : 24), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_USCM, DONT_HAVE_PARENTHESIS, &label_head);//us/cm
				LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg
			}
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			
			LabelList_Add( 0, 112, (uint8_t *)TDS_cn, sizeof(TDS_cn), (uint8_t *)TDS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			// LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//mg
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value2_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 128, (uint8_t *)yandu_cn, sizeof(yandu_cn), (uint8_t *)yandu_zhu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			// if(setting_GetSal_Uni() == 0)
			// {
				LabelList_Add( 32, 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPT, DONT_HAVE_PARENTHESIS, &label_head);//ppt
			// }
			// else if(setting_GetSal_Uni() == 5)
			// {
			// 	LabelList_Add( 32, 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//%
			// }
			// else if(setting_GetSal_Uni() == 10)
			// {
			// 	LabelList_Add( 32, 128, (uint8_t *)qianfenhao1_cn, sizeof(qianfenhao1_cn), (uint8_t *)qianfenhao_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			// }
			
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			}

			break;
		
		case TYPE_ORP:

			if(log_u_generate.log.log_data.ORP_mV >= 10000.0)
			{
			 snprintf((char *)value3_arr, 7, "%5d", (int)log_u_generate.log.log_data.ORP_mV);	
			}else{
			 snprintf((char *)value3_arr, 7, "%5d", (int)log_u_generate.log.log_data.ORP_mV);	
			}

			LabelList_Add( 0, 144, (uint8_t *)ORP_cn, sizeof(ORP_cn), (uint8_t *)ORP_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//ORP数值
			LabelList_Add( (setting_GetIsChn() ? 48 : 48), 144, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MV, DONT_HAVE_PARENTHESIS, &label_head);//mv
			LabelList_Add( 112, 144, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			break;
		
		case TYPE_NH4:
			snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.pH);
			if(log_u_generate.log.log_data.NH4_mg_L >= 10000.0)
			{
			 snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.NH4_mg_L);	
			}else{
			 snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.NH4_mg_L);	
			}		
			
			LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 128, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//pH
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value2_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		
			LabelList_Add( 0, 144, (uint8_t *)andan_cn, sizeof(andan_cn), (uint8_t *)andan_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//DO mg/L
			LabelList_Add( (setting_GetIsChn() ? 48 : 32), 144, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, HAVE_PARENTHESIS, &label_head);//(mg/L)
			LabelList_Add( 112, 144, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
      break;	
			
		case TYPE_F:

			if(log_u_generate.log.log_data.F_mg_L >= 10000.0)
			{
			 snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.F_mg_L);	
			}else{
			 snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.F_mg_L);	
			}
			break;
		
		case TYPE_CL:

			snprintf((char *)value1_arr, 7, "%6.1f", log_u_generate.log.log_data.temperature);//先初始化一下
			if(log_u_generate.log.log_data.OIW_DA511_mg_L >= 100.0f)
			{
				snprintf((char *)value4_arr, 8, "%7.1f", log_u_generate.log.log_data.OIW_DA511_mg_L);
			}
			else if(log_u_generate.log.log_data.OIW_DA511_mg_L >= 10.0f)
			{
				snprintf((char *)value4_arr, 8, "%7.2f", log_u_generate.log.log_data.OIW_DA511_mg_L);
			}
			else
			{
				snprintf((char *)value4_arr, 8, "%7.2f", log_u_generate.log.log_data.OIW_DA511_mg_L);
			}
			LabelList_Add( 0, 96, (uint8_t *)Oiw_cn, sizeof(Oiw_cn), (uint8_t *)Oiw_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//水中油
			LabelList_Add( 104, 96, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			
			
			LabelList_Add( (setting_GetIsChn() ? 64 : 40), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPM, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):

			LabelList_Add( 0, 80, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			break;
		
		case TYPE_Chl:

			if(log_u_generate.log.log_data.Chl_ug_L >= 0 && log_u_generate.log.log_data.Chl_ug_L < 10.0)
			{
				snprintf((char *)value3_arr, 7, "%6.1f", log_u_generate.log.log_data.Chl_ug_L);	
			}
			else if(log_u_generate.log.log_data.Chl_ug_L >= 10 && log_u_generate.log.log_data.Chl_ug_L < 100.0)
			{
				snprintf((char *)value3_arr, 7, "%6.1f", log_u_generate.log.log_data.Chl_ug_L);	
			}
			else if(log_u_generate.log.log_data.Chl_ug_L >= 100 && log_u_generate.log.log_data.Chl_ug_L < 9000.0)
			{
				snprintf((char *)value3_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.Chl_ug_L);	
			}
			
			LabelList_Add( 0, 112, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 128, (uint8_t *)yelvsu_cn, sizeof(yelvsu_cn), (uint8_t *)yelvsu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//叶绿素
      		LabelList_Add( 56, 128, (uint8_t *)yelvsudanwei_cn, sizeof(yelvsudanwei_cn), (uint8_t *)yelvsudanwei_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//ug/L			
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
			break;
		
		case TYPE_Bga:
			if (log_u_generate.log.log_data.Bga_cells_mL <= 0.0)
			{
				snprintf(value3_arr,       7, "%6.4f", log_u_generate.log.log_data.Bga_cells_mL);
			}
			else if((log_u_generate.log.log_data.Bga_cells_mL) <= 100.0)
			{
				snprintf(value3_arr,       7, "%6.3f",log_u_generate.log.log_data.Bga_cells_mL);
			}
			else if((log_u_generate.log.log_data.Bga_cells_mL) <= 1000.0)
			{
				snprintf(value3_arr,       7, "%6.2f", log_u_generate.log.log_data.Bga_cells_mL);
			}
			else if((log_u_generate.log.log_data.Bga_cells_mL) <= 10000.0)
			{
				snprintf(value3_arr,       7, "%6.1f", log_u_generate.log.log_data.Bga_cells_mL);
			}
			LabelList_Add( 0, 80, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 96, (uint8_t *)Bga_cn, sizeof(Bga_cn), (uint8_t *)Bga_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//bga
			LabelList_Add( (setting_GetIsChn() ? 0 : 0), 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_Kcells_mL, HAVE_PARENTHESIS, &label_head);
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			break;
		
		case TYPE_CODuv:
			snprintf((char *)value2_arr, 7, "%6.2f", log_u_generate.log.log_data.Tur_NTU);
			if(log_u_generate.log.log_data.CODuv_mg_L >= 10000.0)
			{
				snprintf((char *)value3_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.CODuv_mg_L);	
			}
			else
			{
				snprintf((char *)value3_arr, 7, "%6.1f", log_u_generate.log.log_data.CODuv_mg_L);	
			}

			if(log_u_generate.log.log_data.CODuv_toc_mg_l >= 10000.0)
			{
				snprintf((char *)value4_arr, 7, "%6d", (unsigned int)log_u_generate.log.log_data.CODuv_toc_mg_l);	
			}
			else
			{
				snprintf((char *)value4_arr, 7, "%6.1f", log_u_generate.log.log_data.CODuv_toc_mg_l);	
			}
			
			if( !( (get_CurDo()->modbus_id == COD_DC18_ModbusID || get_CurDo()->modbus_id == COD_DC17_ModbusID) && (!setting_GetIsKey_DC18_Set()) ) )
			{			  
				LabelList_Add( 0, 144, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
				LabelList_Add( (setting_GetIsChn() ? 48 : 16), 144,(uint8_t *)NTU_cn, sizeof(NTU_cn), (uint8_t *)NTU_en, LABEL_NORMAL, LABEL_STRING, UINT_NONE, HAVE_PARENTHESIS, &label_head);//NTU
				LabelList_Add( 112, 144, NULL, 0, (uint8_t *)value2_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			
			}
			LabelList_Add( 0, 96, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	
			LabelList_Add( 0, 112, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//COD
			LabelList_Add( (setting_GetIsChn() ? 48 : 16), 112, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, HAVE_PARENTHESIS, &label_head);//(mg/L):
			LabelList_Add( 112, 112, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			
			
			LabelList_Add( 0, 128, (uint8_t *)TOC_cn, sizeof(TOC_cn), (uint8_t *)TOC_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//COD
			LabelList_Add( (setting_GetIsChn() ? 48 : 16), 128, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, HAVE_PARENTHESIS, &label_head);//(mg/L):
			LabelList_Add( 112, 128, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			

			break;
		case TYPE_MLSS:
			if(setting_GetMesUnit() == 0 || setting_GetMesUnit()== 10)
			{
				snprintf((char *)value4_arr, 8, "%7d", (int)log_u_generate.log.log_data.MLSS_mg_L);
			}
			else if(setting_GetMesUnit() == 5)
			{
				snprintf((char *)value4_arr, 8, "%7.3f", log_u_generate.log.log_data.MLSS_mg_L/1000.0);

			}
			else
			{
				snprintf((char *)value4_arr, 8, "%7.4f", log_u_generate.log.log_data.MLSS_mg_L/10000.0);
			}
				
			LabelList_Add( 0, 80, (uint8_t *)MLSS_cn, sizeof(MLSS_cn), (uint8_t *)MLSS_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//污泥浓度
			LabelList_Add( 104, 80, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			
			
			if(setting_GetMesUnit() == 0)
			{
			LabelList_Add( (setting_GetIsChn() ? 64 : 40), 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):

			}
			else if(setting_GetMesUnit() == 5)
			{
			LabelList_Add( (setting_GetIsChn() ? 64 : 40), 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_UGL, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):

			}
			else if(setting_GetMesUnit() == 10)
			{
			LabelList_Add( (setting_GetIsChn() ? 64 : 40), 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPM, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):

			}
			else if(setting_GetMesUnit() == 20)
			{
			LabelList_Add( (setting_GetIsChn() ? 64 : 40), 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PERCENT, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):
			}
			break;

		case TYPE_Oiw:				
			snprintf((char *)value1_arr, 7, "%6.1f", log_u_generate.log.log_data.temperature);//先初始化一下
			if(log_u_generate.log.log_data.OIW_mg_L >= 100.0f)
			{
				snprintf((char *)value4_arr, 8, "%7.1f", log_u_generate.log.log_data.OIW_mg_L);
			}
			else if(log_u_generate.log.log_data.OIW_mg_L >= 10.0f)
			{
				snprintf((char *)value4_arr, 8, "%7.2f", log_u_generate.log.log_data.OIW_mg_L);
			}
			else
			{
				snprintf((char *)value4_arr, 8, "%7.2f", log_u_generate.log.log_data.OIW_mg_L);
			}
			LabelList_Add( 0, 96, (uint8_t *)Oiw_cn, sizeof(Oiw_cn), (uint8_t *)Oiw_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//水中油
			LabelList_Add( 104, 96, NULL, 0, (uint8_t *)value4_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);			
			
			// if(get_CurDo()->modbus_id == OiW_guohong_ModbusID||get_CurDo()->modbus_id == OiW_yushan_DA511_ModbusID)
			// {
			// 	LabelList_Add( (setting_GetIsChn() ? 64 : 40), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPM, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):

			// }
			// else
			{
				LabelList_Add( (setting_GetIsChn() ? 64 : 40), 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_PPB, DONT_HAVE_PARENTHESIS, &label_head);//(mg/L):

			}
			LabelList_Add( 0, 80, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			break;

		case TYPE_TSS:
			if(log_u_generate.log.log_data.TSS_mg_L >= 10000.0)
			{
				snprintf((char *)value3_arr, 7, "%d", (unsigned int)log_u_generate.log.log_data.TSS_mg_L);	
			}
			else
			{
				snprintf((char *)value3_arr, 7, "%6.2f", log_u_generate.log.log_data.TSS_mg_L);	
			}		
			if(get_COMADo()->modbus_id == LH_DX01_ModbusID )
			{
				if(setting_GetTemp_jingdu() == 0)
				{
					snprintf((char *)value1_arr, 7, "%6d", (uint16_t)log_u_generate.log.log_data.temperature);
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					snprintf((char *)value1_arr, 7, "%6.1f", log_u_generate.log.log_data.temperature);
				}
				else
				{
					snprintf((char *)value1_arr, 7, "%6.2f", log_u_generate.log.log_data.temperature);
				}	
			}

			LabelList_Add( 0, 80, (uint8_t *)xuanfuwu_cn, sizeof(xuanfuwu_cn), (uint8_t *)xuanfuwu_en,  LABEL_NORMAL, LABEL_xinziku, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//浊度
			LabelList_Add( 112, 80, NULL, 0, (uint8_t *)value3_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);	
			LabelList_Add( (setting_GetIsChn() ? 56 : 32), 80, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_MGL, HAVE_PARENTHESIS, &label_head);

			LabelList_Add( 0, 96, (uint8_t *)wendu_cn, sizeof(wendu_cn), (uint8_t *)wendu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//温度
			LabelList_Add( 32, 96, NULL, 0, NULL,  LABEL_NORMAL, LABEL_UINT, UINT_CELSIUS, HAVE_PARENTHESIS, &label_head);//(℃):
			LabelList_Add( 112, 96, NULL, 0, (uint8_t *)value1_arr,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

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

/*生成温度校准界面*/
void generate_Cal_temp(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	double temp;
	int temp_int;
	uint8_t other_y;
	PtrToDOProbe p;
	
	set_RowSpacing(ROWSPACING_THREE);//设置行间距
	other_y = get_RowSpacing() + OPTION_STARTY;//用于子标签或者别的标签定位用y值
	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;    //子选项  就一串数字
	list_label label_head     = NULL;    //标签项  显示单位 实时数值 和一些标签
	
	p = get_CurDo();
	if((get_CurDo()->modbus_id) == MLSS_Tianjian_ModbusID)
	{
		temp = p->temperature.value_f + Getting_MLSS_Temp_B();
	}
	else
	{
	temp = p->temperature.value_f;
	}

	LabelList_Add( 80, other_y, NULL, 0, (uint8_t *)get_CurDo()->temperature_arr, LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//显示溶解氧的温度
	OptionList_Add(0, (uint8_t *)dangqianzhi_cn, sizeof(dangqianzhi_cn), (uint8_t *)dangqianzhi_en, NONE_PAGE, OPTION_SMALL, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//当前值
	
	other_y += 16 + get_RowSpacing();

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
			case Bga_shenghui_ModbusID:
				temp_int = (int)((temp*100) + 0.5);
			  	NanoOptionList_Add( 72, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
				NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
				NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
				LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				break;

			case LH_DX01_ModbusID:
				if(setting_GetTemp_jingdu() == 0)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);	
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				else
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 72, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
					LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				} 
				break;
			case ZS_DZ09_ModbusID:
				//一位小数
				if(setting_GetTemp_jingdu() == 0)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);	
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				else
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 72, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
					LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				break;

			default:
				temp_int = (int)(temp*100);		
				NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);			
				NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
				LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);	
	
				break;
		}	
	}
	else
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
			case Bga_shenghui_ModbusID:
				temp_int = (int)((temp*100) + 0.5);
				NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/1000), 0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
				NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
				LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				break;

			case LH_DX01_ModbusID:
				if(setting_GetTemp_jingdu() == 0)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);	
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				else
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 72, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
					LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				break;

				case ZS_DZ09_ModbusID:
				//一位小数
				if(setting_GetTemp_jingdu() == 0)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);	
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				else
				{
					temp_int = (int)((temp*100) + 0.5);
					NanoOptionList_Add( 72, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/10000), 0, 1, IS_SINGLE,  &NanoPress);		
					NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10000/1000), 0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
					NanoOptionList_Add(104, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
					NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
					LabelList_Add(96, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
				}
				break;

			default:
				temp_int = (int)(temp*100);			
				NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int/1000), 0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 9, IS_SINGLE,  &NanoPress);
				NanoOptionList_Add(112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
				LabelList_Add(104, other_y, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		
				break;
		}	
	}
	


	OptionList_Add(1, (uint8_t *)xiaozhunzhi_cn, sizeof(xiaozhunzhi_cn), (uint8_t *)xiaozhunzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	if(get_CurDo()->modbus_id == EC_DS46_ModbusID)
	{
		interfacial->content_chn = (uint8_t *)wendujiaozhun_cn ;
		interfacial->ChnContent_size = sizeof(wendujiaozhun_cn);
		interfacial->content_eng = (uint8_t *)wendujiaozhun_en;
	}
	else
	{
		interfacial->content_chn = (uint8_t *)wendu_cn ;
		interfacial->ChnContent_size = sizeof(wendu_cn);
		interfacial->content_eng = (uint8_t *)wendu_en;
	}
}

void generate_AutoLock(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_THREE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)guanbi_cn,   sizeof(guanbi_cn),   (uint8_t *)guanbi_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)zidong_cn,   sizeof(zidong_cn),   (uint8_t *)zidong_en,   PAGE_5_AUTOLOCKVALUE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(2, (uint8_t *)shoudong_cn, sizeof(shoudong_cn), (uint8_t *)shoudong_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)dushusuoding_cn ;
	interfacial->ChnContent_size = sizeof(dushusuoding_cn);
	interfacial->content_eng = (uint8_t *)dushusuoding_en;
	
	interfacial->option_head = option_head;
}


void generate_AutoLock_value(PtrToInterfacial interfacial)//PAGE_4_LOCKLEVEL
{//自动锁定 档位设置 快 中等 慢
	set_RowSpacing(ROWSPACING_THREE);
	
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)man_cn,  sizeof(man_cn),  (uint8_t *)man_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //慢
	OptionList_Add(1, (uint8_t *)zhongdeng_cn, sizeof(zhongdeng_cn), (uint8_t *)zhongdeng_en, NONE_PAGE,  OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//中等
	OptionList_Add(2, (uint8_t *)kuai_cn, sizeof(kuai_cn), (uint8_t *)kuai_en, NONE_PAGE,  OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//快
	
	interfacial->page_father = PAGE_4_AUTOLOCK;//设定父界面
	
	interfacial->content_chn = (uint8_t *)dushusuoding_cn;
	interfacial->ChnContent_size = sizeof(dushusuoding_cn);
	interfacial->content_eng = (uint8_t *)dushusuoding_en;
	
	interfacial->option_head = option_head;
}


/*滑动平均*/
void generate_SlideAverage_type(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	set_RowSpacing(ROWSPACING_TWO);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	if(*(rs485_GetDoList())!=NULL)         //如果do设备列表中有设备的话添加DO设备
	{
    switch(rs485_GetSensorType())
		{
			case TYPE_DO:					
	    	OptionList_Add(option_index++, (uint8_t *)rongjieyang_cn, sizeof(rongjieyang_cn), (uint8_t *)rongjieyang_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			

				break;				
			case TYPE_pH:
				OptionList_Add(option_index++, (uint8_t *)pH_cn, sizeof(pH_cn), (uint8_t *)pH_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_Tur:
				OptionList_Add(option_index++, (uint8_t *)zhuodu_cn, sizeof(zhuodu_cn), (uint8_t *)zhuodu_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_FCL:
				// OptionList_Add(option_index++, (uint8_t *)FCL_cn, sizeof(FCL_cn), (uint8_t *)FCL_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_EC:
				OptionList_Add(option_index++, (uint8_t *)diandaolv_cn, sizeof(diandaolv_cn), (uint8_t *)diandaolv_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_ORP:
				OptionList_Add(option_index++, (uint8_t *)ORP_cn, sizeof(ORP_cn), (uint8_t *)ORP_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_NH4:
				OptionList_Add(option_index++, (uint8_t *)andan_cn, sizeof(andan_cn), (uint8_t *)andan_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	
	      break;		
			case TYPE_F:
				OptionList_Add(option_index++, (uint8_t *)F_cn, sizeof(F_cn), (uint8_t *)F_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_CL:
				OptionList_Add(option_index++, (uint8_t *)CL_cn, sizeof(CL_cn), (uint8_t *)CL_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_Chl:
				OptionList_Add(option_index++, (uint8_t *)Chl_cn, sizeof(Chl_cn), (uint8_t *)Chl_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_Bga:
				OptionList_Add(option_index++, (uint8_t *)Bga_cn, sizeof(Bga_cn), (uint8_t *)COD_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
				break;
			
			case TYPE_CODuv:
				OptionList_Add(option_index++, (uint8_t *)COD_cn, sizeof(COD_cn), (uint8_t *)COD_en, PAGE_4_SLIDEAVG, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);						
				break;
			
			default:
				break;
			
		}	

	}
	else{
		OptionList_Add(option_index++, (uint8_t *)wuchuanganqi_cn, sizeof(wuchuanganqi_cn), (uint8_t *)wuchuanganqi_en, NONE_PAGE, OPTION_LARGE, CANNOT_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);			
	}
		
	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lvboshezhi_cn ;
	interfacial->ChnContent_size = sizeof(lvboshezhi_cn);
	interfacial->content_eng = (uint8_t *)lvboshezhi_en;
	
	interfacial->option_head = option_head;
}

void generate_SlideAverage_Switch(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  PAGE_5_SLIDEVALUE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); 
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	

	interfacial->page_father = PAGE_4_PARASET;//设定父界面	

	
	interfacial->content_chn = (uint8_t *)lvboshezhi_cn ;
	interfacial->ChnContent_size = sizeof(lvboshezhi_cn);
	interfacial->content_eng = (uint8_t *)lvboshezhi_en;
	
	interfacial->option_head = option_head;
}
void generate_SlideAverage_Value(PtrToInterfacial interfacial, uint8_t temp)
{
	uint8_t other_y;
	
	set_RowSpacing(ROWSPACING_TWO);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head  = NULL;//标签 次数 保存
	list_NanoOption NanoTimes  = NULL;//就次数设置
	list_label label_head = NULL;
	
	
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, temp/10, 0, 4, IS_SINGLE, &NanoTimes);//低门限
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, temp%10, 0, 9, IS_SINGLE, &NanoTimes);
	
	LabelList_Add( 112, other_y, NULL, 0, (uint8_t *)cishuxianzhi_slideavg,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//(2-40)
	
	OptionList_Add(0, (uint8_t *)canshu_cn,  sizeof(canshu_cn),  (uint8_t *)canshu_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoTimes, &option_head);	//报警音
	OptionList_Add(1, (uint8_t *)baocun_cn, sizeof(baocun_cn), (uint8_t *)baocun_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存
	
	
	
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_SLIDEAVG;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lvboshezhi_cn;
	interfacial->ChnContent_size = sizeof(lvboshezhi_cn);
	interfacial->content_eng = (uint8_t *)lvboshezhi_en;
	
	interfacial->option_head = option_head;
}

void generate_SlideAverageA_Sensor_Value(PtrToInterfacial interfacial, uint8_t temp)
{
	uint8_t other_y;
	
	set_RowSpacing(ROWSPACING_TWO);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head  = NULL;//标签 次数 保存
	list_NanoOption NanoTimes  = NULL;//就次数设置
	list_label label_head = NULL;
	
	NanoOptionList_Add( 80, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, temp/100, 0, 2, IS_SINGLE, &NanoTimes);//低门限
	NanoOptionList_Add( 88, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, temp/10%10, 0, 9, IS_SINGLE, &NanoTimes);//低门限
	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, temp%10, 0, 9, IS_SINGLE, &NanoTimes);
	
	LabelList_Add( 104, other_y, NULL, 0, (uint8_t *)cishuxianzhi_slideavg,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//(2-40)
	
	OptionList_Add(0, (uint8_t *)canshu_cn,  sizeof(canshu_cn),  (uint8_t *)canshu_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoTimes, &option_head);	//报警音
	OptionList_Add(1, (uint8_t *)baocun_cn, sizeof(baocun_cn), (uint8_t *)baocun_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,      &option_head);//保存
	
	
	
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)lvbocishu_cn;
	interfacial->ChnContent_size = sizeof(lvbocishu_cn);
	interfacial->content_eng = (uint8_t *)lvbocishu_en;
	
	interfacial->option_head = option_head;
}

//间隔保存界面
void generate_IntervaSave(PtrToInterfacial interfacial,uint16_t num, uint8_t is_chn)
{
	uint8_t other_y = 0;
	set_RowSpacing(ROWSPACING_THREE);
	
	other_y = get_RowSpacing() + OPTION_STARTY;
	
	CurInterfacial_Destory();
	
	list_option option_head = NULL;    //主标签
	list_NanoOption NanoDate = NULL;   //日期子标签
	list_NanoOption NanoTime = NULL;   //时间子标签
	list_label label_head = NULL;


/*	NanoOptionList_Add( 96, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,num/3600,   0, 9, IS_SINGLE, &NanoTime);
	NanoOptionList_Add( 112, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, num%3600/60, 0, 59, NOT_SINGLE, &NanoTime);
	NanoOptionList_Add(136, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, num%3600%60, 0, 59, NOT_SINGLE, &NanoTime);
	
	LabelList_Add(104, other_y, NULL, 0, (const char *)MAOHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	LabelList_Add(128, other_y, NULL, 0, (const char *)MAOHAO,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	OptionList_Add(0, (uint8_t *)baocunjiange_cn, sizeof(baocunjiange_cn), baocunjiange_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoTime, &option_head);	
	OptionList_Add(1, (uint8_t *)baocun_cn , sizeof(baocun_cn ), baocun_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);
*/
	other_y += 16 + get_RowSpacing();



	if(is_chn)
	{

		NanoOptionList_Add( 52, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,num/3600,   0, 9, IS_SINGLE, &NanoTime);
		LabelList_Add(60, other_y,(uint8_t *)h_cn, sizeof(h_cn), (uint8_t *)h_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				//时
		NanoOptionList_Add( 76, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, num%3600/60, 0, 59, NOT_SINGLE, &NanoTime);
		LabelList_Add(92, other_y,(uint8_t *)min_cn, sizeof(min_cn), (uint8_t *)min_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		//分	
	}
	else
	{
		NanoOptionList_Add( 56, other_y, NULL, 0, NULL, NANOOPTION_NUMBER,num/3600,   0, 9, IS_SINGLE, &NanoTime);
		LabelList_Add(64, other_y,(uint8_t *)h_cn, sizeof(h_cn), (uint8_t *)h_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);				//h
		NanoOptionList_Add( 72, other_y, NULL, 0, NULL, NANOOPTION_NUMBER, num%3600/60, 0, 59, NOT_SINGLE, &NanoTime);
		LabelList_Add(88, other_y,(uint8_t *)min_cn, sizeof(min_cn),(uint8_t *) min_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);		//min
	}
	OptionList_Add(0, (uint8_t *)baocunjiange_cn, sizeof(baocunjiange_cn), (uint8_t *)baocunjiange_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoTime, &option_head);	
	OptionList_Add(2, (uint8_t *)baocun_cn , sizeof(baocun_cn ), (uint8_t *)baocun_en,  NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL,     &option_head);
	
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	
	interfacial->page_father = PAGE_3_AUTOSAVE;//设定父界面
	
	interfacial->content_chn = (uint8_t *)jiangebaocun_cn;
	interfacial->ChnContent_size = sizeof(jiangebaocun_cn);
	interfacial->content_eng = (uint8_t *)jiangebaocun_en;
}
//锁定保存界面
void generate_LockSave(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //开启
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//关闭
	
	interfacial->page_father = PAGE_3_AUTOSAVE;//设定父界面
	
	interfacial->content_chn = (uint8_t *)suodingbaocun_cn ;
	interfacial->ChnContent_size = sizeof(suodingbaocun_cn);
	interfacial->content_eng = (uint8_t *)suodingbaocun_en;
	
	interfacial->option_head = option_head;
}


//氨氮传感器 校准相关界面

/*NH3N 第一点校准*/
void generate_NH3N_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1 ,0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)andan_jiaozhun1_cn;
	interfacial->ChnContent_size = sizeof(andan_jiaozhun1_cn);
	interfacial->content_eng = (uint8_t *)andan_jiaozhun1_en;
}


/*NH3N 第二点校准*/
void generate_NH3N_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)andan_jiaozhun2_cn;
	interfacial->ChnContent_size = sizeof(andan_jiaozhun2_cn);
	interfacial->content_eng = (uint8_t *)andan_jiaozhun2_en;
}

/*NH3N pH的第一点校准  4.01*/
void generate_NH3N_pH_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(68, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
  if(!setting_GetIs_pH_Group()){
		
		if(get_CurDo()->modbus_id == pH_DpH07_ModbusID)
    {
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 4, 4, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);					
		}	else{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 3, 4, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);			
		}		

		interfacial->content_chn = (uint8_t *)andan_pH_jiaozhun1_cn;
		interfacial->ChnContent_size = sizeof(andan_pH_jiaozhun1_cn);
		interfacial->content_eng = (uint8_t *)andan_pH_jiaozhun1_en;
	}else{
		
		if(get_CurDo()->modbus_id == pH_DpH07_ModbusID)
    {
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 4, 4, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent);				
		}	else{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 3, 4, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 1, 1, IS_SINGLE, &NanoPercent);	
		}
		interfacial->content_chn = (uint8_t *)andan_pH_jiaozhun1_USA_cn;
		interfacial->ChnContent_size = sizeof(andan_pH_jiaozhun1_USA_cn);
		interfacial->content_eng = (uint8_t *)andan_pH_jiaozhun1_USA_en;
	}


	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	

}

/*NH3N pH的第二点校准  6.86*/
void generate_NH3N_pH_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(68, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);



  if(!setting_GetIs_pH_Group()){
		
		if(get_CurDo()->modbus_id == pH_DpH07_ModbusID)
    {
      NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 6, 6, 6, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 8, 8, 8, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 6, 6, 6, IS_SINGLE, &NanoPercent);	
		}	else{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 6, 6, 7, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 8, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 6, 0, 9, IS_SINGLE, &NanoPercent);	
		}		
		

		interfacial->content_chn = (uint8_t *)andan_pH_jiaozhun2_cn;
		interfacial->ChnContent_size = sizeof(andan_pH_jiaozhun2_cn);
		interfacial->content_eng = (uint8_t *)andan_pH_jiaozhun2_en;
	}else{
		
		if(get_CurDo()->modbus_id == pH_DpH07_ModbusID)
    {
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 7, 7, 7, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);					
		}	else{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 7, 6, 7, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);		
		}		

		interfacial->content_chn = (uint8_t *)andan_pH_jiaozhun2_USA_cn;
		interfacial->ChnContent_size = sizeof(andan_pH_jiaozhun2_USA_cn);
		interfacial->content_eng = (uint8_t *)andan_pH_jiaozhun2_USA_en;
	}

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	

}

/*NH3N pH的第三点校准  9.18*/
void generate_NH3N_pH_THREE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(68, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);


  if(!setting_GetIs_pH_Group()){
		
		if(get_CurDo()->modbus_id == pH_DpH07_ModbusID)
    {
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 9, 9, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 1, 1, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 8, 8, 8, IS_SINGLE, &NanoPercent);	
		}	else{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 9, 8, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 8, 0, 9, IS_SINGLE, &NanoPercent);
		}		
		
		interfacial->content_chn = (uint8_t *)andan_pH_jiaozhun3_cn;
		interfacial->ChnContent_size = sizeof(andan_pH_jiaozhun3_cn);
		interfacial->content_eng = (uint8_t *)andan_pH_jiaozhun3_en;
	}else{
		
		if(get_CurDo()->modbus_id == pH_DpH07_ModbusID)
    {
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 1, 1, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 1, 1, IS_SINGLE, &NanoPercent);			
		}	else{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 1, IS_SINGLE, &NanoPercent); 
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 1, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(74, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent);
		}		

		interfacial->content_chn = (uint8_t *)andan_pH_jiaozhun3_USA_cn;
		interfacial->ChnContent_size = sizeof(andan_pH_jiaozhun3_USA_cn);
		interfacial->content_eng = (uint8_t *)andan_pH_jiaozhun3_USA_en;
	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
}





/*pH 标液组管理*/
void generate_pH_Standard_liquid_management(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);	
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	list_label label_head   = NULL;

	OptionList_Add(0,  (uint8_t *)GB_cn, sizeof(GB_cn), (uint8_t *)GB_en, NONE_PAGE,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); // "GB标液组"
	LabelList_Add( 40,  56, NULL, 0, (uint8_t *)GB_Show_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//


	OptionList_Add(2,  (uint8_t *)USA_cn, sizeof(USA_cn), (uint8_t *)USA_en, NONE_PAGE,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //"USA标液组"
	LabelList_Add( 40,  96, NULL, 0, (uint8_t *)USA_Show_en,  LABEL_NORMAL, LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);//	
	
	interfacial->page_father = PAGE_3_SENSORS;//设定父界面
	
	interfacial->content_chn = (uint8_t *)biaoyezuguanli_cn ;
	interfacial->ChnContent_size = sizeof(biaoyezuguanli_cn);
	interfacial->content_eng = (uint8_t *)biaoyezuguanli_en;
	
	interfacial->label_head = label_head;	
	interfacial->option_head = option_head;
}

/*Chl  叶绿素的量程设置*/
void generate_Chl_Measure_Switch(PtrToInterfacial interfacial)
{

	set_RowSpacing(ROWSPACING_TWO);

	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	list_label label_head   = NULL;

	OptionList_Add(0,  (uint8_t *)diliangcheng_cn, sizeof(diliangcheng_cn), (uint8_t *)diliangcheng_en, NONE_PAGE,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); // "低量程（50ug/L）"
	OptionList_Add(1,  (uint8_t *)gaoliangcheng_cn, sizeof(gaoliangcheng_cn), (uint8_t *)gaoliangcheng_en, NONE_PAGE,     OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); // "高量程（500ug/L）"
	

	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)liangchengshezhi_cn ;
	interfacial->ChnContent_size = sizeof(liangchengshezhi_cn);
	interfacial->content_eng = (uint8_t *)liangchengshezhi_en;
	
	interfacial->label_head = label_head;	
	interfacial->option_head = option_head;
}



//COD传感器  校准相关界面

//COD 昇辉  零点校准
void generate_COD_shenghui_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(ROWSPACING_ONE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	list_label label_head   = NULL;	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)&shi_cn, 1, (uint8_t *)shi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	LabelList_Add(20, 136,
								NULL, 0, (uint8_t *)"mg/L",
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head);  
	LabelList_Add(0, 120,
								NULL, 0, (uint8_t *)get_CurDo()->DOmgl_arr,
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head);  
	LabelList_Add(105, 136,
								NULL, 0, (uint8_t *)"NTU",
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head);  
	LabelList_Add(86, 120,
								NULL, 0, (uint8_t *)get_CurDo()->DOpercent_arr,
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head);  

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_Zero_cn ;
	interfacial->ChnContent_size = sizeof(COD_Zero_cn);
	interfacial->content_eng = (uint8_t *)COD_Zero_en;
	

	interfacial->label_head = label_head;
	interfacial->option_head = option_head;
}


/*COD 昇辉  浊度 第一点校准*/
void generate_COD_shenghui_Tur_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 5, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_zhuodu_jiaozhun1_cn;
	interfacial->ChnContent_size = sizeof(COD_zhuodu_jiaozhun1_cn);
	interfacial->content_eng = (uint8_t *)COD_zhuodu_jiaozhun1_en;
}

/*COD 昇辉  浊度 第二点校准*/
void generate_COD_shenghui_Tur_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 2, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_zhuodu_jiaozhun2_cn;
	interfacial->ChnContent_size = sizeof(COD_zhuodu_jiaozhun2_cn);
	interfacial->content_eng = (uint8_t *)COD_zhuodu_jiaozhun2_cn;
}

/*COD 昇辉  COD 第一点校准*/
void generate_COD_shenghui_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 3, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_jiaozhun1_cn;
	interfacial->ChnContent_size = sizeof(COD_jiaozhun1_cn);
	interfacial->content_eng = (uint8_t *)COD_jiaozhun1_en;
}


/*COD 昇辉  COD 第二点校准*/
void generate_COD_shenghui_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_jiaozhun2_cn;
	interfacial->ChnContent_size = sizeof(COD_jiaozhun2_cn);
	interfacial->content_eng = (uint8_t *)COD_jiaozhun2_en;
}


/*COD 昇辉  COD 第一点校准*/
void generate_COD_shenghui_THREE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_jiaozhun3_cn;
	interfacial->ChnContent_size = sizeof(COD_jiaozhun3_cn);
	interfacial->content_eng = (uint8_t *)COD_jiaozhun3_en;
}


	/*COD  开启刷子 清洗*/
void generate_COD_Cleanse(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	
	set_RowSpacing(ROWSPACING_ONE);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	list_label label_head   = NULL;	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)&shi_cn, 1, (uint8_t *)shi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);


	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)Cleanse_cn ;
	interfacial->ChnContent_size = sizeof(Cleanse_cn);
	interfacial->content_eng = (uint8_t *)Cleanse_en;
	

	interfacial->label_head = label_head;
	interfacial->option_head = option_head;
}


/*COD DC18  浊度 零点校准*/
void generate_COD_DC18_Tur_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type,PAGE_NUM page_num)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	if(page_num == PAGE_5_COD_DC17_IN_Tur_Zero)
	{
		interfacial->page_father = PAGE_4_INCAL;//设定父界面
	}
	else
	{
		interfacial->page_father = PAGE_4_CAL;//设定父界面
	}
	
	interfacial->content_chn = (uint8_t *)COD_zhuodu_zero_cn;
	interfacial->ChnContent_size = sizeof(COD_zhuodu_zero_cn);
	interfacial->content_eng = (uint8_t *)COD_zhuodu_zero_en;
}


/*COD DC18  浊度 斜率校准*/
void generate_COD_DC18_Tur_Slope(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_zhuodu_slope_cn;
	interfacial->ChnContent_size = sizeof(COD_zhuodu_slope_cn);
	interfacial->content_eng = (uint8_t *)COD_zhuodu_slope_en;
}


/*COD DC18  COD 零点校准*/
void generate_COD_DC18_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type,PAGE_NUM page_num)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;
	if(page_num == PAGE_5_COD_DC17_IN_Zero)
	{
		interfacial->page_father = PAGE_4_INCAL;//设定父界面
	}
	else
	{
		interfacial->page_father = PAGE_4_CAL;//设定父界面
	}
	
	interfacial->content_chn = (uint8_t *)COD_zero_cn;
	interfacial->ChnContent_size = sizeof(COD_zero_cn);
	interfacial->content_eng = (uint8_t *)COD_zero_en;
}


/*COD DC18  COD  斜率校准*/
void generate_COD_DC18_Slope(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值
	NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_slope_cn;
	interfacial->ChnContent_size = sizeof(COD_slope_cn);
	interfacial->content_eng = (uint8_t *)COD_slope_en;
}


/*COD DC18  补偿系数*/
void generate_COD_DC18_Coefficient(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	int temp_int;
	set_RowSpacing(ROWSPACING_THREE);//设置行间距	
	CurInterfacial_Destory();
	
	list_option option_head   = NULL;    //主选项  就一个save
	list_NanoOption NanoPress = NULL;    //子选项  就一串数字
	list_label label_head     = NULL;    //标签项  显示单位 实时数值 和一些标签
	temp_int=get_CurDo()->Measure_Range.value_f;
  if(temp_int >= 100)
	{
    temp_int=99;	
	}
	NanoOptionList_Add( 88, 36, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%1000/100),   0, 0, IS_SINGLE,  &NanoPress);
	NanoOptionList_Add(104, 36, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%100/10),     0, 9, IS_SINGLE,  &NanoPress);//小数点后1位			
	NanoOptionList_Add(112, 36, NULL, 0, NULL, NANOOPTION_NUMBER,  (uint8_t)(temp_int%10),         0, 9, IS_SINGLE,  &NanoPress);//小数点后两位
	
	LabelList_Add(96, 36, NULL, 0, (uint8_t *)XIAOSHUDIAN, LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
	OptionList_Add(1, (uint8_t *)xiaozhunzhi_cn, sizeof(xiaozhunzhi_cn), (uint8_t *)xiaozhunzhi_en, NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPress, &option_head);//实际值
	
	OptionList_Add(2, (uint8_t *)baocun_cn,      sizeof(baocun_cn),      (uint8_t *)baocun_en,      NONE_PAGE, OPTION_SMALL, CAN_BE_SELECTED,    NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);//保存	

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面
	
	interfacial->content_chn = (uint8_t *)COD_Coefficient_cn;
	interfacial->ChnContent_size = sizeof(COD_Coefficient_cn);
	interfacial->content_eng = (uint8_t *)COD_Coefficient_en;
}


/* 昇辉  浊度 第一点校准*/
void generate_shenghui_Tur_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	}
	else if(get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 2, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	}
	else
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		interfacial->page_father = PAGE_4_PARASET;//设定父界面
		interfacial->content_chn = (uint8_t *)biaoyejiaozhun_one_cn;
		interfacial->ChnContent_size = sizeof(biaoyejiaozhun_one_cn);
		interfacial->content_eng = (uint8_t *)biaoyejiaozhun_one_en;		
	}
	else if(get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
	{
		interfacial->content_chn = (uint8_t *)lingdian_cn;
		interfacial->ChnContent_size = sizeof(lingdian_cn);
		interfacial->content_eng = (uint8_t *)lingdian_en;
	}
	else
	{
		interfacial->page_father = PAGE_4_CAL;//设定父界面
		interfacial->content_chn = (uint8_t *)COD_zhuodu_jiaozhun1_cn;
		interfacial->ChnContent_size = sizeof(COD_zhuodu_jiaozhun1_cn);
		interfacial->content_eng = (uint8_t *)COD_zhuodu_jiaozhun1_en;
	}
}


/* 昇辉  浊度 第二点校准*/
void generate_shenghui_Tur_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值

	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	}
	else if(get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
	{
		LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	}
	else
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		interfacial->page_father = PAGE_4_PARASET;//设定父界面
		interfacial->content_chn = (uint8_t *)biaoyejiaozhun_Two_cn;
		interfacial->ChnContent_size = sizeof(biaoyejiaozhun_Two_cn);
		interfacial->content_eng = (uint8_t *)biaoyejiaozhun_Two_en;		
	}
	else if(get_CurDo()->modbus_id == ZS_DZ09_ModbusID)
	{
		interfacial->content_chn = (uint8_t *)xielvjiaozhun_cn;
		interfacial->ChnContent_size = sizeof(xielvjiaozhun_cn);
		interfacial->content_eng = (uint8_t *)xielvjiaozhun_en;
	}
	else
	{
		interfacial->page_father = PAGE_4_CAL;//设定父界面
		interfacial->content_chn = (uint8_t *)COD_zhuodu_jiaozhun2_cn;
		interfacial->ChnContent_size = sizeof(COD_zhuodu_jiaozhun2_cn);
		interfacial->content_eng = (uint8_t *)COD_zhuodu_jiaozhun2_en;
	}
}

/* 昇辉  浊度 第三点校准*/
void generate_shenghui_Tur_THREE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

	}
	else
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 8, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		interfacial->page_father = PAGE_4_PARASET;//设定父界面
		interfacial->content_chn = (uint8_t *)biaoyejiaozhun_Three_cn;
		interfacial->ChnContent_size = sizeof(biaoyejiaozhun_Three_cn);
		interfacial->content_eng = (uint8_t *)biaoyejiaozhun_Three_en;		
	}
	else
	{
		interfacial->page_father = PAGE_4_CAL;//设定父界面
		interfacial->content_chn = (uint8_t *)COD_zhuodu_jiaozhun3_cn;
		interfacial->ChnContent_size = sizeof(COD_zhuodu_jiaozhun3_cn);
		interfacial->content_eng = (uint8_t *)COD_zhuodu_jiaozhun3_en;
	}
}

/* 昇辉  电导率 一点校准*/
void generate_shenghui_EC_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	
	if(get_CurDo()->modbus_id == Chl_shenghui_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		
		if(get_CurDo()->Measure_Range.value_f == 256)
		{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		}
		else if(get_CurDo()->Measure_Range.value_f == 4095)
		{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 0, 9, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		}	

	}
	else if(get_CurDo()->modbus_id == ORP_DR31_ModbusID)
	{
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 2, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 5, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 6, 0, 9, IS_SINGLE, &NanoPercent);
	}
	else if(get_CurDo()->modbus_id == Bga_shenghui_ModbusID)
	{
		NanoOptionList_Add(44, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

		LabelList_Add(92, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
  	}
	else if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent);
  	}
	else if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(108, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
  	}
	else if(get_CurDo()->modbus_id == OiW_yushan_ModbusID
	|| get_CurDo()->modbus_id == OiW_yushan_DA511_ModbusID
	|| get_CurDo()->modbus_id == EC_DS46_ModbusID
	|| get_CurDo()->modbus_id == EC_DE40_ModbusID)
	{
		// NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
  	}
	else
	{
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 4, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 1, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 3, 0, 9, IS_SINGLE, &NanoPercent);
  	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		

	if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_PARASET;//设定父界面
	}
	else
	{
		interfacial->option_head = option_head;
		interfacial->label_head = label_head;

		interfacial->page_father = PAGE_4_CAL;//设定父界面
	}

	switch(get_CurDo()->modbus_id)
	{									
		case EC_DE26_ModbusID: 
		case EC_DE40_ModbusID:
		case ORP_DR31_ModbusID:
			interfacial->content_chn = (uint8_t *)dandian_cn;
			interfacial->ChnContent_size = sizeof(dandian_cn);
			interfacial->content_eng = (uint8_t *)dandian_en;
			break;	
		case OiW_guohong_ModbusID:
		case OiW_yushan_ModbusID:
		case OiW_yushan_DA511_ModbusID:
			interfacial->content_chn = (uint8_t *)dandianjiaozhun_cn;
			interfacial->ChnContent_size = sizeof(dandianjiaozhun_cn);
			interfacial->content_eng = (uint8_t *)dandianjiaozhun_en;
			break;
		
		case EC_shenghui_ModbusID:  
			interfacial->content_chn = (uint8_t *)diandaolv_cn;
			interfacial->ChnContent_size = sizeof(diandaolv_cn);
			interfacial->content_eng = (uint8_t *)diandaolv_en;
			break;	

		case Chl_shenghui_ModbusID:  
			interfacial->content_chn = (uint8_t *)gaodian_cn;
			interfacial->ChnContent_size = sizeof(gaodian_cn);
			interfacial->content_eng = (uint8_t *)gaodian_en;
			break;

		case Bga_shenghui_ModbusID:
			interfacial->content_chn = (uint8_t *)Bga_jiaozhun1_cn;
			interfacial->ChnContent_size = sizeof(Bga_jiaozhun1_cn);
			interfacial->content_eng = (uint8_t *)Bga_jiaozhun1_en;
			break;

		case MLSS_Tianjian_ModbusID:
			interfacial->content_chn = (uint8_t *)xielvshezhi_cn;
			interfacial->ChnContent_size = sizeof(xielvshezhi_cn);
			interfacial->content_eng = (uint8_t *)xielvshezhi_en;
			break;

		case EC_DS46_ModbusID:
			interfacial->content_chn = (uint8_t *)yandujiaozhun_cn;
			interfacial->ChnContent_size = sizeof(yandujiaozhun_cn);
			interfacial->content_eng = (uint8_t *)yandujiaozhun_en;

			break;
		
		default:
			break;
	}

}


/*水中油斜率校准*/
void generate_OIW_K_CAL(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	if(get_CurDo()->modbus_id == OiW_guohong_ModbusID)
	{
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		LabelList_Add(68, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
  	}
	else if(get_CurDo()->modbus_id == OiW_yushan_ModbusID
	|| get_CurDo()->modbus_id == OiW_yushan_DA511_ModbusID
	|| get_CurDo()->modbus_id == LH_DX01_ModbusID)
	{ 
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面

	switch(get_CurDo()->modbus_id)
	{									
		case OiW_guohong_ModbusID:
		case OiW_yushan_ModbusID:
		case OiW_yushan_DA511_ModbusID:
		case LH_DX01_ModbusID:
			interfacial->content_chn = (uint8_t *)xielvjiaozhun_cn;
			interfacial->ChnContent_size = sizeof(xielvjiaozhun_cn);
			interfacial->content_eng = (uint8_t *)xielvjiaozhun_en;
			break;	
		default:
			break;
	}
}


/* 蓝绿藻 1点校准*/
void generate_shenghui_BGA_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	if(get_CurDo()->modbus_id == Bga_shenghui_ModbusID)
	{
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

		LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
  	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面

	switch(get_CurDo()->modbus_id)
	{									
		case Bga_shenghui_ModbusID:
			interfacial->content_chn = (uint8_t *)lingdianjiaozhun_cn;
			interfacial->ChnContent_size = sizeof(lingdianjiaozhun_cn);
			interfacial->content_eng = (uint8_t *)lingdianjiaozhun_en;
			break;

		default:
			break;
	}

}

/* 蓝绿藻 2点校准*/
void generate_shenghui_BGA_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	if(get_CurDo()->modbus_id == Bga_shenghui_ModbusID)
	{
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

		LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
  	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);
		
	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_CAL;//设定父界面

	switch(get_CurDo()->modbus_id)
	{									
		case Bga_shenghui_ModbusID:
			interfacial->content_chn = (uint8_t *)xielvjiaozhun_cn;
			interfacial->ChnContent_size = sizeof(xielvjiaozhun_cn);
			interfacial->content_eng = (uint8_t *)xielvjiaozhun_en;
			break;

		default:
			break;
	}

}

/* 自研DE26 电导率      昇辉 叶绿素 零点校准 
零点校准*/
void generate_DE26_EC_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type)
{
	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	list_option option_head = NULL;
	list_NanoOption NanoPercent = NULL;
	
	list_label label_head   = NULL;

	LabelList_Add( 0, 36, (uint8_t *)biaozhunzhi_cn, sizeof(biaozhunzhi_cn), (uint8_t *)biaozhunzhi_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //标准值
	
	
	if(get_CurDo()->modbus_id == Chl_shenghui_ModbusID)
	{
		LabelList_Add(76, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
	}
	else if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
	{	
		if((setting_GetMesUnit() == 0) || (setting_GetMesUnit() == 10))
		{
			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		}
		else if(setting_GetMesUnit() == 5)
		{
			LabelList_Add(68, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

		}
		else
		{
			LabelList_Add(68, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

			NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 68, 67, 68, IS_SINGLE, &NanoPercent); //数值
			NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(84, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
			NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);

		}			
	}
	else if(get_CurDo()->modbus_id == EC_DE40_ModbusID)
	{
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(92, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(100, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);
		LabelList_Add(84, 56, NULL, 0, (uint8_t *)XIAOSHUDIAN,  LABEL_NORMAL, LABEL_PUNCTUATION, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head);

	}
	else
	{
		NanoOptionList_Add(52, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent); //数值
		NanoOptionList_Add(60, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(68, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 0, IS_SINGLE, &NanoPercent);
		NanoOptionList_Add(76, 56, NULL, 0, NULL, NANOOPTION_NUMBER, 0, 0, 9, IS_SINGLE, &NanoPercent);					
	}
	OptionList_Add(0, (uint8_t *)jinrujiaozhunmoshi_cn, sizeof(jinrujiaozhunmoshi_cn), (uint8_t *)jinrujiaozhunmoshi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NanoPercent, &option_head);

	interfacial->option_head = option_head;
	interfacial->label_head = label_head;

		
	if(get_CurDo()->modbus_id == MLSS_Tianjian_ModbusID)
	{
		interfacial->page_father = PAGE_4_PARASET;//设定父界面

		interfacial->content_chn = (uint8_t *)lingdianshezhi_cn;
		interfacial->ChnContent_size = sizeof(lingdianshezhi_cn);
		interfacial->content_eng = (uint8_t *)lingdianshezhi_en;
	}
	else
	{
		interfacial->page_father = PAGE_4_CAL;//设定父界面

		interfacial->content_chn = (uint8_t *)COD_Zero_cn;
		interfacial->ChnContent_size = sizeof(COD_Zero_cn);
		interfacial->content_eng = (uint8_t *)COD_Zero_en;
	}
}



/* 昇辉  COD 吸光度显示*/
void generate_shenghui_COD_Current_absorbance(PtrToInterfacial interfacial)
{
//	set_RowSpacing(44);
	CurInterfacial_Destory();
	
	static uint8_t value1_arr[7] = {0};//设备的第一个值
	static uint8_t value2_arr[7] = {0};//设备的第二个值
	static uint8_t value3_arr[7] = {0};//设备的第三个值
	static uint8_t value4_arr[7] = {0};//设备的第四个值
	
//	get_CurDo()->NH4_Vol.value_f=2.1589;
//	get_CurDo()->pH_Vol.value_f=9.4239;
//	get_CurDo()->compensate_k.value_f=0.7931;
//	get_CurDo()->compensate_b.value_f=1.2555;	
	snprintf((char *)value1_arr, 7, "%2.4f", get_CurDo()->NH4_Vol.value_f);//254电压
	snprintf((char *)value2_arr, 7, "%2.4f", get_CurDo()->pH_Vol.value_f);//365电压
	snprintf((char *)value3_arr, 7, "%2.4f", get_CurDo()->compensate_k.value_f);//254吸光度
	snprintf((char *)value4_arr, 7, "%2.4f", get_CurDo()->compensate_b.value_f);//365吸光度
	
	list_label label_head   = NULL;
	
	LabelList_Add( 0, 36, (uint8_t *)COD_254_cn, sizeof(COD_254_cn), (uint8_t *)COD_254_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //254
	LabelList_Add( 32, 36, (uint8_t *)xiguangdu_cn, sizeof(xiguangdu_cn), (uint8_t *)xiguangdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //吸光度
	LabelList_Add(96, 36,
								NULL, 0, (uint8_t *)value3_arr,
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
							  &label_head); //254吸光度		

	LabelList_Add( 0, 56, (uint8_t *)COD_365_cn, sizeof(COD_365_cn), (uint8_t *)COD_365_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //365	
	LabelList_Add( 32, 56, (uint8_t *)xiguangdu_cn, sizeof(xiguangdu_cn), (uint8_t *)xiguangdu_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //吸光度		
	LabelList_Add(96, 56,
								NULL, 0, (uint8_t *)value4_arr,
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head);  //365吸光度		

	LabelList_Add( 0, 76, (uint8_t *)COD_254_cn, sizeof(COD_254_cn), (uint8_t *)COD_254_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //254
	LabelList_Add( 32, 76, (uint8_t *)dianya_cn, sizeof(dianya_cn), (uint8_t *)dianya_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //电压
	LabelList_Add(96, 76,
								NULL, 0, (uint8_t *)value1_arr,
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head);  //254电压	

	LabelList_Add( 0, 96, (uint8_t *)COD_365_cn, sizeof(COD_365_cn), (uint8_t *)COD_365_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //365
	LabelList_Add( 32, 96, (uint8_t *)dianya_cn, sizeof(dianya_cn), (uint8_t *)dianya_en,  LABEL_NORMAL, LABEL_STRING, UINT_NONE, DONT_HAVE_PARENTHESIS, &label_head); //电压							
	LabelList_Add(96, 96,
								NULL, 0, (uint8_t *)value2_arr,
								LABEL_NORMAL,  LABEL_NUMBERORENG, UINT_NONE, DONT_HAVE_PARENTHESIS,
								&label_head); //365电压
	interfacial->label_head = label_head;

	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)dangqianxiguangdu_cn;
	interfacial->ChnContent_size = sizeof(dangqianxiguangdu_cn);
	interfacial->content_eng = (uint8_t *)dangqianxiguangdu_en;
}

//工厂模式
void generate_FACTORY(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	// OptionList_Add(option_index++, (uint8_t *)zhuoduxianshi_cn, sizeof(zhuoduxianshi_cn), (uint8_t *)zhuoduxianshi_en,     PAGE_4_COD_SET,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	if(get_CurDo()->modbus_id == COD_DC17_ModbusID && get_CurDo()->DC17_Mes_Para.value_f == 1)//连续测量才有
	{
		OptionList_Add(option_index++, (uint8_t *)tiaolingjiaozhun_cn,sizeof(tiaolingjiaozhun_cn),(uint8_t *)tiaolingjiaozhun_en, PAGE_4_INCAL,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	else
	{
		OptionList_Add(option_index++, (uint8_t *)tiaolingjiaozhun_cn,sizeof(tiaolingjiaozhun_cn),(uint8_t *)tiaolingjiaozhun_en, NONE_PAGE,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	}
	OptionList_Add(option_index++, (uint8_t *)xinghaoxuanze_cn,sizeof(xinghaoxuanze_cn),(uint8_t *)xinghaoxuanze_en, PAGE_4_Model_selection,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);

	interfacial->page_father = PAGE_2_SYSTEM;//设定父界面
	
	interfacial->content_chn = (uint8_t *)chuchangmoshi_cn ;
	interfacial->ChnContent_size = sizeof(chuchangmoshi_cn);
	interfacial->content_eng = (uint8_t *)chuchangmoshi_en;
	
	interfacial->option_head = option_head;
}

//DC18 浊度显示开启和关闭设置
void generate_DC18_Set(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)kaiqi_cn,  sizeof(kaiqi_cn),  (uint8_t *)kaiqi_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head); //开启
	OptionList_Add(1, (uint8_t *)guanbi_cn, sizeof(guanbi_cn), (uint8_t *)guanbi_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);	//关闭
	
	interfacial->page_father = PAGE_0_START;//设定父界面
	
	interfacial->content_chn = (uint8_t *)zhuoduxianshi_cn ;
	interfacial->ChnContent_size = sizeof(zhuoduxianshi_cn);
	interfacial->content_eng = (uint8_t *)zhuoduxianshi_en;
	
	interfacial->option_head = option_head;
}


//DC18 内部校准
void generate_INCAL(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头

	OptionList_Add(option_index++, (uint8_t *)COD_zhuodu_zero_cn, sizeof(COD_zhuodu_zero_cn), (uint8_t *)COD_zhuodu_zero_en,     PAGE_5_COD_DC17_IN_Tur_Zero,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(option_index++, (uint8_t *)COD_zero_cn,       sizeof(COD_zero_cn),       (uint8_t *)COD_zero_en, PAGE_5_COD_DC17_IN_Zero,    OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);

	interfacial->page_father = PAGE_4_FACTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)tiaolingjiaozhun_cn ;
	interfacial->ChnContent_size = sizeof(tiaolingjiaozhun_cn);
	interfacial->content_eng = (uint8_t *)tiaolingjiaozhun_en;
	
	interfacial->option_head = option_head;
}

//型号选择
void generate_Model_selection(PtrToInterfacial interfacial)
{
	uint8_t option_index = 0;
	set_RowSpacing(ROWSPACING_MORE);//两个
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(option_index++, (uint8_t *)LH_B580_cn, sizeof(LH_B580_cn), (uint8_t *)LH_B580_en,     NONE_PAGE,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	OptionList_Add(option_index++, (uint8_t *)LH_D702_cn, sizeof(LH_D702_cn), (uint8_t *)LH_D702_en,     NONE_PAGE,        OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);

	interfacial->page_father = PAGE_4_FACTORY;//设定父界面
	
	interfacial->content_chn = (uint8_t *)chuchangmoshi_cn ;
	interfacial->ChnContent_size = sizeof(chuchangmoshi_cn);
	interfacial->content_eng = (uint8_t *)chuchangmoshi_en;
	
	interfacial->option_head = option_head;
}

/*测量单位设置*/
void generate_SetMes_Unit(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)Mlss_mg_cn,   sizeof(Mlss_mg_cn),   (uint8_t *)Mlss_mg_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)Mlss_g_cn,  sizeof(Mlss_g_cn),  (uint8_t *)Mlss_g_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	OptionList_Add(2, (uint8_t *)Mlss_ppm_cn, sizeof(Mlss_ppm_cn), (uint8_t *)Mlss_ppm_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	OptionList_Add(3, (uint8_t *)Mlss_baifen_cn, sizeof(Mlss_baifen_cn), (uint8_t *)Mlss_baifen_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)celiangdanweishezhi_cn ;
	interfacial->ChnContent_size = sizeof(celiangdanweishezhi_cn);
	interfacial->content_eng = (uint8_t *)celiangdanweishezhi_en;
	
	interfacial->option_head = option_head;
}

/*测量档位设置,温度模式设置*/
void generate_SetTemp_para_Mode(PtrToInterfacial interfacial,PAGE_NUM cur_PageNum)
{
	set_RowSpacing(ROWSPACING_FOUR);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	if(cur_PageNum == PAGE_4_Mes_Mode)
	{
		//设置测量档位
		OptionList_Add(0, (uint8_t *)Auto_cn,   sizeof(Auto_cn),   (uint8_t *)Auto_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
		OptionList_Add(1, (uint8_t *)shi_num_cn, sizeof(shi_num_cn), (uint8_t *)shi_num_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
		OptionList_Add(2, (uint8_t *)bai_num_cn, sizeof(bai_num_cn), (uint8_t *)bai_num_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
		interfacial->content_chn = (uint8_t *)celiangdangweishezhi_cn ;
		interfacial->ChnContent_size = sizeof(celiangdanweishezhi_cn);
		interfacial->content_eng = (uint8_t *)celiangdanweishezhi_en;
	}
	else
	{
		OptionList_Add(0, (uint8_t *)ATC_cn,   sizeof(ATC_cn),   (uint8_t *)ATC_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
		OptionList_Add(1, (uint8_t *)MTC_cn, sizeof(MTC_cn), (uint8_t *)MTC_en, NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, DisplayWord_xin, NULL, &option_head);
		interfacial->content_chn = (uint8_t *)wendumoshishezhi_cn ;
		interfacial->ChnContent_size = sizeof(wendumoshishezhi_cn);
		interfacial->content_eng = (uint8_t *)wendumoshishezhi_en;
	}	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	interfacial->option_head = option_head;
}

/*标定点数设置*/
void generate_SetCal_Num(PtrToInterfacial interfacial)
{
	set_RowSpacing(ROWSPACING_FOUR);
	CurInterfacial_Destory();           //清空当前界面所有的链表
	
	list_option option_head = NULL;     //生成一个临时的选项链表头
	
	OptionList_Add(0, (uint8_t *)liangdian_cn,   sizeof(liangdian_cn),   (uint8_t *)liangdian_en,   NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	OptionList_Add(1, (uint8_t *)sandian_cn,  sizeof(sandian_cn),  (uint8_t *)sandian_en,  NONE_PAGE, OPTION_LARGE, CAN_BE_SELECTED, NOT_LANGUAGE_OPTION, NOT_ENG_ONLY, NULL, &option_head);
	
	interfacial->page_father = PAGE_4_PARASET;//设定父界面
	
	interfacial->content_chn = (uint8_t *)biaodingdianshushezhi_cn ;
	interfacial->ChnContent_size = sizeof(biaodingdianshushezhi_cn);
	interfacial->content_eng = (uint8_t *)biaodingdianshushezhi_en;
	
	interfacial->option_head = option_head;
}