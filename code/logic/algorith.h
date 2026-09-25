#ifndef __ALGORITH_H
#define __ALGORITH_H

#include "main.h"


/*算法的载体*/
typedef struct 
{
	uint8_t is_fulled;
	uint16_t max_len;
	uint16_t index;
	float* arr;
}filter_t;

///////////////////////////////////////////////////滑动平均
float filter_get_avg(filter_t* p2filter);
/*添加数据到结构体中*/
void filter_inset2arr(filter_t* p2filter, float value);
/*初始化结构体*/
void filter_init(filter_t* p2filter, uint16_t max_size);
/*清除数据载体*/
void filter_destroy(filter_t* p2filter);
/*清除数据*/
void filter_clear(filter_t* p2filter);

#endif


