#ifndef __INTERFACIAL_H
#define __INTERFACIAL_H


#include "gui.h"

#include "label.h"
#include "option.h"
#include "nano_option.h"

#include "rs485.h"

//#define MSGISRESRT  1
//#define MSGNOTRESET 0

#define PAGE_NOT_BACK 0 //不是从子界面来的
#define PAGE_IS_BACK  1 //是从子界面来的

//标签是指那些只是用来显示的
//选项是指那些能用选中的

//界面结构体
typedef struct struct_interfacial{
	uint8_t ChnContent_size;        //中文长度
	
	PAGE_NUM page_father;           //这个界面的父界面是哪个界面
	
	list_NanoOption NanoOption_msg; //弹窗是否两个选项
	
	list_label label_head;          //这个界面上的标签的头指针
	list_option option_head;        //这个界面上的一级选项的头指针  
	
	uint8_t* content_chn;           //中文标签
	uint8_t* content_eng;           //英文标签
	
}Interfacial_t;

typedef enum {
	MESSAGE_SETTING,                //设置弹窗
	MESSAGE_SAVELOG,                //保存数据弹窗
	MESSAGE_SUCCESSFUL,             //成功弹窗
	MESSAGE_DELETE,		            //删除数据弹窗
	MESSAGE_tixing,					//提醒信息弹窗
	MESSAGE_Close,					//关机信息弹窗

}MESSAGE_TYPE;

typedef enum{
	VER_A = 0,                      //低低硬件版本a
	VER_B,
	VER_C,
	VER_D

}HARDWARE_VERSION;

typedef Interfacial_t* PtrToInterfacial;

void interfacial_refresh(void);//刷新界面

void btn_func(void);          //得在main中调用的按键函数


void interfacial_Destory(PtrToInterfacial interfacial);

void CurInterfacial_Destory(void);

void interfacial_SetPage(PAGE_NUM page_num, uint8_t IsBack);

PAGE_NUM interfacial_GetCurPage(void);
PAGE_NUM interfacial_GetTempFatherPage(void);

HARDWARE_VERSION interfacial_GetHWVersion(void);
void interfacial_SetHWVersion(HARDWARE_VERSION hwv);
uint8_t* interfacial_GetSWVersion(void);
	
void  set_PressArr(double press);
char* get_PressArr(void);
void  set_SalArr(float sal);
char* get_SalArr(void);
uint8_t *interfacial_GetCurLogIndexArr(void);
void interfacial_UpdateCurLogIndexArr(uint16_t data_index);



void interfacial_InitMsg(void);

Interfacial_t* interfacial_GetCurrentInterfacial(void);
PtrToOptionNode interfacial_GetCurrentOption(void);
uint8_t* interfacial_GetOptionSensorName(void);
void interfacial_SetOptionSensorName(uint8_t* value);

uint8_t interfacial_GetSalChangedFlag(void);
void interfacial_ClearSalChangedFlag(void);

void interfacial_ClearLabel(void);

void interfacial_ClearMessageBoxFlag(void);

void interfacial_SetNeedDestroyMSG(void);
void interfacial_ClearNeedDestroyMSG(void);
uint8_t interfacial_GetNeedDestroyMSG(void);

uint8_t interfacial_GetNeedWarning(void);
void interfacial_ClearNeedWarning(void);

void destory_MessageBox(void);
uint8_t interfacial_GetMessageBoxFlag(void);
void no_signal(void);
void clear_NoSignal(void);

void generate_MessageBox(MESSAGE_TYPE msg_type, uint8_t is_successful);

void StatusBar_Update(void);

MESSAGE_TYPE interfacial_GetCurMsgType(void);

extern uint8_t GetCircularSent_Flag;

void rs485_Search_Sensor(void);

extern SENSOR_TYPE datashow_SensorType;


#define Auto_Search_Time 10
extern uint16_t Auto_Search_Count;
#endif




