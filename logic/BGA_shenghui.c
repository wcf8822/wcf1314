#include "BGA_shenghui.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"
#include "interfacial.h"
#include "time.h"

/*获取探头Modbus通讯ID*/
void BGA_shenghui_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 蓝绿藻 温度 */
void BGA_shenghui_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}

/*获取 蓝绿藻电压 温度电压 */
void BGA_shenghui_rs485_GetVol(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x6d;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetDOPercent);
	
}


/*获取sn*/
void BGA_shenghui_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
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
void BGA_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x45;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}

/*获取标定1 2 , k b数据*/
void BGA_shenghui_rs485_Get_Cal_INFO(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x83;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x08;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}


/*获取标定1 2 温度数据*/
void BGA_shenghui_rs485_Get_Cal_temp(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x91;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}


void BGA_shenghui_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x51;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void BGA_shesnghui_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x57;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = ptd->temperature.value_arr[1];
	rs485_usart.tx_buf[8]  = ptd->temperature.value_arr[0];
	rs485_usart.tx_buf[9]  = ptd->temperature.value_arr[3];
	rs485_usart.tx_buf[10] = ptd->temperature.value_arr[2];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}


//BGA 标定1
void BGA_shenghui_cal_rs485_Frist(PtrToDOProbe ptd,float Value)

{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = Bga_shenghui_ModbusID;
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


//BGA 标定2
void BGA_shenghui_cal_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = Bga_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x6c;
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

void BGA_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_mgl_temp = 0.0;
	
	if(ptd == NULL) return;
	
	ptd->DOmgl.value_arr[0] =	dat[1];
	ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];
	
	ptd->temperature.value_arr[0] =	dat[5];
	ptd->temperature.value_arr[1] =	dat[4];
	ptd->temperature.value_arr[2] =	dat[7];
	ptd->temperature.value_arr[3] =	dat[6];	

	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		ptd->last_DOmgl = ptd->DOmgl.value_f;

		if(ptd->DOmgl.value_f >= 10000)//Kcells/mL
		{
			if ((ptd->DOmgl.value_f / 1000.0) < 10.0)
			{
				snprintf(ptd->DOmgl_arr,       5, "%3.1f", (ptd->DOmgl.value_f /1000.0));
			}
			else if((ptd->DOmgl.value_f / 1000.0) < 100.0)
			{
				snprintf(ptd->DOmgl_arr,       4, "%3d", (unsigned int)(ptd->DOmgl.value_f /1000.0));
			}
			else if((ptd->DOmgl.value_f / 1000.0) < 1000.0)
			{
				snprintf(ptd->DOmgl_arr,       4, "%3d", (unsigned int)(ptd->DOmgl.value_f /1000.0));
			}
			
		}
		else//Cells/mL
		{
			if(ptd->DOmgl.value_f < 10.0)
			{
				snprintf(ptd->DOmgl_arr,       5, "%3.2f", ptd->DOmgl.value_f);
			}
			else if(ptd->DOmgl.value_f < 100.0)
			{
				snprintf(ptd->DOmgl_arr,       5, "%4.1f", ptd->DOmgl.value_f);
			}
			else
			{
				snprintf(ptd->DOmgl_arr,       5, "%4d", (unsigned int)(ptd->DOmgl.value_f));
			}
		}
		snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
	}
	else
	{
		ptd->DOmgl_sum += ptd->DOmgl.value_f;
		ptd->temperature_sum += ptd->temperature.value_f;
		
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		ptd->last_DOmgl = ptd->DOmgl.value_f;//更新一下上次的值
		
		if(++ptd->update_count >= 3)
		{
			
			temperature_temp = ptd->temperature_sum / ((float)ptd->update_count);
			DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
			
			if(setting_GetIsOpen_SlideAvg_Bga() && setting_GetSlideAvgTimes_Bga()>=2)//开启并且次数最起码为2次
			{
				if(is_FirstFilter)
				{
					is_FirstFilter = 0;
					
					filter_clear(&(ptd->queue_domgl));
					filter_clear(&(ptd->queue_temp));
				}
				
				filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
				filter_inset2arr(&(ptd->queue_temp), temperature_temp);
				
				temperature_temp  = filter_get_avg(&(ptd->queue_temp)); 
				DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
			}
			else
			{
				is_FirstFilter = 1;
			}
			
			if(!DO_GetValueLocked(ptd))
			{
				snprintf(ptd->temperature_arr, 6, "%5.2f", temperature_temp);
				if(DO_mgl_temp >= 10000)//Kcells/mL
				{
					if ((DO_mgl_temp / 1000.0) < 10.0)
					{
						snprintf(ptd->DOmgl_arr,       5, "%3.1f", (DO_mgl_temp /1000.0));
					}
					else if((DO_mgl_temp / 1000.0) < 100.0)
					{
						snprintf(ptd->DOmgl_arr,       4, "%3d", (unsigned int)(DO_mgl_temp /1000.0));
					}
					else if((DO_mgl_temp / 1000.0) < 1000.0)
					{
						snprintf(ptd->DOmgl_arr,       4, "%3d", (unsigned int)(DO_mgl_temp /1000.0));
					}
				}
				else//Cells/mL
				{
					if(DO_mgl_temp < 10.0)
					{
						snprintf(ptd->DOmgl_arr,       5, "%3.2f", DO_mgl_temp);
					}
					else if(DO_mgl_temp < 100.0)
					{
						snprintf(ptd->DOmgl_arr,       5, "%4.1f", DO_mgl_temp);
					}
					else
					{
						snprintf(ptd->DOmgl_arr,       5, "%4d", (unsigned int)(DO_mgl_temp));
					}
				}
			}
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->temperature_sum = 0.0;		
		}
	}
	rs485_SetIsChangeSenesor();
}


