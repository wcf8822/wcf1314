#ifndef __GUI_H
#define __GUI_H


#include "main.h"
#include "stdint.h"
#include "stdlib.h"



#define  GUI_LCM_XMAX		160							/* 定义液晶x轴的点数 */
#define  GUI_LCM_YMAX		160							/* 定义液晶y轴的点数 */

/*是否是数字*/
#define IS_NUMBER    1
#define NOT_NUMBER   0

/*是否是单个字符的标点符号*/
#define IS_SYMBOL    1
#define NOT_SYMBOL   0

#define IS_STR       1
#define NOT_STR      0

/*是否是单位*/
#define IS_UINT      1
#define NOT_UINT     0


#define DisplayWord_xin  2
#define IS_ENG_ONLY  1
#define NOT_ENG_ONLY 0


#define IS_SINGLE    1
#define NOT_SINGLE   0


/*枚举界面*/
typedef enum{
	NONE_PAGE = 0,         //没有后续界面
	
	PAGE_0_START,          //开机初始界面
	
	/*一级界面*/
	PAGE_1_MENU,           //menu界面
	
	PAGE_1_RESETCAL,       //重置校准错参数
	
	/*二级界面*/
	PAGE_2_SENSORMANAGE,   //传感器管理
	PAGE_2_SYSTEM,         //系统设置界面
	PAGE_2_HISTORY_ShowOPTION,	
	PAGE_2_HISTORY_DeleteOPTION,	
	PAGE_2_HISTORY,        //历史数据界面

	/*三级界面*/
	PAGE_3_SENSORSSEARCH,  //自动搜索设备
	PAGE_3_SENSORS,        //传感器信息
	
	PAGE_3_TIME,           //时间设置
	PAGE_3_BEEP,           //按键音
	PAGE_3_LANGUAGE,       //语言设置
	PAGE_3_AUTOSHUT,       //自动关机
  	PAGE_3_AUTOSAVE,      //自动保存
	PAGE_3_ALARM_TYPE,     //报警设置已接设备列表
	PAGE_3_PRESSURE,       //气压设置
	PAGE_3_SALT,           //盐度设置
	PAGE_3_GPS,            //gps
  	PAGE_3_UNIT,		    //单位设置
	PAGE_3_INFO,           //仪表信息
	PAGE_3_RESERT,         //恢复出厂设置
	PAGE_3_Set_TDS_coefficient,  //电导率tds系数设置
	PAGE_3_Set_Temp_coefficient,  //电导率温度系数设置
	PAGE_3_Set_Sal_uint,  //盐度单位设置

	PAGE_3_DATASAVE,       //数据保存
	PAGE_3_DATASHOW,       //数据显示
	PAGE_3_DATADELETE,     //数据删除

	PAGE_3_LOG,            //日志
	
	PAGE_3_SLIDEAVG_TYPE,  //显示需要滑动平均的传感器类型
	
	/*四级界面*/
	PAGE_4_ALARMTONE,      //报警音
	PAGE_4_KEYPADTONE,		 //按键音
	
	PAGE_4_ALARM,          //不同类型选择点进去之后的界面
	PAGE_4_AUTOLOCK,       //不同类型选择点进去之后的界面
	
	PAGE_4_DATALOG,        //数据日志
	PAGE_4_DATALOG_GPS,    //数据日志gps界面
	
	PAGE_4_SENSORINFO,     //传感器信息
	PAGE_4_CAL,                          //校准
	PAGE_4_PARASET,        //参数设置

	PAGE_4_MesUNIT,		   //测量单位设置
	PAGE_4_Cal_Num_Set,		   //标定点数设置
	PAGE_4_Mes_Mode,		   //测量档位设置
	PAGE_4_Set_Mode_Temp,		   //温度模式设置
	PAGE_4_Temp_k,		   //温度补偿系数
	PAGE_4_Clear_time,		   //清洁周期
	PAGE_4_Temp_xiaoshu,		   //温度精度设置
	PAGE_4_Cal_Info,		   //校准系数
	PAGE_4_SLIDEAVG,       //滑动平均开启关闭设置
	
	PAGE_4_CAL_GROUP, //pH 标液组管理
		
	PAGE_4_Measure_Switch, //Chl 量程设置	

	PAGE_4_Current_absorbance, //COD的吸光度显示	
	PAGE_4_COD_SET, //COD 浊度显示开启和关闭	
	
	PAGE_4_Mes_Set,		 //测量模式设置
	PAGE_4_Mes_Time_Set, //单次测量时间设置
	PAGE_4_INTERVALSAVE, //间隔保存
	PAGE_4_FACTORY,		 //工厂模式
	PAGE_4_INCAL,	 //内部校准
	PAGE_4_Model_selection,	 //型号选择
	PAGE_4_LOCKSAVE,  //锁定保存
	
	/*五级界面*/
	PAGE_5_ALARMVALUE,     //报警阈值设置
	
	PAGE_5_AUTOLOCKVALUE,  //锁定等级设置
	
	PAGE_5_SLIDEVALUE,     //滑动平均数值设置
	
	PAGE_5_TEMP,           //温度校准
	
	//DO	
	PAGE_5_DO_ONE_First,            //单点
	PAGE_5_DO_TWO_FIRST,       // 连续校准两点的第1点
	PAGE_5_DO_TWO_SECOND,      //连续校准两点的第2点
	PAGE_5_MLSS_zero_signal,
	PAGE_5_OIW_K_CAL,//斜率校准
	PAGE_5_SENSORCAP,      //帽膜设置
	
	//NH3N
  	PAGE_5_NH3N_ONE,    //氨氮第一点校准
  	PAGE_5_NH3N_TWO,   //氨氮第二点校准
	PAGE_5_NH3N_pH_ONE, //氨氮的pH第一点4.01校准
	PAGE_5_NH3N_pH_TWO,//氨氮的pH第二点6.86校准
	PAGE_5_NH3N_pH_THREE,//氨氮的pH第三点9.18校准
	
	//COD
	PAGE_5_COD_shenghui_Zero,   //昇辉COD的零点校准
	PAGE_5_COD_shenghui_Tur_ONE,//昇辉COD的 浊度第一点校准
	PAGE_5_COD_shenghui_Tur_TWO,//昇辉COD的 浊度第二点校准
	PAGE_5_COD_shenghui_ONE, //昇辉COD的 第一点校准
	PAGE_5_COD_shenghui_TWO,//昇辉COD的 第二点校准
	PAGE_5_COD_shenghui_THREE,	//昇辉COD的 第三点校准
	
	PAGE_5_COD_DC18_Tur_Zero, //DC18 COD的浊度零点校准
	PAGE_5_COD_DC18_Tur_Slope,//DC18 COD的浊度斜率校准
	PAGE_5_COD_DC18_Zero,//DC18 COD的零点校准
	PAGE_5_COD_DC18_Slope,//DC18 COD的斜率校准
	PAGE_5_COD_DC18_Coefficient,	//DC18 COD的补偿系数

	PAGE_5_COD_DC17_IN_Tur_Zero, //DC17 COD的浊度零点校准
	PAGE_5_COD_DC17_IN_Zero,//DC17 COD的内部零点校准
	
	PAGE_5_COD_Cleanse,		  //清洗界面
	
	//昇辉 Tur
	PAGE_5_shenghui_Tur_ONE,//昇辉 浊度第一点校准
	PAGE_5_shenghui_Tur_TWO,//昇辉 浊度第二点校准
	PAGE_5_shenghui_Tur_THREE,//昇辉 浊度第三点校准
		
	//昇辉 EC
	PAGE_5_shenghui_EC_ONE,//昇辉 电导率校准 
	//昇辉 BGA
	PAGE_5_shenghui_BGA_ONE,//昇辉 BGA标定点1校准
	PAGE_5_shenghui_BGA_TWO,//昇辉 BGA标定点2校准
	//DR31 ORP
	PAGE_5_DR31_ORP_ONE,   //DR31_ORP单点校准
	//自研DE26 EC
	PAGE_5_DE26_EC_Zero,  // 电导率零点校准 
	
} PAGE_NUM;

/*枚举温度单位*/
typedef enum
{
	CELSIUS=0,
	FAHRENHEIT
}TEMP_UINT;

typedef uint8_t TCOLOR;

struct struct_NanoOption;

//////////////////////那种大选项的结构体
typedef struct struct_option{
	uint8_t IsSelected : 1;                //是否被选中了
	uint8_t IsLarge : 1;                   //是否是那种大的选项要刷一条横的
	uint8_t IsCanBeSelected :1;            //能否被选中
	uint8_t IsLanguage :1;                 //是否是那种语言标签
	uint8_t IsEngOnly;                 //是否只显示英语（设备标签）
	
	uint8_t ChnContent_size;               //中文标签的长度
	uint8_t option_index;                  //选项下标
	
	PAGE_NUM next_page;                    //如果被选中按确定后跳转的界面
	
	uint8_t* content_chn;                  //显示的中文内容
	uint8_t* content_eng;                  //显示的英文内容
	struct struct_option* next_option;     //为了方便下一个选项的指针
	struct struct_option* prev_option;     //为了方便上一个选项的指针
	struct struct_NanoOption* son_option;  //子标签的指针
	
}node_option;

typedef node_option* PtrToOptionNode;    //选项的指针类型声明
typedef PtrToOptionNode list_option;     // 选项的表头类型声明



typedef enum{
	NANOOPTION_NUMBER,
	NANOOPTION_UINT,
	NANOOPTION_BUTTON,
	NANOOPTION_STRING
}NANOOPTION_TYPE;

/////////////////////那种小选项的结构体（例如：修改时间时候的每一位，保存的小标签）
typedef struct struct_NanoOption{
	uint8_t IsSelected : 1;                    //是否被选中了
	uint8_t IsSingle   : 1;                    //是否是单一一位的数字（用于补0）
	
//	uint8_t IsNumber   : 1;                    //是否是数字
//	uint8_t IsUint     : 1;
	
	NANOOPTION_TYPE NanoOption_type;
	
	uint8_t x;                                 //字显示的起始x轴坐标
	uint8_t y;                                 //字显示的起始y轴坐标
	uint8_t UintIndex;
	uint8_t ChnContent_size;                   //中文标签的长度
	uint8_t value;
	uint8_t min_value;
	uint8_t max_value;
	
	uint8_t* content_chn;                      //显示的中文内容
	uint8_t* content_eng;                      //显示的英文内容
	
	struct struct_NanoOption* next_option;     //为了方便下一个选项的指针
	struct struct_NanoOption* prev_option;     //为了方便上一个选项的指针
	struct struct_option*     father_option;   //声明子设置子设置项链表的的父设置项链表
}node_nano;

typedef node_nano* PtrToNanoOptionNode;      //子选项的指针类型说明
typedef PtrToNanoOptionNode list_NanoOption; //子选项的表头类型声明







#endif



