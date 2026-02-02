#include "OiW_yushan.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"


/*获取探头Modbus通讯ID*/
void OiW_yushan_rs485_GetModbusId(uint8_t modbus_id)	
{
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x30;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 水中油 温度 */
void OiW_yushan_rs485_GetValue(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);

	rs485_SetSentType(DO_SendType_GetTempTwoDO);	
}


/*获取sn*/
void OiW_yushan_rs485_GetSN(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x09;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x07;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
}

/*设置sn*/
void OiW_yushan_rs485_SetSN(PtrToDOProbe ptd,uint8_t *dat)
{
	for(uint8_t i =0; i<12; i++)
	{
		ptd->SN[i] = *(dat + i);
	}	
}

/*获取软硬件版本号*/
void OiW_yushan_rs485_GetSHWVersion(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}

/*获取校准kb值*/
void OiW_yushan_rs485_GetCal_k_b(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x11;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetDOPercent);
	
}

/*设置需要显示的kb值*/
void OiW_yushan_rs485_SetCal_k_b(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	//k ptd->sal.value_f
	ptd->sal.value_arr[0] = dat[0];
	ptd->sal.value_arr[1] = dat[1];
	ptd->sal.value_arr[2] = dat[2];
	ptd->sal.value_arr[3] = dat[3];
	snprintf(ptd->tocmgl_Vol_arr, 7, "%6.2f", ptd->sal.value_f);

	//b ptd->press.value_f
	ptd->press.value_arr[0] = dat[4];
	ptd->press.value_arr[1] = dat[5];
	ptd->press.value_arr[2] = dat[6];
	ptd->press.value_arr[3] = dat[7];
	snprintf(ptd->DOmgl_Vol_arr, 7, "%6.2f", ptd->press.value_f);
}

/*设置软硬件版本*/
void OiW_yushan_rs485_SetSHWVersion(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	ptd->HWV[0] = (*dat) + '0';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = (*(dat+1))+'0';
	ptd->HWV[3] = '\0';
	
	ptd->SWV[0] = (*(dat+2))+'0';
	ptd->SWV[1] = '.';
	ptd->SWV[2] = (*(dat+3))+'0';
	ptd->SWV[3] = '\0';
}

/*Kb值恢复校准系数 k = 1 b = 0*/
void OiW_yushan_rs485_Set_OIW_K_b_Reset(PtrToDOProbe ptd,float value_1,float value_2,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	float_u Cal_Value_k;
	float_u Cal_Value_b;
	Cal_Value_k.value_f = value_1;
	Cal_Value_b.value_f = value_2;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x11;
	rs485_usart.tx_buf[3] = 0x00;//地址
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;//数量
	rs485_usart.tx_buf[6] = 0x08;//字节数

	rs485_usart.tx_buf[7] = Cal_Value_k.value_arr[0];
	rs485_usart.tx_buf[8] = Cal_Value_k.value_arr[1];
	rs485_usart.tx_buf[9] = Cal_Value_k.value_arr[2];
	rs485_usart.tx_buf[10] = Cal_Value_k.value_arr[3];//数据

	rs485_usart.tx_buf[11] = Cal_Value_b.value_arr[0];
	rs485_usart.tx_buf[12] = Cal_Value_b.value_arr[1];
	rs485_usart.tx_buf[13] = Cal_Value_b.value_arr[2];
	rs485_usart.tx_buf[14] = Cal_Value_b.value_arr[3];//数据

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

// setting_GetSlideAvgTimes_OIW_ppm
void OiW_yushan_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
	
	if(ptd == NULL) return;
	
	//水中油
	ptd->DOmgl.value_arr[0] =	dat[4];
	ptd->DOmgl.value_arr[1] =	dat[5];
	ptd->DOmgl.value_arr[2] =	dat[6];
	ptd->DOmgl.value_arr[3] =	dat[7];
	
	//温度值
	ptd->temperature.value_arr[0] =	dat[0];
	ptd->temperature.value_arr[1] =	dat[1];
	ptd->temperature.value_arr[2] =	dat[2];
	ptd->temperature.value_arr[3] =	dat[3];
	
	// ptd->DOmgl.value_f = 123.6;
	if(setting_Get_Temp_Unit())//温度单位为华氏度时需要做以下换算
	{
		ptd->temperature.value_f = ptd->temperature.value_f*1.8+32;//摄氏度转华氏度公式
		if(ptd->temperature.value_f > 140)
		{
			ptd->temperature.value_f = 140;
		}
		else if(ptd->temperature.value_f < 32)
		{
			ptd->temperature.value_f = 32;
		}
	}
	
	if(ptd->DOmgl.value_f <=0 )
	{
		ptd->DOmgl.value_f = 0;
	}
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);//温度

		if(ptd->DOmgl.value_f >= 1000.00f)
		{
			snprintf(ptd->DOmgl_arr,       7, "%6d", (uint16_t)ptd->DOmgl.value_f);
		}
		else if(ptd->DOmgl.value_f >= 100.00f)
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.1f", ptd->DOmgl.value_f);//水中油
		}
		else 
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.2f", ptd->DOmgl.value_f);//水中油
		}
		
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
			
			if(ptd->modbus_id == OiW_yushan_ModbusID)
			{
				if(setting_GetIsOpen_SlideAvg_OIW() && setting_GetSlideAvgTimes_OIW()>=2)//开启并且次数最起码为2次
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
			}
			else
			{
				if(setting_GetIsOpen_SlideAvg_OIW_ppm() && setting_GetSlideAvgTimes_OIW_ppm()>=2)//开启并且次数最起码为2次
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
			}
			
			if(!DO_GetValueLocked(ptd))
			{
				snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);

				if(DO_mgl_temp >= 1000.00f)
				{
					snprintf(ptd->DOmgl_arr,       7, "%6d", (uint16_t)DO_mgl_temp);
				}
				else if(DO_mgl_temp >= 100.00f)
				{
					snprintf(ptd->DOmgl_arr,       7, "%6.1f", DO_mgl_temp);
				}
				else
				{
					snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
				}
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->temperature_sum = 0.0;		
		}
	}
}