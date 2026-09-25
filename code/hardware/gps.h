#ifndef __GPS_H
#define __GPS_H


#include "main.h"
#include "usart.h"

#define GPS_USART USART2

//定义数组长度
#define GPS_Buffer_Length  100
#define UTCTime_Length     11
#define latitude_Length    12
#define N_S_Length         2
#define longitude_Length   12
#define E_W_Length         2 

#define GPS_SMALLER 0

#define USART_REC_LEN  		 900  	//定义最大接收字节数 200

#define false  0
#define true   1



typedef struct 
{ 
	char isGetData   :1;		                  //    是否获取到GPS数据
	char isParseData :1;	                    //    是否解析完成
	char isUsefully  :1;	 	                  //1   定位信息是否有效
	
	char E_W[E_W_Length];		                  //2   E/W
	char N_S[N_S_Length];		                  //2   N/S
	
	char UTCTime[UTCTime_Length];		          //11  UTC时间
	char latitude[latitude_Length];		        //11  纬度
	char longitude[longitude_Length];		      //12  经度
	char GPS_Buffer[GPS_Buffer_Length];       //80  gps buf长度
	
	void (*init)(UART_HandleTypeDef *huart);  //4   初始化函数指针
	UART_HandleTypeDef *huart;                //4   使用的是哪个串口
	
	
	
} gps_t;

extern gps_t gps_usart;

#if !GPS_SMALLER
extern uint8_t GPS_RX_BUF[USART_REC_LEN];
#endif

void gps_IDLECallBack(UART_HandleTypeDef *huart);
void gps_RxCallBack(UART_HandleTypeDef *huart);



void get_GpsData(void); //从dma数据中解析出坐标信息

char* get_LatitudeArr(void);
char* get_LongitudeArr(void);

char* get_ns(void);
char* get_ew(void);

void cpy_GpsData(void);



#endif



