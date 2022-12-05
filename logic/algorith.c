#include "algorith.h"
#include <stdlib.h>
#include "math.h"
#include "string.h"

/////这个文件主要是一些算法

/////////////////////////////////////////////////////////////////滑动平均

const double eps = 1e-8;

#define MoreEqu(a, b) (((a) - (b)) > (-eps))
#define LessEqu(a, b) (((a) - (b)) < (eps))


/*内部函数用来计算数组平均值*/
static float arr_avg(float* arr, uint8_t len)
{
    float sum = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        sum += arr[i];
    }
    return sum / len;
}

/*根据结构体来计算平均值*/
float filter_get_avg(filter_t* p2filter)
{
	return p2filter->is_fulled ? arr_avg(p2filter->arr, p2filter->max_len) : arr_avg(p2filter->arr, p2filter->index);
}

/*添加数据到结构体中*/
void filter_inset2arr(filter_t* p2filter, float value)
{
	if(p2filter->index >= p2filter->max_len)
	{
		p2filter->index = 0;
		p2filter->is_fulled = 1;
	}
	p2filter->arr[p2filter->index++] = value;
}

/*初始化结构体*/
void filter_init(filter_t* p2filter, uint16_t max_size)
{
	p2filter->max_len = max_size;
	
	p2filter->index = 0;
	p2filter->is_fulled = 0;
	
	if(p2filter->arr != NULL)
	{//如果之前有指向数组的话重新变更下大小
		p2filter->arr = (float*)realloc(p2filter->arr, sizeof(float) * p2filter->max_len);
	}
	else
	{
		p2filter->arr = (float*)calloc(p2filter->max_len, sizeof(float));
	}
}

/*清除结构体*/
void filter_destroy(filter_t* p2filter)
{
	if(p2filter->arr != NULL)
	{
		free(p2filter->arr);
	}
	p2filter->arr = NULL;
	p2filter->index = 0;
	p2filter->is_fulled = 0;
}

/*将结构体全置0*/
void filter_clear(filter_t* p2filter)
{
	memset(p2filter->arr, 0, sizeof(float) * p2filter->max_len);
	p2filter->index = 0;//让下标从0开始
	p2filter->is_fulled = 0;//清除当前是满的状态
}
