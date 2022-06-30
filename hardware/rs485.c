#include "rs485.h"

//STATIC uint8_t rs485_circular_sent = 0;
//STATIC uint8_t rs485_need_sent = 0;

//STATIC rs485_sent_type sent_type = none_type;

//STATIC uint8_t device_count = 0; //已连接设备计数
//STATIC uint8_t resend_count = 0;//重发计数

STATIC uint8_t is_change_senesor = 0;

STATIC connected_probe_t connected_probe = {
	.DO_list = NULL,
	.current_sensor_type = TYPE_NONE
};


void rs485_SetIsChangeSenesor(void)
{
	is_change_senesor = 1;
}
uint8_t rs485_GetIsChangeSenesor(void)
{
	return is_change_senesor;
}
void rs485_ClearIsChangeSenesor(void)
{
	is_change_senesor = 0;
}

/*当前连接的设备类型*/
void rs485_SetSensorType(SENSOR_TYPE st)//设置当前传感器类型
{
	connected_probe.current_sensor_type = st;
}
SENSOR_TYPE rs485_GetSensorType(void)//获取当前传感器类型
{
	return connected_probe.current_sensor_type;
}




/*外部获取重发次数*/
uint8_t get_ResendCount(void)
{
	return rs485_usart.resend_count;
}
uint8_t rs485_GetDeviceCount(void)
{
	return rs485_usart.device_count;
}
void rs485_DevicePlus(void)
{
	rs485_usart.device_count++;
}
void rs485_DeviceReduce(void)
{
	rs485_usart.device_count--;
}

void rs485_SetSentType(rs485_sent_type rst)
{
	rs485_usart.sent_type = rst;
}
rs485_sent_type rs485_GetSentType(void)
{
	return rs485_usart.sent_type;
}


void rs485_ClearRxFlag(void)
{
	rs485_usart.rx_flag = 0;
}
void rs485_SetRxFlag(void)
{
	rs485_usart.rx_flag = 1;
}
uint8_t rs485_GetRxFlag(void)
{
	return rs485_usart.rx_flag;
}



uint8_t rs485_GetCircularSentStatus(void)
{
	return rs485_usart.rs485_circular_sent;
}
void rs485_SetCircularSentStatus(void)     //置这个标志会循环发送
{
	rs485_usart.rs485_circular_sent = 1;
}
void rs485_ClearCircularSentStatus(void)
{
	rs485_usart.rs485_circular_sent = 0;
}


uint8_t rs485_GetNeedSendStatus(void)
{
	return rs485_usart.rs485_need_sent;
}
void rs485_SetNeedSendStatus(void)          //置这个标志会发送一次
{
	rs485_usart.rs485_need_sent = 1;
}
void rs485_ClearNeedSendStatus(void)
{
	rs485_usart.rs485_need_sent = 0;
}

void rs485_SetIsDisconnect(void)
{
	rs485_usart.rs485_is_disconnect = 1;
}
void rs485_ClearIsDisconnect(void)
{
	rs485_usart.rs485_is_disconnect = 0;
}
uint8_t rs485_GetIsDisconnect(void)
{
	return rs485_usart.rs485_is_disconnect;
}

PtrToDOProbe* rs485_GetDoList(void) //获取已连接设备的do设备列表
{
	return &(connected_probe.DO_list);
}

void rs485_UartInit(UART_HandleTypeDef *huart);

 rs485_t rs485_usart = {
	.init = rs485_UartInit,
	.tx_flag = 0,
	.rx_flag = 0,
	.rs485_circular_sent = 0,
	.rs485_need_sent = 0,
	.sent_type = DO_SendType_None,
	.device_count = 0, //已连接设备计数
	.resend_count = 0,  //重发计数
	.rs485_is_disconnect = 0
	 
};

void rs485_UartInit(UART_HandleTypeDef *huart)
{
	
	rs485_usart.huart = huart;
	__HAL_UART_ENABLE_IT(rs485_usart.huart, UART_IT_IDLE);      //开启空闲中断
	HAL_UART_Receive_DMA(rs485_usart.huart, rs485_usart.rx_buf, RS485_RXBUFFSIZE);//开始dma接收
}


void rs485_RxCallBack(UART_HandleTypeDef *huart)
{
	if(rs485_usart.huart == huart)
	{
		
	}
}


/*485串口的空闲中断*/
void rs485_IDLECallBack(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance == RS485_USART)
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);		//清中断

		HAL_UART_AbortReceive(huart);	//已经接收完一帧数据,所以这里要停止接收,然后再重新接收		
		
		rs485_usart.rx_size = RS485_RXBUFFSIZE - hdma_usart3_rx.Instance->CNDTR;  //接收到多少数据
		
		rs485_SetRxFlag();
		
		HAL_UART_Receive_DMA(huart, rs485_usart.rx_buf, RS485_RXBUFFSIZE); //开启DMA接收
	}
}

/*清除重发计数*/
void rs485_ClearResendCount(void)
{
	rs485_usart.resend_count = 0;
}



/*清空接收buf*/
void rs485_ClearRxBuf(void)
{
	clear_buf(rs485_usart.rx_buf, RS485_RXBUFFSIZE);
}

void rs485_send(void)
{
	RS485_DE_H();
	for(uint16_t i=1000;i>0;i--);
	
	HAL_UART_Transmit(rs485_usart.huart, rs485_usart.tx_buf, rs485_usart.tx_size,200);
	rs485_usart.tx_flag = 1;
	for(uint16_t i=1000;i>0;i--);
	RS485_DE_L();
}

/*将发送buf发送出去*/
void rs485_SendBuf(void)//这个也是在main函数里面运行的
{
	
	if(rs485_GetSentType() != DO_SendType_GetModbusId && ++rs485_usart.resend_count >= RESEND_MAX)//当不是搜索设备的时候发送次数超过设定值 就放弃这个设备
	{
		rs485_ClearCircularSentStatus(); //关闭循环发送
		rs485_ClearResendCount();    //清计数
		rs485_SetIsDisconnect();
		return;//不让它再发了
	}
	
	rs485_send();
	
}


STATIC uint16_t rs485_tim_count = 0;

void rs485_ClearRs485Tim(void)
{
	rs485_tim_count = 0;
}

void rs485_TimHandle(void)
{
	if(rs485_GetCircularSentStatus() && ++rs485_tim_count >= RS485_CIRCULAR_TIM)
	{
		rs485_ClearRs485Tim(); //清定时器计数
		rs485_SetNeedSendStatus();//设置需要有东西发送
	}
	
}

float float_format(float data)
{
	return (int)((data*100) + 0.5)/100.0;
}







