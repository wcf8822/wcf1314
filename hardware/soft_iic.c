//IOI2C.c
#include "soft_iic.h"

void delay_us(uint32_t us){
	uint32_t i = 0;
	for(i=0;i<us;i++){
		uint8_t a = 10;
		while(a--);
	}
}

void my_SDA_IN(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = RTC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RTC_SDA_GPIO_Port, &GPIO_InitStruct);
}
void my_SDA_OUT(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = RTC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RTC_SDA_GPIO_Port, &GPIO_InitStruct);
}

void my_IIC_SCL(GPIO_PinState val){
	HAL_GPIO_WritePin(RTC_SCL_GPIO_Port,RTC_SCL_Pin,val);
}

void my_IIC_SDA(GPIO_PinState val){
	HAL_GPIO_WritePin(RTC_SDA_GPIO_Port,RTC_SDA_Pin,val);
}

uint8_t my_READ_SDA(void){
	return HAL_GPIO_ReadPin(RTC_SDA_GPIO_Port,RTC_SDA_Pin);
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	my_IIC_SDA(GPIO_PIN_SET);	  	  
	my_IIC_SCL(GPIO_PIN_SET);
	delay_us(4);
 	my_IIC_SDA(GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	my_IIC_SCL(GPIO_PIN_RESET);//钳住I2C总线，准备发送或接收数据 
}	  

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	my_IIC_SCL(GPIO_PIN_RESET);
	my_IIC_SDA(GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	my_IIC_SCL(GPIO_PIN_SET); 
	my_IIC_SDA(GPIO_PIN_SET);//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	my_IIC_SDA(GPIO_PIN_SET);delay_us(1);	   
	my_IIC_SCL(GPIO_PIN_SET);delay_us(1);	 
	while(my_READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	my_IIC_SCL(GPIO_PIN_RESET);//时钟输出0 	   
	return 0;  
} 

//产生ACK应答
void IIC_Ack(void)
{
	my_IIC_SCL(GPIO_PIN_RESET);
	SDA_OUT();
	my_IIC_SDA(GPIO_PIN_RESET);
	delay_us(2);
	my_IIC_SCL(GPIO_PIN_SET);
	delay_us(2);
	my_IIC_SCL(GPIO_PIN_RESET);
}

//不产生ACK应答		    
void IIC_NAck(void)
{
	my_IIC_SCL(GPIO_PIN_RESET);
	SDA_OUT();
	my_IIC_SDA(GPIO_PIN_SET);
	delay_us(2);
	my_IIC_SCL(GPIO_PIN_SET);
	delay_us(2);
	my_IIC_SCL(GPIO_PIN_RESET);
}					 	

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
	uint8_t t;   
	SDA_OUT(); 	    
	my_IIC_SCL(GPIO_PIN_RESET);//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
		//IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
		{
			my_IIC_SDA(GPIO_PIN_SET);
		}
		else
		{
			my_IIC_SDA(GPIO_PIN_RESET);
		}
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		my_IIC_SCL(GPIO_PIN_SET);
		delay_us(2); 
		my_IIC_SCL(GPIO_PIN_RESET);	
		delay_us(2);
	}	 
} 	    

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		my_IIC_SCL(GPIO_PIN_RESET); 
		delay_us(2);
		my_IIC_SCL(GPIO_PIN_SET);
		receive<<=1;
		if(my_READ_SDA())
		{
			receive++;
		}
		delay_us(1); 
	}					 
	if (!ack)
	{
		IIC_NAck();//发送nACK
	}
	else
	{
		IIC_Ack(); //发送ACK   
	}
	return receive;
}
