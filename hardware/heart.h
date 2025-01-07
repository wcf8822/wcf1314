#ifndef __HEART_H
#define __HEART_H


#include "main.h"

//	uint8_t flag_RefreshLcd : 1; //将缓存刷到屏幕上去
//	uint8_t flag_RefreshRtc : 1; //rtc采集
//	uint8_t flag_RefreshBmp : 1; //采集气压
//	uint8_t flag_RefreshBat : 1; //采集电池ad
//	uint8_t flag_RefreshGPS : 1; //更新gps信息
//	uint8_t flag_interfacial: 1; //将界面刷到缓存中
//	
//	uint8_t flag_Test       : 1; //测试用定时器


#define TIM_REFRESHLCD  1     //屏幕刷新间隔（单位ms）
#define TIM_REFRESHRTC  751   //RTC刷新间隔（单位ms）
#define TIM_REFRESHBMP  2002  //气压温度刷新间隔（单位ms）
#define TIM_REFRESHBAT  1513  //电池电量更新时间
#define TIM_REFRESHGPS  4504  //gps更新时间
#define TIM_INTERFACIAL 250   //显存更新时间  这个应该还能再改小一丢丢但是不能小于等于200 200的话会刷新不全
#define TIM_WARNING     1306  //报警图标闪烁间隔
#define TIM_TEST        2507   //测试用定时器 现在暂时用于遍历链表


#define KEY_TIM 10            //短按消抖时间
#define KEY_TIM_LONG 3000     //长按判断时间
#define KEY_TIM_LONG_LONG 6000     //长按判断时间

#define TIM_SUCCEFFUL 1500    //成功弹窗停留时间

#define LOGO_TIM 1500         //开机logo显示时间

#define BATTERY_TIM 1000      //没电了闪的时间




//main函数相关的flag标志接口
uint8_t get_RtcFlag(void);
uint8_t get_BmpFlag(void);
uint8_t get_LcdFlag(void);
uint8_t get_TestFlag(void);
uint8_t get_BatFlag(void);
uint8_t get_GPSFlag(void);
uint8_t get_InterfacialFlag(void);
uint8_t get_WarningFlag(void);
uint16_t get_SinMesFlag(void);


void clear_RtcFlag(void);
void clear_BmpFlag(void);
void clear_LcdFlag(void);
void clear_TestFlag(void);
void clear_BatFlag(void);
void clear_InterfacialFlag(void);
void clear_GPSFlag(void);
void clear_WarningFlag(void);
void clear_SinMesTimeFlag(void);
void clear_DataFlag(void);
//按键按下开始计时的相关接口                     计时的标志都是CountFlag结尾
void set_KeyUpCountFlag(void);
void set_KeyDownCountFlag(void);
void set_KeyOkCountFlag(void);
void set_KeyEscCountFlag(void);
void set_KeyCalCountFlag(void);
void set_KeyMenuCountFlag(void);
void set_KeyBluCountFlag(void);
void set_KeyOffCountFlag(void);
void set_KeyOffLongCountFlag(void);


uint8_t get_KeyUpCountFlag(void);
uint8_t get_KeyDownCountFlag(void);
uint8_t get_KeyOkCountFlag(void);
uint8_t get_KeyEscCountFlag(void);
uint8_t get_KeyCalCountFlag(void);
uint8_t get_KeyMenuCountFlag(void);
uint8_t get_KeyBluCountFlag(void);
uint8_t get_KeyOffCountFlag(void);
uint8_t get_KeyOffLongCountFlag(void);

uint16_t get_KeyCalCount(void);
uint16_t get_KeyOkCount(void);
uint16_t get_KeyOffCount(void);



void clear_KeyUpCountFlag(void);
void clear_KeyDownCountFlag(void);
void clear_KeyOkCountFlag(void);
void clear_KeyEscCountFlag(void);
void clear_KeyCalCountFlag(void);
void clear_KeyMenuCountFlag(void);
void clear_KeyBluCountFlag(void);
void clear_KeyOffCountFlag(void);
void clear_KeyOffLongCountFlag(void);







//按键被按过的相关接口                           按过的标志都是ClickedFlag结尾
uint8_t get_KeyUpClickedFlag(void);
uint8_t get_KeyDownClickedFlag(void);
uint8_t get_KeyOkClickedFlag(void);
uint8_t get_KeyEscClickedFlag(void);
uint8_t get_KeyCalClickedFlag(void);
uint8_t get_KeyMenuClickedFlag(void);
uint8_t get_KeyBluClickedFlag(void);
uint8_t get_KeyOffClickedFlag(void);
uint8_t get_KeyOffLongClickedFlag(void);

void clear_KeyUpClickedFlag(void);
void clear_KeyDownClickedFlag(void);
void clear_KeyOkClickedFlag(void);
void clear_KeyEscClickedFlag(void);
void clear_KeyCalClickedFlag(void);
void clear_KeyMenuClickedFlag(void);
void clear_KeyBluClickedFlag(void);
void clear_KeyOffClickedFlag(void);
void clear_KeyOffLongClickedFlag(void);





//清空按键按下计时计数的
void clear_KeyUpCount(void);
void clear_KeyDownCount(void);
void clear_KeyEscCount(void);
void clear_KeyOkCount(void);
void clear_KeyCalCount(void);
void clear_KeyMenuCount(void);
void clear_KeyBluCount(void);
void clear_KeyOffCount(void);


void set_SuccessfulTimStartFlag(void);

void clear_WarningCount(void);


extern uint16_t TimeValue;
extern uint8_t MesRun;
#endif



