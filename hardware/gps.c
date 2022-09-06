#include "gps.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



#if !GPS_SMALLER
uint8_t GPS_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
#endif

const uint8_t GPS_RESET[14] = {0x24 ,0x50 ,0x43 ,0x41 ,0x53 ,0x31 ,0x30 ,0x2C ,0x33 ,0x2A ,0x31 ,0x46 ,0x0D ,0x0A};//以出厂模式重启（）
const uint8_t GPS_HOT[14]   = {0x24 ,0x50 ,0x43 ,0x41 ,0x53 ,0x31 ,0x30 ,0x2C ,0x30 ,0x2A ,0x31 ,0x43 ,0x0D ,0x0A};//以热启动方式重启
const uint8_t GPS_OUTSINGLE[28] = {0x24, 0x50, 0x43, 0x41, 0x53, 0x30, 0x33, 0x2C, 0x30, 0x2C, 
																	0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x31, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2A, 0x30, 0x33, 0x0D, 0x0A };


void gps_UartInit(UART_HandleTypeDef *huart);


gps_t gps_usart = {
	.init = gps_UartInit,
};

/*清空接收buf*/
#if !GPS_SMALLER
void Clr_Buf(void)
{
	memset(GPS_RX_BUF, 0, USART_REC_LEN);      //清空          
}
#endif
void Clr_LocationBuf(void)
{
	memset(gps_usart.GPS_Buffer, 0, GPS_Buffer_Length);
}
void Clr_LatitudeBuf(void)
{
	memset(gps_usart.latitude,   0, latitude_Length);
}
void Clr_LongitudeBuf(void)
{
	memset(gps_usart.longitude,  0, longitude_Length);
}

/*清结构体*/
void Clr_GPSStruct(void)
{
	gps_usart.isGetData = false;
	gps_usart.isParseData = false;
	gps_usart.isUsefully = false;
	
	memset(gps_usart.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
	memset(gps_usart.UTCTime,    0, UTCTime_Length);
	memset(gps_usart.latitude,   0, latitude_Length);
	memset(gps_usart.N_S,        0, N_S_Length);
	memset(gps_usart.longitude,  0, longitude_Length);
	memset(gps_usart.E_W,        0, E_W_Length);
}


/*gps模块初始化*/
void gps_UartInit(UART_HandleTypeDef *huart)
{
#if !GPS_SMALLER
	Clr_Buf();
#endif
	Clr_GPSStruct();
	gps_usart.huart = huart;
	__HAL_UART_ENABLE_IT(gps_usart.huart, UART_IT_IDLE);      //开启空闲中断
	
	
#if GPS_SMALLER
	
	HAL_UART_Transmit(gps_usart.huart, (uint8_t *)GPS_OUTSINGLE, sizeof(GPS_OUTSINGLE), 200);//让gps以出厂方式启动
	HAL_UART_Receive_DMA(gps_usart.huart, (uint8_t *)gps_usart.GPS_Buffer, sizeof(gps_usart.GPS_Buffer));//开始dma接收
	HAL_UART_Transmit(gps_usart.huart, (uint8_t *)GPS_RESET, sizeof(GPS_RESET), 200);//让gps以出厂方式启动
#else
	HAL_UART_Receive_DMA(gps_usart.huart, GPS_RX_BUF, USART_REC_LEN);//开始dma接收
	HAL_UART_Transmit(gps_usart.huart, (uint8_t *)GPS_RESET, sizeof(GPS_RESET), 200);//让gps以出厂方式启动
#endif
	
}



/*接收中断回调*/
void gps_RxCallBack(UART_HandleTypeDef *huart)
{
	if(gps_usart.huart == huart)
	{
		//这里可以置标志
	}
}

#if !GPS_SMALLER
void cpy_GpsData(void)
{
	char *temp1,*temp2;
	temp1 = strstr((const char *)GPS_RX_BUF,"$GNRMC");
  temp2 = strstr((const char *)temp1+1,"$");
	memcpy(gps_usart.GPS_Buffer, temp1, temp2 - temp1);//获取最简坐标信息
}
#endif

/*gps串口空闲回调，主要是清buf和拷数据*/
void gps_IDLECallBack(UART_HandleTypeDef *huart)
{
	if(huart->Instance == GPS_USART)
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);		//清中断

		HAL_UART_AbortReceive(huart);	//已经接收完一帧数据,所以这里要停止接收,然后再重新接收		
		
		//receive_size = USART_REC_LEN - hdma_usart2_rx.Instance->CNDTR;  //接收到多少数据
		
		
#if GPS_SMALLER
		HAL_UART_Receive_DMA(huart, (uint8_t *)gps_usart.GPS_Buffer, sizeof(gps_usart.GPS_Buffer));//开始dma接收
#else
		Clr_LocationBuf();
		cpy_GpsData();//直接拷贝怕等下在拷贝中途有中断发生
		HAL_UART_Receive_DMA(huart, GPS_RX_BUF, USART_REC_LEN); //开启DMA接收
#endif
	}
}



/*解析gps数据从数据中解析出经纬度*/
void get_GpsData(void)
{
	char *subString;
	char *subStringNext;
	char temp_buf[latitude_Length]={0};
	uint8_t i = 0;
	
	for (i = 0 ; i <= 6 ; i++)
	{
		if (i == 0)
		{
			if ((subString = strstr(gps_usart.GPS_Buffer, ",")) == NULL)
			{
				return;
			}
		}
		else
		{
			subString++;
			if ((subStringNext = strstr(subString, ",")) != NULL)
			{
				char usefullyBuffer[2]; 
				switch(i)
				{
					case 1://获取UTC时间
						memcpy(gps_usart.UTCTime, subString, subStringNext - subString);
						break;  	
					case 2://获取数据类型 A定位 V导航
						memcpy(usefullyBuffer, subString, subStringNext - subString);
						gps_usart.isUsefully = ((usefullyBuffer[0] == 'A') ? true : false);
						break;	      
					
					case 3://获取纬度信息
						if(gps_usart.isUsefully)
						{
							double latitude = 0.0;
							int temp = 0;
							
							memcpy(temp_buf, subString, subStringNext - subString);
							
							latitude = strtod(temp_buf, NULL);
							latitude /= 100.0;
							
							temp = (int) latitude;
							
							Clr_LatitudeBuf();
							snprintf(gps_usart.latitude, sizeof(gps_usart.latitude), "%.7f", temp + (latitude - temp)*100/60);
						}
						break;	
					case 4://获取N/S
						if(gps_usart.isUsefully)
						{
							memcpy(gps_usart.N_S, subString, subStringNext - subString);
						}
						break;	      
					case 5://获取经度信息
						if(gps_usart.isUsefully)
						{
							double longitude = 0.0;
							int temp = 0;
							
							memcpy(temp_buf, subString, subStringNext - subString);
							
							longitude = strtod(temp_buf, NULL);
							longitude /= 100.0;
							
							temp = (int) longitude;
							
							Clr_LongitudeBuf();
							snprintf(gps_usart.longitude, sizeof(gps_usart.longitude), "%.7f", temp + (longitude - temp)*100/60);
						}
						break;	
					case 6://获取E/W
						if(gps_usart.isUsefully)
						{
							memcpy(gps_usart.E_W, subString, subStringNext - subString);
						}
						break;

					default:break;
				}
				
				subString = subStringNext;
				gps_usart.isParseData = true;
			}
			else
			{
				return;
			}
		}
	}
}

uint8_t get_GpsDataUsefully(void)
{
	return gps_usart.isUsefully;
}

char* get_LatitudeArr(void)
{
	return gps_usart.latitude;
}

char* get_LongitudeArr(void)
{
	return gps_usart.longitude;
}

char* get_ns(void)
{
	return gps_usart.N_S;
}

char* get_ew(void)
{
	return gps_usart.E_W;
}

