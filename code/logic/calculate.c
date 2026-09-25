#include "calculate.h"
#include "setting.h"
#include <stdlib.h>

/*初始化队列*/
LiQueue* Queue_init(void)
{
	LiQueue *lq = (LiQueue*)malloc(sizeof(LiQueue));
	if(lq == NULL)
	{
		return NULL;
	}
    lq->num = 0;
	lq->front = lq->rear = NULL;
	return lq;
}

/*队列是否为空判断*/
uint8_t Queue_IsEmpty(LiQueue* lq)
{
	if(lq == NULL)
	{
		return 1;
	}
	return ((lq->front == NULL || lq->rear == NULL) ? 1 : 0);
}

/*删除头节点*/
void Queue_delete(LiQueue* lq)
{
	QNode *p = lq->front;
	
	if(Queue_IsEmpty(lq))
	{
		return; //安全性判断如果是空队列的话直接返回
	}
	if(lq->front == lq->rear)//如果队列里只有一个节点的话
	{
		lq->front = lq->rear = NULL;
	}
	else
	{
		lq->front = lq->front->next;
	}
	free(p);
}

/*向队列插入数据*/
void Queue_insert(LiQueue *lq, float data, uint8_t maxsize)
{
	QNode *p = NULL;
    
	if(lq == NULL)
	{
			return;
	}

	p = (QNode*)malloc(sizeof(QNode));
	
	if(p == NULL)
	{
		return;
	}
	
	p->data = data;
	p->next = NULL;
	if(lq->rear == NULL)
	{
		lq->front = lq->rear = p;
	}
	else
	{
		lq->rear->next = p;
		lq->rear = p;
	}

	if(++lq->num > maxsize)//如果超过最大容量的话就出队列
	{
		while(lq->num > maxsize)
		{
      lq->num--;
			Queue_delete(lq);
		}
	}
}

/*遍历数据计算平均值*/
/*这个要手动添加一下安全性判断 就是使用前得先判断下是否为空然后在if括号里写这个*/
void Queue_GetAvg(LiQueue* lq, float* avg)
{
	float sum = 0;
	QNode* p = lq->front;

	if(Queue_IsEmpty(lq))
	{
		return; //安全性判断如果是空队列的话直接返回
	}

	while(p != NULL)
	{
    sum += p->data;
		p = p->next;
	}
	
	*avg = sum/lq->num;
}
	
/*这里得用指针的指针方便把指针给置空*/
void Queue_Destroy(LiQueue** lqp)
{
//	QNode* p = NULL;
	if(*lqp == NULL)//安全性判断 防止传入空指针
	{
		return;
	}
	
	while((*lqp)->rear != NULL)
	{
		Queue_delete((*lqp));
	}
	
	free(*lqp);
	*lqp = NULL;//指针指向空
}

void Queue_Clear(LiQueue* lq)
{
	if(lq == NULL)
	{
		return;
	}
	lq->num = 0;
	while(lq->rear != NULL)
	{
		Queue_delete(lq);
	}
}










