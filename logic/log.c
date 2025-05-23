#include "log.h"

#include "dissolved_oxygen.h"
#include "spi_flash.h"
#include "bmp280.h"
#include "setting.h"
#include "ch340e.h"

#include <stdio.h>
#include <stdlib.h>



typedef union
{
	uint16_t log_count_Type[log_count_Max];
	uint8_t arr[sizeof(uint16_t)*log_count_Max];
}log_count_union;

STATIC log_count_union log_count; 

SENSOR_TYPE SaveData_SensorType;

STATIC log_union log_u;
const uint16_t LogCount_max = LOG_MAX_COUNT;



uint16_t log_Get_Log_Total_Count()
{				

 return   log_count.log_count_Type[TYPE_DO]   + log_count.log_count_Type[TYPE_pH]  + log_count.log_count_Type[TYPE_Tur] 
 		+ log_count.log_count_Type[TYPE_FCL]  + log_count.log_count_Type[TYPE_EC]  + log_count.log_count_Type[TYPE_ORP] 
	    + log_count.log_count_Type[TYPE_NH4]  + log_count.log_count_Type[TYPE_F]   + log_count.log_count_Type[TYPE_CL]  
		+ log_count.log_count_Type[TYPE_Chl]  + log_count.log_count_Type[TYPE_Bga] + log_count.log_count_Type[TYPE_CODuv]
		+ log_count.log_count_Type[TYPE_MLSS] + log_count.log_count_Type[TYPE_Oiw];

}


uint16_t log_GetLogCount(SENSOR_TYPE type)
{				
 return log_count.log_count_Type[type];
}

void log_ReadLogCount(void)
{
 W25QXX_Read(log_count.arr, LOG_COUNT_ADDR, 2*log_count_Max);	
}


void log_SetLogCount(uint16_t count,SENSOR_TYPE type)
{

	log_count.log_count_Type[type]=count ;                   
	W25QXX_Write(log_count.arr, LOG_COUNT_ADDR, 2*log_count_Max);
}

void log_WriteLog(log_union *log, uint16_t index,SENSOR_TYPE type)
{
	uint32_t addr=0;
	switch(type){
			case TYPE_DO:					
       addr = LOG_FIRST_ADDR + (index << 8);
				break;				
			case TYPE_pH:
       addr = LOG_FIRST_pH_ADDR + (index << 8);
				break;
			
			case TYPE_Tur:
       addr = LOG_FIRST_Tur_ADDR + (index << 8);
				break;
			
			case TYPE_FCL:
       addr = LOG_FIRST_FCL_ADDR + (index << 8);
				break;
			
			case TYPE_EC:
       addr = LOG_FIRST_EC_ADDR + (index << 8);
				break;
			
			case TYPE_ORP:
       addr = LOG_FIRST_ORP_ADDR + (index << 8);
				break;
			
			case TYPE_NH4:
       addr = LOG_FIRST_NH4_ADDR + (index << 8);			
			 break;
			
			case TYPE_F:
       addr = LOG_FIRST_F_ADDR + (index << 8);		
				break;
			
			case TYPE_CL:
       addr = LOG_FIRST_CL_ADDR + (index << 8);		
				break;
			
			case TYPE_Chl:
       addr = LOG_FIRST_Chl_ADDR + (index << 8);		
				break;
			
			case TYPE_Bga:
       addr = LOG_FIRST_Bga_ADDR + (index << 8);		
				break;
			
			case TYPE_CODuv:
       addr = LOG_FIRST_COD_ADDR + (index << 8);		
				break;

			case TYPE_MLSS:
		addr = LOG_FIRST_MLSS_ADDR + (index << 8);		
				break;
			
			case TYPE_Oiw:
		addr = LOG_FIRST_OIW_ADDR + (index << 8);		
				break;
			
			default:
				break;	     
	}
	W25QXX_Write(log->log_arr, addr, sizeof(log_t));
}


void log_ReadData(log_union*p ,uint16_t index,SENSOR_TYPE type)
{
	uint32_t addr =0;	
	switch(type){
			case TYPE_DO:					
       addr = LOG_FIRST_ADDR + (index << 8);
				break;				
			case TYPE_pH:
       addr = LOG_FIRST_pH_ADDR + (index << 8);
				break;
			
			case TYPE_Tur:
       addr = LOG_FIRST_Tur_ADDR + (index << 8);
				break;
			
			case TYPE_FCL:
       addr = LOG_FIRST_FCL_ADDR + (index << 8);
				break;
			
			case TYPE_EC:
       addr = LOG_FIRST_EC_ADDR + (index << 8);
				break;
			
			case TYPE_ORP:
       addr = LOG_FIRST_ORP_ADDR + (index << 8);
				break;
			
			case TYPE_NH4:
       addr = LOG_FIRST_NH4_ADDR + (index << 8);			
			 break;
			
			case TYPE_F:
       addr = LOG_FIRST_F_ADDR + (index << 8);		
				break;
			
			case TYPE_CL:
       addr = LOG_FIRST_CL_ADDR + (index << 8);		
				break;
			
			case TYPE_Chl:
       addr = LOG_FIRST_Chl_ADDR + (index << 8);		
				break;
			
			case TYPE_Bga:
       addr = LOG_FIRST_Bga_ADDR + (index << 8);		
				break;
			
			case TYPE_CODuv:
       addr = LOG_FIRST_COD_ADDR + (index << 8);		
				break;
			
			case TYPE_MLSS:
		addr = LOG_FIRST_MLSS_ADDR + (index << 8);		
				break;

			case TYPE_Oiw:
		addr = LOG_FIRST_OIW_ADDR + (index << 8);		
				break;
			default:
				break;	     
	}
	W25QXX_Read(p->log_arr, addr, sizeof(log_t));
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
	dat->log_data.DO_mg_L = 0.0;
	dat->log_data.DO_percent = 0.0;
	
	dat->log_data.pH = 0.0;
	dat->log_data.pH_mv= 0.0;	
	dat->log_data.ORP_mV = 0.0;

	dat->log_data.EC_us_cm = 0.0;
	dat->log_data.EC_TDS = 0.0;	
	dat->log_data.EC_salinity = 0.0;

	dat->log_data.Tur_NTU = 0.0;
	
	dat->log_data.NH4_mg_L = 0.0;
	
	dat->log_data.F_mg_L = 0.0;
	
	dat->log_data.Cl_mg_L = 0.0;

	dat->log_data.FCL_mg_L = 0.0;
	
	dat->log_data.Chl_ug_L = 0.0;
	
	dat->log_data.Bga_cells_mL = 0.0;
	
	dat->log_data.CODuv_mg_L = 0.0;
	
	dat->log_data.CODuv_toc_mg_l = 0.0;
	dat->log_data.NO3_mg_L= 0.0;	
	dat->log_data.MLSS_mg_L= 0.0;
	dat->log_data.OIW_mg_L= 0.0;		
}





uint8_t log_SaveData(SENSOR_TYPE sensor_type)
{
  uint16_t index=0;
	log_init(&(log_u.log));
	
	switch(sensor_type)
	{
		case TYPE_DO:
			if(log_count.log_count_Type[0] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'D';
			log_u.log.type_str[1] = 'O';
			log_u.log.type_str[2] = 0x00;			
      index=	log_count.log_count_Type[0];
			
			
			log_u.log.log_data.salinity = setting_GetSalinity();
			log_u.log.log_data.pressure = bmp280_GetPress() + setting_GetAirCompensate();	
			

			log_u.log.log_data.DO_mg_L = atof(get_CurDo()->DOmgl_arr);          //DO_GetDOmgL(get_CurDo());
			log_u.log.log_data.DO_percent = atof(get_CurDo()->DOpercent_arr);   //DO_GetDOPercent(get_CurDo())*100;
			
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}
			break;
			
		case TYPE_pH:
			if(log_count.log_count_Type[1] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] ='p';
			log_u.log.type_str[1] = 'H';
      index=	log_count.log_count_Type[1];
			
			log_u.log.log_data.pH = atof(get_CurDo()->DOmgl_arr);        
			
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}
			break;
			
		case TYPE_Tur:
			if(log_count.log_count_Type[2] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'T';
			log_u.log.type_str[1] = 'u';
			log_u.log.type_str[2] = 'r';		
      index=	log_count.log_count_Type[2];			
			
			log_u.log.log_data.Tur_NTU = atof(get_CurDo()->DOmgl_arr); 
			
			break;
			
		case TYPE_FCL:
			if(log_count.log_count_Type[3] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'F';
			log_u.log.type_str[1] = 'C';
			log_u.log.type_str[2] = 'L';				
      index=	log_count.log_count_Type[3];
			
			log_u.log.log_data.FCL_mg_L = atof(get_CurDo()->DOmgl_arr);        
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			
		break;
			
		case TYPE_EC:
			if(log_count.log_count_Type[4] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'E';
			log_u.log.type_str[1] = 'C';
			log_u.log.type_str[2] = 0x00;		
      index=	log_count.log_count_Type[4];			
			
			log_u.log.log_data.EC_us_cm = get_CurDo()->DOmgl.value_f;    
			// log_u.log.log_data.EC_TDS = atof(get_CurDo()->DOpercent_arr);  
			log_u.log.log_data.EC_TDS = get_CurDo()->DOpercent.value_f;
			// log_u.log.log_data.EC_salinity = get_CurDo()->pH_Vol.value_f ; 
			 
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			
			break;
			
		case TYPE_ORP:
			if(log_count.log_count_Type[5] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'O';
			log_u.log.type_str[1] = 'R';
			log_u.log.type_str[2] = 'P';		
      index=	log_count.log_count_Type[5];			
			
			log_u.log.log_data.ORP_mV = atof(get_CurDo()->DOmgl_arr);       
			// if(setting_Get_Temp_Unit())
			// {
			//     log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			// }
			// else
			// {
		    // 	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			// }			
			break;
			
		case TYPE_NH4:
			if(log_count.log_count_Type[6] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'N';
			log_u.log.type_str[1] = 'H';
			log_u.log.type_str[2] = '3';
			log_u.log.type_str[3] = 'N';	
      index=	log_count.log_count_Type[6];
			
			log_u.log.log_data.NH4_mg_L = atof(get_CurDo()->DOmgl_arr);        
			log_u.log.log_data.pH = atof(get_CurDo()->DOpercent_arr);   
			
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			break;
		case TYPE_F:
			if(log_count.log_count_Type[7] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'F';
			log_u.log.type_str[1] = 0x00;
			log_u.log.type_str[2] = 0x00;			
      index=	log_count.log_count_Type[7];			
			
			log_u.log.log_data.F_mg_L = atof(get_CurDo()->DOmgl_arr);      
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			

			break;
		case TYPE_CL:
			if(log_count.log_count_Type[8] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'C';
			log_u.log.type_str[1] = 'L';
			log_u.log.type_str[2] = 0x00;	
      index=	log_count.log_count_Type[8];
			
			log_u.log.log_data.Cl_mg_L = atof(get_CurDo()->DOmgl_arr);        

			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			break;
			
			
		case TYPE_Chl:
			if(log_count.log_count_Type[9] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'C';
			log_u.log.type_str[1] = 'h';
			log_u.log.type_str[2] = 'l';	
      index=	log_count.log_count_Type[9];
			
			log_u.log.log_data.Chl_ug_L = atof(get_CurDo()->DOmgl_arr);      
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			break;
		case TYPE_Bga:
			if(log_count.log_count_Type[10] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'B';
			log_u.log.type_str[1] = 'g';
			log_u.log.type_str[2] = 'a';	
      index=	log_count.log_count_Type[10];
		
			// log_u.log.log_data.Bga_cells_mL = atof(get_CurDo()->DOmgl_arr);    
			log_u.log.log_data.Bga_cells_mL = get_CurDo()->DOmgl.value_f;  
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			
			break;
		case TYPE_CODuv:
			if(log_count.log_count_Type[11] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'C';
			log_u.log.type_str[1] = 'O';
			log_u.log.type_str[2] = 'D';
      index=	log_count.log_count_Type[11];
			log_u.log.log_data.CODuv_mg_L = atof(get_CurDo()->DOmgl_arr);      
			log_u.log.log_data.Tur_NTU = atof(get_CurDo()->DOpercent_arr);   
			log_u.log.log_data.CODuv_toc_mg_l = atof(get_CurDo()->tocmgl_Vol_arr); 
			
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}			
			break;
		case TYPE_MLSS:
			if(log_count.log_count_Type[12] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'M';
			log_u.log.type_str[1] = 'L';
			log_u.log.type_str[2] = 'S';
			log_u.log.type_str[3] = 'S';
      index=	log_count.log_count_Type[12];

			log_u.log.log_data.MLSS_mg_L = get_CurDo()->DOmgl.value_f;
			break;

		case TYPE_Oiw:
			if(log_count.log_count_Type[13] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'O';
			log_u.log.type_str[1] = 'i';
			log_u.log.type_str[2] = 'W';
      index=	log_count.log_count_Type[13];

			log_u.log.log_data.OIW_mg_L = atof(get_CurDo()->DOmgl_arr);

			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}	
			break;
					
		default:
			break;
	}
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
	

	
	log_WriteLog(&log_u, index,rs485_GetSensorType());
	log_SetLogCount(index+1,sensor_type);
	
	return 1;
}

void send_char(char ch)
{
	while(!(USART1->SR & (1<<6)));
	USART1->DR = ch;
}

uint16_t DO_Send_Count = 0,		pH_Send_Count = 0,		Tur_Send_Count = 0,
		FCL_Send_Count = 0,		EC_Send_Count = 0,		ORP_Send_Count = 0,
		NH4_Send_Count = 0,		F_Send_Count = 0,		CL_Send_Count = 0,
		Chl_Send_Count = 0,		Bga_Send_Count = 0,		COD_Send_Count = 0,
		MLSS_Send_Count = 0,	OiW_Send_Count = 0;

void log_SendBytes()
{
	log_send_union log_send_u;
	log_send_u.log_send.head[0] = 0xFF;
	log_send_u.log_send.head[1] = 0x03;
	log_send_u.log_send.head[2] = 0x02;

	 if( COD_Send_Count < log_count.log_count_Type[TYPE_CODuv] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, COD_Send_Count,TYPE_CODuv);
		 COD_Send_Count++;
	 }		 
	 else if( Bga_Send_Count < log_count.log_count_Type[TYPE_Bga] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, Bga_Send_Count,TYPE_Bga);
		 Bga_Send_Count++;
	 }		 
	 else if( Chl_Send_Count < log_count.log_count_Type[TYPE_Chl] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, Chl_Send_Count,TYPE_Chl);
		 Chl_Send_Count++;
	 }	
	 else if( FCL_Send_Count < log_count.log_count_Type[TYPE_FCL] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, FCL_Send_Count,TYPE_FCL);
		 FCL_Send_Count++;
	 }	
	 else if( CL_Send_Count < log_count.log_count_Type[TYPE_CL] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, CL_Send_Count,TYPE_CL);
		 CL_Send_Count++;
	 }		 
	 else if( F_Send_Count < log_count.log_count_Type[TYPE_F] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, F_Send_Count,TYPE_F);
		 F_Send_Count++;
	 }		 
	 else if( NH4_Send_Count < log_count.log_count_Type[TYPE_NH4] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, NH4_Send_Count,TYPE_NH4);
		 NH4_Send_Count++;
	 }	 
	 else if( ORP_Send_Count < log_count.log_count_Type[TYPE_ORP] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, ORP_Send_Count,TYPE_ORP);
		 ORP_Send_Count++;
	 }	
	 else if( EC_Send_Count < log_count.log_count_Type[TYPE_EC] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, EC_Send_Count,TYPE_EC);
		 EC_Send_Count++;
	 }	
	 else if( Tur_Send_Count < log_count.log_count_Type[TYPE_Tur] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, Tur_Send_Count,TYPE_Tur);
		 Tur_Send_Count++;
	 }	 
	 else if( pH_Send_Count < log_count.log_count_Type[TYPE_pH] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, pH_Send_Count,TYPE_pH);
		 pH_Send_Count++;
	 }		 
	 else if( DO_Send_Count < log_count.log_count_Type[TYPE_DO] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, DO_Send_Count,TYPE_DO);
		 DO_Send_Count++;
	 }	
	 else if( MLSS_Send_Count < log_count.log_count_Type[TYPE_MLSS] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, MLSS_Send_Count,TYPE_MLSS);
		 MLSS_Send_Count++;
	 }
	 else if( OiW_Send_Count < log_count.log_count_Type[TYPE_Oiw] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, OiW_Send_Count,TYPE_Oiw);
		 OiW_Send_Count++;
	 }
	 
	SetCrc(log_send_u.log_send_arr, sizeof(log_send_u.log_send_arr));
	
	HAL_UART_Transmit(ch340e_usart.huart, log_send_u.log_send_arr, sizeof(log_send_u.log_send_arr),200);
}

void log_SendCount(void)
{
	uint8_t send_temp[7];
	send_temp[0] = 0xFF;
	send_temp[1] = 0x03;
	send_temp[2] = 0x01;
	send_temp[3] = log_Get_Log_Total_Count() >> 8;
	send_temp[4] = log_Get_Log_Total_Count() & 0xFF;
	SetCrc(send_temp, sizeof(send_temp));
	
	HAL_UART_Transmit(ch340e_usart.huart, send_temp, sizeof(send_temp),200);
}