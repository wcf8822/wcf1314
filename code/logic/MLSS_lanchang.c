#include "MLSS_lanchang.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

/*获取探头Modbus通讯ID*/
void MLSS_Lan_rs485_GetModbusId(void)
{
	rs485_usart.tx_buf[0] = MLSS_Lan_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x2e;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 悬浮物和温度  */
void MLSS_Lan_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Lan_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x2c;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}

/*获取 k,b数值  */
void MLSS_Lan_rs485_Get_K_B(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Lan_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x2a;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetKB);
}

//立即清洗一次刷子
void MLSS_Lan_rs485_Set_Clear(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Lan_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x2f;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x00; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

//数据解析
void MLSS_Lan_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_mgl_temp = 0.0;
	// uint16_u DZ09_Value;
	// uint16_u DZ09_Temp_Value;
	
	uint32_u DZ09_Value;
	int16_u DZ09_Temp_Value;
	if(ptd == NULL) return;
	
	DZ09_Temp_Value.value_f=0.0;
	DZ09_Value.value_f=0.0;

  	DZ09_Value.value_arr[0] =	dat[1];
  	DZ09_Value.value_arr[1] =	dat[0];
	DZ09_Value.value_arr[2] =	dat[3];
	DZ09_Value.value_arr[3] =	dat[2];

	DZ09_Temp_Value.value_arr[0] = dat[5];
	DZ09_Temp_Value.value_arr[1] =	dat[4];

	ptd->DOmgl.value_f = (float)DZ09_Value.value_f / 1000;
	ptd->temperature.value_f = (float)DZ09_Temp_Value.value_f / 100;
	
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
	
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
		snprintf(ptd->DOmgl_arr,       7, "%6.2f", ptd->DOmgl.value_f);
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
			
			if(setting_GetIsOpen_SlideAvg_Tur() && setting_GetSlideAvgTimes_Tur()>=2)//开启并且次数最起码为2次
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
				snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;	
		}
	}
}