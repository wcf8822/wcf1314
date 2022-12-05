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

#define IS_ENG_ONLY  1
#define NOT_ENG_ONLY 0

#define IS_SINGLE    1
#define NOT_SINGLE   0


/*枚举界面*/
typedef enum{
	NONE_PAGE = 0,           //没有后续界面
	
	PAGE_0_START,          //开机初始界面
	
	/*一级界面*/
	PAGE_1_MENU,           //menu界面
	
	PAGE_1_RESETCAL,
	
	/*二级界面*/
	PAGE_2_SENSORMANAGE,  //传感器管理
	PAGE_2_SYSTEM,         //系统设置界面
	PAGE_2_HISTORY,        //历史数据界面
	
	/*三级界面*/
	PAGE_3_SENSORSSEARCH,  //自动搜索设备
	PAGE_3_SENSORS,        //传感器信息
	
	
	PAGE_3_TIME,           //时间设置
	PAGE_3_BEEP,           //按键音
	PAGE_3_LANGUAGE,       //语言设置
	PAGE_3_AUTOSHUT,       //自动关机
	PAGE_3_ALARM_TYPE,     //报警设置已接设备列表
	PAGE_3_PRESSURE,       //气压设置
	PAGE_3_SALT,           //盐度设置
	PAGE_3_GPS,            //gps
	PAGE_3_INFO,           //仪表信息
	PAGE_3_RESERT,         //恢复出厂设置
	PAGE_3_AUTOLOCK_TYPE,  //显示需要自动锁定的传感器类型
	
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
	
	PAGE_4_SENSORINFO,    //传感器信息
	PAGE_4_CAL,           //校准
	PAGE_4_PARASET,       //参数设置
	
	PAGE_4_SLIDEAVG,     //滑动平均开启关闭设置
	
	/*五级界面*/
	PAGE_5_ALARMVALUE,     //报警阈值设置
	
	PAGE_5_AUTOLOCKVALUE,  //锁定等级设置
	
	PAGE_5_SLIDEVALUE,     //滑动平均数值设置
	
	PAGE_5_TEMP,           //温度校准
	
	PAGE_5_ZERO,           //零点
	PAGE_5_ONE,            //单点
	PAGE_5_TWOFIRST,       //两点校准第一个点
	PAGE_5_TWOSECOND,      //两点校准第二个点
	PAGE_5_THREE,          //三点
	
	PAGE_5_SENSORCAP,      //帽膜设置
	
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
	uint8_t IsEngOnly  :1;                 //是否只显示英语（设备标签）
	
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
	NANOOPTION_BUTTON
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



