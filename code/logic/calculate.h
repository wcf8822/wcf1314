#ifndef __CALCULATE_H
#define __CALCULATE_H

#include "main.h"

/*链队结点结构*/
typedef struct QTempNode
{
	float data;
  struct QTempNode *next;
}QNode;

/*链队列结构*/
typedef struct
{
	uint8_t num;
	QNode *front;
	QNode *rear;
}LiQueue;



/*初始化队列*/
LiQueue* Queue_init(void);
/*队列是否为空判断*/
uint8_t Queue_IsEmpty(LiQueue* lq);

/*删除头节点*/
void Queue_delete(LiQueue* lq);
/*向队列插入数据*/
void Queue_insert(LiQueue *lq, float data, uint8_t maxsize);
/*遍历数据计算平均值*/
void Queue_GetAvg(LiQueue* lq, float* avg);
void Queue_Destroy(LiQueue** lqp);

void Queue_Clear(LiQueue* lq);
















#endif
