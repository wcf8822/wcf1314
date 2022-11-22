#ifndef __RS485_H
#define __RS485_H

#include "main.h"
#include "usart.h"
#include "calculate.h"

#define RS485_USART USART3

#define RS485_RXBUFFSIZE 20   //485的读取缓存buf最大值
#define RS485_TXBUFFSIZE 42   //485的写入缓存buf最大值

#define RS485_DE_H()    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)
#define RS485_DE_L()    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)

#define RS485_CIRCULAR_TIM 850  //循环发送的话多久发一次

#define RESEND_MAX 4 //最大重发数，超过这个数就认为设备断开连接了


//	uint8_t IsAlarm_DO;        //是否报警
//	uint8_t IsAlarm_pH;        //是否报警
//	uint8_t IsAlarm_Tur;        //是否报警

//	uint8_t IsAlarm_FCL;        //是否报警
//	uint8_t IsAlarm_EC;        //是否报警

//	uint8_t IsAlarm_ORP;        //是否报警
//	uint8_t IsAlarm_NH4;        //是否报警
//	uint8_t IsAlarm_F;        //是否报警
//	uint8_t IsAlarm_CL;        //是否报警
//	uint8_t IsAlarm_Chl;        //是否报警
//	uint8_t IsAlarm_Bga;        //是否报警
//	uint8_t IsAlarm_CODuv;        //是否报警


/*传感器类型*/
typedef enum{
	TYPE_DO = 0,           //溶解氧
	TYPE_pH,               //ph
	TYPE_Tur,              //浊度
	TYPE_FCL,              //浊度
	TYPE_EC,              //浊度
	TYPE_ORP,              //浊度
	TYPE_NH4,              //浊度
	TYPE_F,              //浊度
	TYPE_CL,              //浊度
	TYPE_Chl,              //浊度
	TYPE_Bga,              //浊度
	TYPE_CODuv,              //浊度
	
	TYPE_NONE              //未接传感器
}SENSOR_TYPE;





struct DO_struct;
typedef struct DO_struct{
	
	uint8_t is_init:1;          //是否初始化了
	uint8_t is_GetedValue:1;    //是否获取到数据了
	uint8_t is_FirstGetValue:1; //是否第一次获取到数据
	uint8_t is_ValueLocked:1;   //值是否被锁住
	
	uint8_t modbus_id;         //设备的Modbus ID
	uint8_t new_ModbusID;
	
	char SWV[4];
	char HWV[4];
	
	char name[6];
	
	char temperature_arr[6];   //温度显示数组
	char DOpercent_arr[7];     //DO %    显示数组
	char DOmgl_arr[6];         //DO mg/L 显示数组 
	
	uint8_t SN[13];            //设备sn码  还要加一位\0
	
	
	
	float_u temperature;       //温度值
	float_u DOpercent;         //DO %
	float_u DOmgl;             //DO mg/L
	
	float_u compensate_k;      //补偿的k值
	float_u compensate_b;      //补偿的b值
	
	float_u sal;               //盐度
	float_u press;             //气压值
	
	LiQueue* queue_domgl;      //mg/l 数据队列
	LiQueue* queue_dopercent;  //%    数据队列
	LiQueue* queue_temp;       //℃    数据队列
	
	struct DO_struct* next_DO; //下一个溶解氧设备
	
	
}DOProbe_t;
typedef DOProbe_t* PtrToDOProbe;//DO设备指针


typedef struct{//所有设备列表的结构体
	
	SENSOR_TYPE current_sensor_type; //用来设备管理里面选中设备之后用哪个cur设备指针
	
	PtrToDOProbe DO_list;

}connected_probe_t;

extern connected_probe_t connected_probe;


typedef enum{
	DO_SendType_None,
	DO_SendType_SetAddr,
	DO_SendType_GetSN,
	DO_SendType_Start,
	DO_SendType_GetTempTwoDO,
	DO_SendType_GetTemperature,
	DO_SendType_GetDOPercent,
	DO_SendType_GetDOmgL,
	DO_SendType_GetSHWVersion,
	DO_SendType_Stop,
	DO_SendType_GetKB,
	DO_SendType_SetKB,
	DO_SendType_GetModbusId,
	DO_SendType_SetSensorCap,
	DO_SendType_GetSalinity,
	DO_SendType_SetSalinity,
	DO_SendType_GetPressure,
	DO_SendType_SetPressure,
}rs485_sent_type;



//串口数据结构
typedef struct
{
	uint8_t tx_flag :1;                       //485已发送数据标志，方便判断是否回传
	uint8_t rx_flag :1;                       //485接收到数据标志
	
	uint8_t rs485_circular_sent:1;            //是否循环发送标志
	uint8_t rs485_need_sent:1;                //是否需要发送buf标志
	uint8_t rs485_is_disconnect:1;            //设备是否断开连接标志
	
	uint8_t device_count;                     //已链接设备计数
	uint8_t resend_count;                     //重发次数计数
	
	uint8_t tx_size;                          //发送buf大小 方便重发
	uint8_t rx_size;                          //接收buf大小
	
	uint8_t rx_buf[RS485_RXBUFFSIZE];         //485读取数据buf
	uint8_t tx_buf[RS485_TXBUFFSIZE];         //485输出buf方便重发

	rs485_sent_type sent_type;                //1字节  发送指令的类型
	
	void (*init)(UART_HandleTypeDef *huart);  //485初始化函数指针
	UART_HandleTypeDef *huart;                //485使用的是哪个串口
	
	
	
	
	
}rs485_t;

extern rs485_t rs485_usart;


void rs485_SendBuf(void);
void rs485_ClearRxBuf(void);

void rs485_RxCallBack(UART_HandleTypeDef *huart);//485串口的回调函数
void rs485_TxCallBack(UART_HandleTypeDef *huart);//
void rs485_IDLECallBack(UART_HandleTypeDef *huart);//485串口空闲中断回调

void rs485_SetSensorType(SENSOR_TYPE st);
SENSOR_TYPE rs485_GetSensorType(void);

void rs485_SetCircularSentStatus(void);
uint8_t rs485_GetCircularSentStatus(void);
void rs485_ClearCircularSentStatus(void);

void rs485_SetNeedSendStatus(void);
uint8_t rs485_GetNeedSendStatus(void);
void rs485_ClearNeedSendStatus(void);

void rs485_SetIsDisconnect(void);
void rs485_ClearIsDisconnect(void);
uint8_t rs485_GetIsDisconnect(void);


PtrToDOProbe* rs485_GetDoList(void);

void rs485_ClearRs485Tim(void);

void rs485_TimHandle(void);



rs485_sent_type rs485_GetSentType(void);
void rs485_SetSentType(rs485_sent_type rst);

rs485_sent_type rs485_GetSentType(void);

uint8_t rs485_GetRxFlag(void);
void rs485_SetRxFlag(void);
void rs485_ClearRxFlag(void);

void rs485_DevicePlus(void);
void rs485_DeviceReduce(void);
uint8_t rs485_GetDeviceCount(void);

void rs485_SetSearchMode(void);
void rs485_CloseSearchMode(void);
uint8_t rs485_IsSearching(void);

void rs485_ClearResendCount(void);

float float_format(float data);

void rs485_SetIsChangeSenesor(void);
void rs485_ClearIsChangeSenesor(void);
uint8_t rs485_GetIsChangeSenesor(void);

#endif



