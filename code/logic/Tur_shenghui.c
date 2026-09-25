#include "Tur_shenghui.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"


/*获取探头Modbus通讯ID*/
void Tur_shenghui_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 浊度  */
void Tur_shenghui_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}



/*获取sn*/
void Tur_shenghui_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x30;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取软硬件版本号*/
void Tur_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x45;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}


/*获取探头状态*/
void Tur_shenghui_rs485_Get_Tur_Status(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x66;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Get_Tur_Status);
	
}



// 浊度1 标定
void Tur_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x60;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x04;
	rs485_usart.tx_buf[6]  = 0x08;
 
	
	rs485_usart.tx_buf[7]  = timestamp.value_arr[1];
	rs485_usart.tx_buf[8]  = timestamp.value_arr[0];
	rs485_usart.tx_buf[9]  = timestamp.value_arr[3];
	rs485_usart.tx_buf[10] = timestamp.value_arr[2];
	
	rs485_usart.tx_buf[11]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[12]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[13]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[14] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

// 浊度 2 标定
void Tur_shenghui_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x64;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x04;
	rs485_usart.tx_buf[6]  = 0x08;
 
	
	rs485_usart.tx_buf[7]  = timestamp.value_arr[1];
	rs485_usart.tx_buf[8]  = timestamp.value_arr[0];
	rs485_usart.tx_buf[9]  = timestamp.value_arr[3];
	rs485_usart.tx_buf[10] = timestamp.value_arr[2];
	
	rs485_usart.tx_buf[11]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[12]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[13]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[14] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

// 浊度3 标定
void Tur_shenghui_rs485_Third(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x68;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x04;
	rs485_usart.tx_buf[6]  = 0x08;
 
	
	rs485_usart.tx_buf[7]  = timestamp.value_arr[1];
	rs485_usart.tx_buf[8]  = timestamp.value_arr[0];
	rs485_usart.tx_buf[9]  = timestamp.value_arr[3];
	rs485_usart.tx_buf[10] = timestamp.value_arr[2];
	
	rs485_usart.tx_buf[11]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[12]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[13]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[14] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}



void Tur_shenghui_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Tur_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x51;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void Tur_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
	
	
	if(ptd == NULL) return;
	
  ptd->DOmgl.value_arr[0] =	dat[1];
  ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];
	
  ptd->pH_Vol.value_arr[0] =	dat[5];
  ptd->pH_Vol.value_arr[1] =	dat[4];
	ptd->pH_Vol.value_arr[2] =	dat[7];
	ptd->pH_Vol.value_arr[3] =	dat[6];
	
	snprintf(ptd->DOpercent_Vol_arr,  8, "%5.2f", (ptd->pH_Vol.value_f*1000 ));
	
  ptd->temperature.value_f=0.0;
	ptd->DOpercent.value_f=0.0;
	
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.2f", (ptd->DOpercent.value_f ));
		snprintf(ptd->DOmgl_arr,       7, "%6.2f", ptd->DOmgl.value_f);
	}
	else
	{
		ptd->DOmgl_sum += ptd->DOmgl.value_f;
		ptd->DOpercent_sum += (ptd->DOpercent.value_f);
		ptd->temperature_sum += ptd->temperature.value_f;
		
		
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;//更新一下上次的值
		
		if(++ptd->update_count >= 3)
		{
			
			temperature_temp = ptd->temperature_sum / ((float)ptd->update_count);
			DO_Percent_temp = ptd->DOpercent_sum / ((float)ptd->update_count);
			DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
			
			
			
			if(setting_GetIsOpen_SlideAvg_Tur() && setting_GetSlideAvgTimes_Tur()>=2)//开启并且次数最起码为2次
			{
				if(is_FirstFilter)
				{
					is_FirstFilter = 0;
					
					filter_clear(&(ptd->queue_domgl));
					filter_clear(&(ptd->queue_dopercent));
					filter_clear(&(ptd->queue_temp));
				}
				
				filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
				filter_inset2arr(&(ptd->queue_dopercent), DO_Percent_temp);
				filter_inset2arr(&(ptd->queue_temp), temperature_temp);
				
				temperature_temp  = filter_get_avg(&(ptd->queue_temp)); 
				DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
				DO_Percent_temp  = filter_get_avg(&(ptd->queue_dopercent));
			}
			else
			{
				is_FirstFilter = 1;
			}
			
			if(!DO_GetValueLocked(ptd))
			{
				snprintf(ptd->temperature_arr, 6, "%5.2f", temperature_temp);
				snprintf(ptd->DOpercent_arr,   7, "%6.2f", DO_Percent_temp);
				snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
			
			
		}
	}
}


