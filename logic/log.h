#ifndef __LOG_H
#define __LOG_H

#include "main.h"
#include "rtc.h"
#include "gps.h"
#include "rs485.h"
#define log_count_Max  15

typedef struct{
	float temperature;                        //温度
	float pressure;                             //气压
	float salinity;                               //盐度

	float DO_mg_L;                          //溶解氧mgl
	float DO_percent;                       //溶解氧%
	
	float pH;                                    //ph值

	float pH_mv;                              //PHmv值
	
	float ORP_mV;                          //氧化还原电位
	
	float EC_us_cm;                        //电导率
	
	float EC_TDS;                            //TDS值

	float EC_salinity;                        //电导率盐度
	
	float Tur_NTU;                            //浊度

	float NH4_mg_L;                         //铵离子
	
	float F_mg_L;                             //氟离子
	
	float Cl_mg_L;                            //氯离子

	float FCL_mg_L;                         //余氯
	
	float Chl_ug_L;                           //叶绿素
	
	float Bga_cells_mL;                   //蓝绿藻
	
	float CODuv_mg_L;                    //化学需氧量

	float CODuv_toc_mg_l;				 //COD的toc值
		
	float NO3_mg_L;                      //硝酸根离子	

	float MLSS_mg_L;						//污泥浓度

	float OIW_mg_L;							//水中油

	float TSS_mg_L;							//悬浮物

	float OIW_DA511_mg_L;							//水中油DA511
	
}log_FloatData_t;

typedef struct{
	char type_str[6];                         //设备类型
	
	datetime_t time;                          //日期时间
	
	char sn[13];                              //探头sn
	
	char E_W[E_W_Length];		                  //2   E/W
	char N_S[N_S_Length];		                  //2   N/S
	char latitude[latitude_Length];		        //12  纬度
	char longitude[longitude_Length];		      //12  经度
	
	log_FloatData_t log_data;
}log_t;

typedef union{
	log_t log;
	uint8_t log_arr[sizeof(log_t)];
}log_union;

typedef struct{
	uint8_t head[3];
	log_union logu;
	uint8_t crc[2];
}log_send_t;

typedef union{
	log_send_t log_send;
	uint8_t log_send_arr[sizeof(log_send_t)];
}log_send_union;



void log_ReadLogCount(void);


uint8_t log_SaveData(SENSOR_TYPE sensor_type);

void log_SetLogCount(uint16_t count,SENSOR_TYPE type);
void log_ReadData(log_union*p ,uint16_t index,SENSOR_TYPE type);



uint16_t log_GetLogCount(SENSOR_TYPE type);

void log_PrintfLogOffset(void);

void log_SendBytes();
void log_SendCount(void);


extern uint16_t DO_Send_Count ,		pH_Send_Count ,		Tur_Send_Count,
				SAL_Send_Count ,	EC_Send_Count ,		ORP_Send_Count ,
				NH4_Send_Count ,	F_Send_Count,		CL_Send_Count,
				Chl_Send_Count ,	Bga_Send_Count ,	COD_Send_Count ,
				MLSS_Send_Count,	OiW_Send_Count,		TSS_Send_Count;

#endif




