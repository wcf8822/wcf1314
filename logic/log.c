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

 return   log_count.log_count_Type[0]  + log_count.log_count_Type[1]  + log_count.log_count_Type[2] 
 		    + log_count.log_count_Type[3]  + log_count.log_count_Type[4]  + log_count.log_count_Type[5] 
	      + log_count.log_count_Type[6]  + log_count.log_count_Type[7]  + log_count.log_count_Type[8]  
		    + log_count.log_count_Type[9]  + log_count.log_count_Type[10] + log_count.log_count_Type[11]
		    + log_count.log_count_Type[12] + log_count.log_count_Type[13] + log_count.log_count_Type[14]
		    + log_count.log_count_Type[15] + log_count.log_count_Type[16] + log_count.log_count_Type[17];
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

void log_WriteLog(log_union *log, uint16_t index,uint8_t log_type)
{
	uint32_t addr=0;
	switch(log_type){
		
			case 0:					
       addr = LOG_FIRST_ADDR + (index << 8);
				break;	
			
			case 1:
       addr = LOG_FIRST_pH_ADDR + (index << 8);
				break;
			
			case 2:
       addr = LOG_FIRST_Tur_ADDR + (index << 8);
				break;

			case 3:
        addr = LOG_FIRST_FCL_ADDR + (index << 8);
				break;
			
			case 4:
        addr = LOG_FIRST_EC_ADDR + (index << 8);
				break;
			
			case 5:
       addr = LOG_FIRST_ORP_ADDR + (index << 8);
				break;
			
			case 6:
       addr = LOG_FIRST_NH4_ADDR + (index << 8);
				break;
			
			case 7:
       addr = LOG_FIRST_F_ADDR + (index << 8);			
			 break;
			
			case 8:
        addr = LOG_FIRST_CL_ADDR + (index << 8);		
				break;
			
			case 9:
       addr = LOG_FIRST_Chl_ADDR + (index << 8);		
				break;
			
			case 10:
       addr = LOG_FIRST_Bga_ADDR + (index << 8);		
				break;
			
			case 11:
       addr = LOG_FIRST_COD_ADDR + (index << 8);		
				break;
			
			case 12:
       addr = LOG_FIRST_MLSS_ADDR + (index << 8);
				break;

			case 13:
		   addr = LOG_FIRST_OIW_ADDR + (index << 8);		
				break;
			
			case 14:
		   addr = LOG_FIRST_OIW_YUSHAN_ADDR + (index << 8);		
				break;

			case 15:
		   addr = LOG_FIRST_TSS_ADDR + (index << 8);	
				break;

			case 16:
		   addr = LOG_FIRST_SAL_ADDR + (index << 8);		
				break;

			case 17:
		   addr = LOG_FIRST_TDS_ADDR + (index << 8);	
				break;
			
			default:
				break;	     
	}
	W25QXX_Write(log->log_arr, addr, sizeof(log_t));
}


void log_ReadData(log_union*p ,uint16_t index,SENSOR_TYPE log_type)
{
	uint32_t addr =0;	
	switch(log_type){
			case 0:					
       addr = LOG_FIRST_ADDR + (index << 8);
				break;				
			case 1:
       addr = LOG_FIRST_pH_ADDR + (index << 8);
				break;
			
			case 2:
       addr = LOG_FIRST_Tur_ADDR + (index << 8);
				break;
			
			case 3:
       addr = LOG_FIRST_FCL_ADDR + (index << 8);
				break;
			
			case 4:
       addr = LOG_FIRST_EC_ADDR + (index << 8);
				break;
			
			case 5:
       addr = LOG_FIRST_ORP_ADDR + (index << 8);
				break;
			
			case 6:
       addr = LOG_FIRST_NH4_ADDR + (index << 8);			
			 break;
			
			case 7:
       addr = LOG_FIRST_F_ADDR + (index << 8);		
				break;
			
			case 8:
       addr = LOG_FIRST_CL_ADDR + (index << 8);		
				break;
			
			case 9:
       addr = LOG_FIRST_Chl_ADDR + (index << 8);		
				break;
			
			case 10:
       addr = LOG_FIRST_Bga_ADDR + (index << 8);		
				break;
			
			case 11:
       addr = LOG_FIRST_COD_ADDR + (index << 8);		
				break;
			
			case 12:
		   addr = LOG_FIRST_MLSS_ADDR + (index << 8);		
				break;

			case 13:
		   addr = LOG_FIRST_OIW_ADDR + (index << 8);		
				break;

			case 14:
		   addr = LOG_FIRST_OIW_YUSHAN_ADDR + (index << 8);		
				break;
	
			case 15:
		   addr = LOG_FIRST_TSS_ADDR + (index << 8);		
				break;

			case 16:
		   addr = LOG_FIRST_SAL_ADDR + (index << 8);		
				break;

			case 17:
		   addr = LOG_FIRST_TDS_ADDR + (index << 8);		
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

	dat->log_data.TSS_mg_L = 0.0;	
	dat->log_data.OIW_DA511_mg_L=0;
}





uint8_t log_SaveData(SENSOR_TYPE sensor_type)
{
	
	uint8_t log_type=0;
  uint16_t index=0;
	log_init(&(log_u.log));
	
	switch(sensor_type)
	{
		case TYPE_DO:
			log_type=0;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'D';
			log_u.log.type_str[1] = 'O';
			log_u.log.type_str[2] = 0x00;			
      index=	log_count.log_count_Type[log_type];
			
			
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
			log_type=1;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] ='p';
			log_u.log.type_str[1] = 'H';
      index=	log_count.log_count_Type[log_type];
			
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
			log_type=2;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'T';
			log_u.log.type_str[1] = 'u';
			log_u.log.type_str[2] = 'r';		
      index=	log_count.log_count_Type[log_type];			
			
			log_u.log.log_data.Tur_NTU = atof(get_CurDo()->DOmgl_arr); 
			
			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}
			break;
			
		case TYPE_FCL:
			log_type=3;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'F';
			log_u.log.type_str[1] = 'C';
			log_u.log.type_str[2] = 'L';				
      index=	log_count.log_count_Type[log_type];
			
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
			log_type=4;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'E';
			log_u.log.type_str[1] = 'C';
			log_u.log.type_str[2] = 0x00;		
      index=	log_count.log_count_Type[log_type];			
			
			if(get_CurDo()->modbus_id == EC_DE40_ModbusID)
			{
				log_u.log.log_data.EC_us_cm = get_CurDo()->DOmgl.value_f * 1000.0f;    
			}
			else
			{
				log_u.log.log_data.EC_us_cm = get_CurDo()->DOmgl.value_f;    
			}
			// log_u.log.log_data.EC_TDS = atof(get_CurDo()->DOpercent_arr);  
			log_u.log.log_data.EC_TDS = get_CurDo()->DOpercent.value_f;
			log_u.log.log_data.EC_salinity = get_CurDo()->pH_Vol.value_f ; 

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
			log_type=5;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'O';
			log_u.log.type_str[1] = 'R';
			log_u.log.type_str[2] = 'P';		
      index=	log_count.log_count_Type[log_type];			
			
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
			log_type=6;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'N';
			log_u.log.type_str[1] = 'H';
			log_u.log.type_str[2] = '3';
			log_u.log.type_str[3] = 'N';	
      index=	log_count.log_count_Type[log_type];
			
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
			log_type=7;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'F';
			log_u.log.type_str[1] = 0x00;
			log_u.log.type_str[2] = 0x00;			
      index=	log_count.log_count_Type[log_type];			
			
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
			log_type=8;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'C';
			log_u.log.type_str[1] = 'L';
			log_u.log.type_str[2] = 0x00;	
      index=	log_count.log_count_Type[log_type];
			
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
			log_type=9;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'C';
			log_u.log.type_str[1] = 'h';
			log_u.log.type_str[2] = 'l';	
      index=	log_count.log_count_Type[log_type];
			
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
			log_type=10;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'B';
			log_u.log.type_str[1] = 'g';
			log_u.log.type_str[2] = 'a';	
      index=	log_count.log_count_Type[log_type];
		
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
			log_type=11;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'C';
			log_u.log.type_str[1] = 'O';
			log_u.log.type_str[2] = 'D';
      index=	log_count.log_count_Type[log_type];
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
			log_type=12;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'M';
			log_u.log.type_str[1] = 'L';
			log_u.log.type_str[2] = 'S';
			log_u.log.type_str[3] = 'S';
      index=	log_count.log_count_Type[log_type];

			log_u.log.log_data.MLSS_mg_L = get_CurDo()->DOmgl.value_f;
			break;

		case TYPE_Oiw:
      
			if(get_CurDo()->modbus_id == OiW_yushan_DA511_ModbusID)
			{
				log_type=14;
				if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
				{
					return 0;
				}
				log_u.log.type_str[0] = 'O';
				log_u.log.type_str[1] = 'i';
				log_u.log.type_str[2] = 'W';
				index=	log_count.log_count_Type[log_type];

				log_u.log.log_data.OIW_DA511_mg_L = atof(get_CurDo()->DOmgl_arr);
				if(setting_Get_Temp_Unit())
				{
					log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
				}
				else
				{
					log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
				}	
			}
			else
			{
				log_type=13;
				if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
				{
					return 0;
				}
				log_u.log.type_str[0] = 'O';
				log_u.log.type_str[1] = 'i';
				log_u.log.type_str[2] = 'W';
				index=	log_count.log_count_Type[log_type];

				log_u.log.log_data.OIW_mg_L = atof(get_CurDo()->DOmgl_arr);

				if(setting_Get_Temp_Unit())
				{
					log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
				}
				else
				{
					log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
				}	
			}
			break;

		case TYPE_TSS:
			log_type=15;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'T';
			log_u.log.type_str[1] = 'S';
			log_u.log.type_str[2] = 'S';
      index=	log_count.log_count_Type[log_type];

			log_u.log.log_data.TSS_mg_L = atof(get_CurDo()->DOmgl_arr);

			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}	
			break;

		case TYPE_SAL:
			log_type=16;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'S';
			log_u.log.type_str[1] = 'A';
			log_u.log.type_str[2] = 'L';		
      index=	log_count.log_count_Type[log_type];			
			
			if(get_CurDo()->Measure_Range.value_f == 0)//ppt
			{
				log_u.log.log_data.EC_salinity = get_CurDo()->pH_Vol.value_f ; 
			}
			else
			{
				log_u.log.log_data.EC_salinity = get_CurDo()->pH_Vol.value_f *10; 
			}


			if(setting_Get_Temp_Unit())
			{
			    log_u.log.log_data.temperature = (get_CurDo()->temperature.value_f-32)/1.8;
			}
			else
			{
		    	log_u.log.log_data.temperature = atof(get_CurDo()->temperature_arr);//DO_GetTemperature(get_CurDo());			
			}	
			break;
		
		case TYPE_TDS:
			log_type=17;
			if(log_count.log_count_Type[log_type] >= LogCount_max)//如果写满了直接跳过
			{
				return 0;
			}
			log_u.log.type_str[0] = 'T';
			log_u.log.type_str[1] = 'D';
			log_u.log.type_str[2] = 'S';		
      index=	log_count.log_count_Type[log_type];			
			
			log_u.log.log_data.EC_TDS = get_CurDo()->DOmgl.value_f; 

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
	


	log_WriteLog(&log_u, index,log_type);
	
	log_SetLogCount(index+1,log_type);
	return 1;
}

void send_char(char ch)
{
	while(!(USART1->SR & (1<<6)));
	USART1->DR = ch;
}

uint16_t DO_Send_Count = 0,		  pH_Send_Count = 0,		Tur_Send_Count = 0,
		     FCL_Send_Count = 0,		EC_Send_Count = 0,		ORP_Send_Count = 0,
		     NH4_Send_Count = 0,		F_Send_Count = 0,		  CL_Send_Count = 0,
		     Chl_Send_Count = 0,		Bga_Send_Count = 0,		COD_Send_Count = 0,
		     MLSS_Send_Count = 0,	  OiW_Send_Count = 0,		OiW_YUSHAN_Send_Count = 0,
		     TSS_Send_Count = 0,	  SAL_Send_Count = 0,		TDS_Send_Count = 0;

void log_SendBytes()
{
	log_send_union log_send_u;
	log_send_u.log_send.head[0] = 0xFF;
	log_send_u.log_send.head[1] = 0x03;
	log_send_u.log_send.head[2] = 0x02;
	
	 if( TDS_Send_Count < log_count.log_count_Type[17] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, TDS_Send_Count,17);
		 TDS_Send_Count++;
	 }
	 else if( SAL_Send_Count < log_count.log_count_Type[16] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, SAL_Send_Count,16);
		 SAL_Send_Count++;
	 }
	 else if( TSS_Send_Count < log_count.log_count_Type[15] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, TSS_Send_Count,15);
		 TSS_Send_Count++;
	 }	
	 else if( OiW_YUSHAN_Send_Count < log_count.log_count_Type[14] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, OiW_YUSHAN_Send_Count,14);
		 OiW_YUSHAN_Send_Count++;
	 }
	 else if( OiW_Send_Count < log_count.log_count_Type[13] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, OiW_Send_Count,13);
		 OiW_Send_Count++;
	 }
	 else if( MLSS_Send_Count < log_count.log_count_Type[12] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, MLSS_Send_Count,12);
		 MLSS_Send_Count++;
	 }
	 else if( COD_Send_Count < log_count.log_count_Type[11] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, COD_Send_Count,11);
		 COD_Send_Count++;
	 }		 
	 else if( Bga_Send_Count < log_count.log_count_Type[10] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, Bga_Send_Count,10);
		 Bga_Send_Count++;
	 }		 
	 else if( Chl_Send_Count < log_count.log_count_Type[9] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, Chl_Send_Count,9);
		 Chl_Send_Count++;
	 }	
	 else if( CL_Send_Count < log_count.log_count_Type[8] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, CL_Send_Count,8);
		 CL_Send_Count++;
	 }	
	 else if( F_Send_Count < log_count.log_count_Type[7] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, F_Send_Count,7);
		 F_Send_Count++;
	 }		 
	 else if( NH4_Send_Count < log_count.log_count_Type[6] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, NH4_Send_Count,6);
		 NH4_Send_Count++;
	 }		 	 
	 else if( ORP_Send_Count < log_count.log_count_Type[5] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, ORP_Send_Count,5);
		 ORP_Send_Count++;
	 }	
	 else if( EC_Send_Count < log_count.log_count_Type[4] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, EC_Send_Count,4);
		 EC_Send_Count++;
	 }
	 else if( FCL_Send_Count < log_count.log_count_Type[3] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, FCL_Send_Count,3);
		 FCL_Send_Count++;
	 }	 
	 else if( Tur_Send_Count < log_count.log_count_Type[2] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, Tur_Send_Count,2);
		 Tur_Send_Count++;
	 }	 
	 else if( pH_Send_Count < log_count.log_count_Type[1] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, pH_Send_Count,1);
		 pH_Send_Count++;
	 }		 
	 else if( DO_Send_Count < log_count.log_count_Type[0] )
	 {
		 log_ReadData(&log_send_u.log_send.logu, DO_Send_Count,0);
		 DO_Send_Count++;
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