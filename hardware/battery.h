#ifndef __BATTERY_H
#define __BATTERY_H

#include "main.h"

#define BATTERY_TIMES 10 //电池ad采样次数

/*不同档位ad阈值*/
#define BATTERY_LAVEL_1 3.6
#define BATTERY_LAVEL_2 4.1
#define BATTERY_LAVEL_3 4.6
#define BATTERY_LAVEL_USB 0.6 
/*不同档位电池显示的图标*/
#define BATTERY_LAVEL_ARRNONE battery_image[0]
#define BATTERY_LAVEL_ARR0    battery_image[1]
#define BATTERY_LAVEL_ARR1    battery_image[2]
#define BATTERY_LAVEL_ARR2    battery_image[3]
#define BATTERY_LAVEL_ARR3    battery_image[4]
#define BATTERY_LAVEL_ARRUSB  battery_image[5]

typedef struct {
	uint8_t bat_lvlnone :1;
	uint8_t bat_lvl1:1;
	uint8_t bat_lvl2:1;
	uint8_t bat_lvl3:1;
}bat_flag_t;



double get_bat(void);

double get_bat_avg(uint8_t times);

void battery_draw(float ad_value);











#endif






