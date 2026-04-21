#ifndef __RS485_H
#define __RS485_H

#include "main.h"
#include "usart.h"
#include "algorith.h"

#define RS485_USART USART3

#define RS485_RXBUFFSIZE 64   //485的读取缓存buf最大值
#define RS485_TXBUFFSIZE 64   //485的写入缓存buf最大值

#define RS485_DE_H()    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)
#define RS485_DE_L()    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)

#define RS485_CIRCULAR_TIM 600  //循环发送的话多久发一次

#define RESEND_MAX 4 //最大重发数，超过这个数就认为设备断开连接了

#define DO_shenghui_ModbusID 90
#define DO_HF1012_ModbusID 81
#define DO_HF_DY12_ModbusID 33

#define pH_DpH07_ModbusID 1
#define pH_shenghui_ModbusID 92

#define Tur_shenghui_ModbusID 91

#define EC_shenghui_ModbusID 93
#define EC_DE26_ModbusID 52

#define ORP_DR31_ModbusID 9

#define NH3N_DN02_ModbusID 34
#define NH3N_shenghui_ModbusID 94

#define Chl_shenghui_ModbusID 96

#define Bga_shenghui_ModbusID 97

#define COD_DC18_ModbusID 10
#define	COD_DC17_ModbusID 74
#define COD_shenghui_ModbusID 95

#define OiW_yushan_ModbusID 78
#define OiW_yushan_DA511_ModbusID 42
#define OiW_guohong_ModbusID 29

#define ZS_DZ09_ModbusID 13

#define MLSS_Tianjian_ModbusID 77

#define LH_DX01_ModbusID 5
#define MLSS_Lan_ModbusID 83

#define EC_DE40_ModbusID 3
#define EC_DS46_ModbusID 85
#define TDS_DT49_Modbus 86
/*传感器类型*/
typedef enum{
	TYPE_DO = 0,           //溶解氧
	TYPE_pH,               //ph
	TYPE_Tur,              //浊度
	TYPE_FCL,              //
	TYPE_EC,              //
	TYPE_ORP,              //
	TYPE_NH4,              //氨氮
	TYPE_F,              //
	TYPE_CL,              //
	TYPE_Chl,              //
	TYPE_Bga,              //
	TYPE_CODuv,              //
	TYPE_MLSS,				//污泥浓度
	TYPE_Oiw,				//水中油
	TYPE_TSS,				//悬浮物
	TYPE_SAL,				//盐度传感器
	TYPE_NONE              //未接传感器
	
}SENSOR_TYPE;


typedef enum
{//制造商枚举
	manufacturer_yosemitech,//禹山
	manufacturer_hyphive,   //海发
}manufacturer_enum;

typedef enum
{
	TREND_UP,
	TREND_DOWN,
}trend_enum;
typedef struct DO_struct{
	
	uint8_t is_init:1;          //是否初始化了
	uint8_t is_GetedValue:1;    //是否获取到数据了
	uint8_t is_FirstGetValue:1; //是否第一次获取到数据
	
	uint8_t is_ValueLocked:1;   //值是否被锁住
	uint8_t shake_count;   //锁定计数
	trend_enum last_trend ;//1 上涨 0 下降
	uint8_t up_count ;
	uint8_t down_count ;
	
	uint8_t modbus_id;         //设备的Modbus ID
	uint8_t new_ModbusID;      //临时的设备modbus id
	
	uint8_t update_count ;
	float DOmgl_sum ;
	float DOpercent_sum ;
	float temperature_sum ;
  	float last_DOmgl;	
	float tdsvalue;
	
	manufacturer_enum manufacturer; //生产厂商
	char SWV[5];
	char HWV[5];
	char name[8];
	
	char temperature_arr[6];   //温度显示数组
	char DOpercent_arr[7];     //DO %    显示数组
	char DOmgl_arr[7];         //DO mg/L 显示数组 
	char DOpercent_Vol_arr[8];     //DO %    显示数组
	char DOmgl_Vol_arr[8];         //DO mg/L 显示数组 	
	char tocmgl_Vol_arr[7];         //toc 显示数组 	

	uint8_t SN[17];            //设备sn码  还要加一位\0

 	uint16_u Measure_Range;
	float_u temperature;       //温度值
	float_u DOpercent;         //DO %
	float_u DOmgl;             //DO mg/L
	float_u NH4_Vol;      //NH4的电压值
	float_u pH_Vol;      //pH的电压值
	
	float_u compensate_k;      //补偿的k值
	float_u compensate_b;      //补偿的b值
	
	float_u sal;               //盐度
	float_u press;             //气压值
	
	int16_u ORP_value;		   //ORP测量数值
	int16_t ORP_sum;		   //ORP_sum值
	
	filter_t queue_domgl;      //mg/l 数据队列
	filter_t queue_dopercent;  //%    数据队列
	filter_t queue_temp;       //℃    数据队列
	filter_t queue_value;       

	uint16_u DC17_Mes_Para;	   //DC17测量模式
	uint16_u DC17_Mes_Time;	   //DC17测量时间

	char Mes_Timearr[4];	//测量时间数组	
}DOProbe_t;
typedef DOProbe_t* PtrToDOProbe;//DO设备指针


typedef struct{//所有设备列表的结构体
	
	SENSOR_TYPE current_sensor_type; //用来设备管理里面选中设备之后用哪个cur设备指针
	
	PtrToDOProbe DO_list;

}connected_probe_t;




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
	DO_SendType_SetTemp,
	DO_SendType_HyphiveClearCal, //海发溶解氧清除所有校准参数
  	DO_SendType_SetZeroCal,
	DO_SendType_SetFullCal,	
	DO_SendType_GetMesParameter, //获取测量间隔,测量模式
	DO_SendType_Set_Mes_mode,		 //设置测量模式
	DO_SendType_Set_Mes_Time,		 //设置测量间隔时间
	DO_SendType_Get_Tur_Status,		 //获取探头状态

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
extern rs485_t rs485_usart_COMB;

void rs485_SendBuf(void);
void rs485_ClearRxBuf(void);

void rs485_RxCallBack(UART_HandleTypeDef *huart);//485串口的回调函数
void rs485_TxCallBack(UART_HandleTypeDef *huart);//
void rs485_IDLECallBack(UART_HandleTypeDef *huart);//485串口空闲中断回调

void rs485_SetSensorType(SENSOR_TYPE st);
SENSOR_TYPE rs485_GetSensorType(void);

/*当前COMA连接的设备类型*/
void COMA_rs485_SetSensorType(SENSOR_TYPE st);//设置当前传感器类型
SENSOR_TYPE COMA_rs485_GetSensorType(void);//获取当前传感器类型

/*当前COMB连接的设备类型*/
void COMB_rs485_SetSensorType(SENSOR_TYPE st);//设置当前传感器类型
SENSOR_TYPE COMB_rs485_GetSensorType(void);//获取当前传感器类型


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


void rs485_SetSentType_COMB(rs485_sent_type rst);
rs485_sent_type rs485_GetSentType_COMB(void);

#endif



