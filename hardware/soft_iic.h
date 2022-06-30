//IOI2C.h
#ifndef __IOI2C_H
#define __IOI2C_H

#include "main.h"

//IO方向设置 
#define SDA_IN()  my_SDA_IN()
#define SDA_OUT() my_SDA_OUT()

void my_SDA_IN(void);
void my_SDA_OUT(void);

//IIC所有操作函数 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

#endif
