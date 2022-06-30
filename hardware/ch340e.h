#ifndef __CH340E_H
#define __CH340E_H

#include "main.h"
#include "usart.h"
#include <stdio.h>


#define CH340E_USART USART1

#define CH340E_RXBUFFSIZE 20   //ch340e的读取缓存buf最大值
#define CH340E_TXBUFFSIZE     //ch340e的写入缓存buf最大值

//串口数据结构
typedef struct
{
	uint8_t receive_flag : 1;                   //ch340e接收到数据标志
	
	uint8_t rx_size;                          //接收buf大小
	
	uint8_t rx_buf[CH340E_RXBUFFSIZE];          //ch340读取数据buf
	
	void (*init)(UART_HandleTypeDef *huart);    //ch340初始化函数指针
	UART_HandleTypeDef *huart;                  //ch340使用的是哪个串口
	
	
}ch340e_t;

extern ch340e_t ch340e_usart;


void ch340e_RxCallBack(UART_HandleTypeDef *huart);
void ch340e_IDLECallBack(UART_HandleTypeDef *huart);
void clear_ch340erx_buf(void);


void ch340_DataHandle(void);







#endif




