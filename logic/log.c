#include "log.h"

#include "dissolved_oxygen.h"
#include "spi_flash.h"
#include "bmp280.h"
#include "setting.h"
#include "ch340e.h"

#include <stdio.h>
#include <stdlib.h>

STATIC uint16_t log_count = 0; 

SENSOR_TYPE SaveData_SensorType;

STATIC log_union log_u;
const uint16_t LogCount_max = 50000;

void log_SetSaveData_SensorType(SENSOR_TYPE type)
{
	SaveData_SensorType = type;
}

uint16_t log_GetLogCount(void)
{
	return log_count;
}

void log_ReadLogCount(void)
{
	uint8_t dat[2] = {0};
	W25QXX_Read(dat, LOG_COUNT_ADDR, 2);
	
	log_count = dat[0]<<8 | dat[1];
}
void log_SetLogCount(uint16_t count)
{
	uint8_t dat[2] = {0};
	dat[0] = count >> 8;
	dat[1] = count & 0xFF;
	log_count = count;
	W25QXX_Write(dat, LOG_COUNT_ADDR, 2);
}

void log_WriteLog(log_union *log, uint16_t index)
{
	uint32_t addr = LOG_FIRST_ADDR + (index << 8);
	W25QXX_Write(log->log_arr, addr, sizeof(log_t));
}

void log_init(log_t *dat)
{
	memset(dat->type_str, 0, sizeof(dat->type_str));
	memset(dat->E_W, 0, sizeof(dat->E_W));
	memset(dat->N_S, 0, sizeof(dat->N_S));
	memset(dat->latitude, 0, sizeof(dat->latitude));
	memset(dat->longitude, 0, sizeof(dat->longitude));
	memset(dat->sn, 0, sizeof(dat->sn));

	dat->log_data.temperature = 0.0;
	dat->log_data.pressure = 0.0;
	dat->log_data.salinity = 0.0;
	dat->log_data.pH = 0.0;
	dat->log_data.DO_mg_L = 0.0;
	dat->log_data.DO_percent = 0.0;
	dat->log_data.FCL_mg_L = 0.0;
	dat->log_data.EC_us_cm = 0.0;
	dat->log_data.Tur_NTU = 0.0;
	dat->log_data.ORP_mV = 0.0;
	dat->log_data.NH4_mg_L = 0.0;
	dat->log_data.F_mg_L = 0.0;
	dat->log_data.Cl_mg_L = 0.0;
	dat->log_data.Chl_ug_L = 0.0;
	dat->log_data.Bga_cells_mL = 0.0;
	dat->log_data.CODuv_mg_L = 0.0;
}

/*
	char type_str[5];                         //0
	
	datetime_t time;                          //5
	
	char sn[13];                              //    
	
	char E_W[E_W_Length];		                  //2   E/W
	char N_S[N_S_Length];		                  //2   N/S
	char latitude[latitude_Length];		        //12  纬度
	char longitude[longitude_Length];		      //12  经度
*/



uint8_t log_SaveData(SENSOR_TYPE sensor_type)
{
	if(log_count >= LogCount_max)//如果写满了直接跳过
	{
		return 0;
	}
	log_init(&(log_u.log));
	
	switch(sensor_type)
	{
		case TYPE_DO:
			log_u.log.type_str[0] = 0x44;
			log_u.log.type_str[1] = 0x4F;
			log_u.log.type_str[2] = 0x00;
		
			log_u.log.time.day = RTC_GetDay();
			log_u.log.time.hour = RTC_GetHour();
			log_u.log.time.minute = RTC_GetMinute();
			log_u.log.time.month = RTC_GetMonth();
			log_u.log.time.seconds = RTC_GetSecond();
			log_u.log.time.week = RTC_GetWeek();
			log_u.log.time.years = RTC_GetYear();
			
			memcpy(log_u.log.sn, get_CurDo()->SN, sizeof(log_u.log.sn));
		
			memcpy(log_u.log.latitude, get_LatitudeArr(), sizeof(log_u.log.latitude));
			memcpy(log_u.log.longitude, get_LongitudeArr(), sizeof(log_u.log.longitude));
		
			memcpy(log_u.log.E_W, get_ew(), sizeof(log_u.log.E_W));
			memcpy(log_u.log.N_S, get_ns(), sizeof(log_u.log.N_S));
			
			log_u.log.log_data.salinity = setting_GetSalinity();
			log_u.log.log_data.pressure = bmp280_GetPress() + setting_GetAirCompensate();
			
			log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());
			log_u.log.log_data.DO_mg_L = atof(get_CurDo()->DOmgl_arr);          //DO_GetDOmgL(get_CurDo());
			log_u.log.log_data.DO_percent = atof(get_CurDo()->DOpercent_arr);   //DO_GetDOPercent(get_CurDo())*100;
			
			
			break;
		
		default:
			break;
	}
	log_WriteLog(&log_u, log_count);
	log_SetLogCount(log_count+1);
	
	return 1;
}

void log_ReadData(log_union*p ,uint16_t index)
{
	uint32_t addr = LOG_FIRST_ADDR + (index << 8);
	W25QXX_Read(p->log_arr, addr, sizeof(log_t));
}
	
//void log_PrintfLogOffset(void)
//{
//	printf("type_str\t:%ld\n",&(((log_send_t *)0)->logu.log.type_str));
//	
//	printf("years\t\t:%ld\n",&(((log_send_t *)0)->logu.log.time.years));
//	printf("month\t\t:%ld\n",&(((log_send_t *)0)->logu.log.time.month));
//	printf("day\t\t:%ld\n",&(((log_send_t *)0)->logu.log.time.day));
//	printf("hour\t\t:%ld\n",&(((log_send_t *)0)->logu.log.time.hour));
//	printf("minute\t\t:%ld\n",&(((log_send_t *)0)->logu.log.time.minute));
//	printf("seconds\t\t:%ld\n",&(((log_send_t *)0)->logu.log.time.seconds));
//	
//	printf("sn\t\t:%ld\n",&(((log_send_t *)0)->logu.log.sn));
//	printf("E_W\t\t:%ld\n",&(((log_send_t *)0)->logu.log.E_W));
//	printf("N_S\t\t:%ld\n",&(((log_send_t *)0)->logu.log.N_S));
//	printf("latitude\t:%ld\n",&(((log_send_t *)0)->logu.log.latitude));
//	printf("longitude\t:%ld\n",&(((log_send_t *)0)->logu.log.longitude));

//	printf("temperature\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.temperature));
//	printf("pressure\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.pressure));
//	printf("salinity\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.salinity));
//	printf("pH\t\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.pH));
//	printf("DO_mg_L\t\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.DO_mg_L));
//	printf("DO_percent\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.DO_percent));
//	printf("FCL_mg_L\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.FCL_mg_L));
//	printf("EC_us_cm\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.EC_us_cm));
//	printf("Tur_NTU\t\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.Tur_NTU));
//	printf("ORP_mV\t\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.ORP_mV));
//	printf("NH4_mg_L\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.NH4_mg_L));
//	printf("F_mg_L\t\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.F_mg_L));
//	printf("Cl_mg_L\t\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.Cl_mg_L));
//	printf("Chl_ug_L\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.Chl_ug_L));
//	printf("Bga_cells_mL\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.Bga_cells_mL));
//	printf("CODuv_mg_L\t:%ld\n",&(((log_send_t *)0)->logu.log.log_data.CODuv_mg_L));
//}

void send_char(char ch)
{
	while(!(USART1->SR & (1<<6)));
	USART1->DR = ch;
}

void log_SendBytes(uint16_t index)
{
	log_send_union log_send_u;
	log_send_u.log_send.head[0] = 0xFF;
	log_send_u.log_send.head[1] = 0x03;
	log_send_u.log_send.head[2] = 0x02;
	
	log_ReadData(&log_send_u.log_send.logu, index);
	
	SetCrc(log_send_u.log_send_arr, sizeof(log_send_u.log_send_arr));
	
	HAL_UART_Transmit(ch340e_usart.huart, log_send_u.log_send_arr, sizeof(log_send_u.log_send_arr),200);
}

void log_SendCount(void)
{
	uint8_t send_temp[7];
	send_temp[0] = 0xFF;
	send_temp[1] = 0x03;
	send_temp[2] = 0x01;
	send_temp[3] = log_GetLogCount() >> 8;
	send_temp[4] = log_GetLogCount() & 0xFF;
	SetCrc(send_temp, sizeof(send_temp));
	
	HAL_UART_Transmit(ch340e_usart.huart, send_temp, sizeof(send_temp),200);
}






