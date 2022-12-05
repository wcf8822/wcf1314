#ifndef __RTC_H
#define __RTC_H

#include "soft_iic.h"




typedef struct{
	uint8_t seconds;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t week;
	uint8_t month;
	uint8_t years;
}datetime_t;


void HYM8563_init(void);


void HYM8563_SetTime(datetime_t *p);
void HYM8563_UpdateTime(void);

void RTC_UpdateShutDownTime(uint8_t autoshut);


//获取时间相关参数
uint8_t RTC_GetYear(void);
uint8_t RTC_GetMonth(void);
uint8_t RTC_GetDay(void);
uint8_t RTC_GetHour(void);
uint8_t RTC_GetMinute(void);
uint8_t RTC_GetSecond(void);
uint8_t RTC_GetWeek(void);

void RTC_AutoShut(void);

#endif




