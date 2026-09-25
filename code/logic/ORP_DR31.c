#include "ORP_DR31.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

/*获取探头Modbus通讯ID*/
void ORP_DR31_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = ORP_DR31_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x0B;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 ORP测量值  */
void ORP_DR31_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ORP_DR31_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x0B;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}


// ORP 标定
void ORP_DR31_rs485_CAL(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = ORP_DR31_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x0B;
	rs485_usart.tx_buf[3]  = 0x01;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
}


void ORP_DR31_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ORP_DR31_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x0B;
	rs485_usart.tx_buf[3] = 0x33;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void ORP_DR31_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	int16_t DO_mgl_temp = 0.0;
	
	if(ptd == NULL) return;
	
	ptd->ORP_value.value_arr[0] =	dat[1];
	ptd->ORP_value.value_arr[1] =	dat[0];

	ptd->DOmgl.value_f = ptd->ORP_value.value_f;
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->ORP_value.value_f;
		
		snprintf(ptd->DOmgl_arr,       6, "%5d", ptd->ORP_value.value_f);
	}
	else
	{
		ptd->ORP_sum += ptd->ORP_value.value_f;
	
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		
		ptd->last_DOmgl = ptd->ORP_value.value_f;//更新一下上次的值
		
		if(++ptd->update_count >= 3)
		{
			
			DO_mgl_temp = ptd->ORP_sum / (ptd->update_count);
			
			if(setting_GetIsOpen_SlideAvg_ORP() && setting_GetSlideAvgTimes_ORP()>=2)//开启并且次数最起码为2次
			{
				if(is_FirstFilter)
				{
					is_FirstFilter = 0;
					
					filter_clear(&(ptd->queue_domgl));
				}
				
				filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
				
				DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
			}
			else
			{
				is_FirstFilter = 1;
			}
			
			if(!DO_GetValueLocked(ptd))
			{
				snprintf(ptd->DOmgl_arr,       6, "%5d", DO_mgl_temp);
			}
			
			ptd->update_count = 0;
			ptd->ORP_sum =0;			
		}
	}
}


