#include "pH_Dph07.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"


/*获取探头Modbus通讯ID*/
void pH_DpH07_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x08;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 pH  温度 */
void pH_DpH07_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x08;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}



/*获取sn*/
void pH_DpH07_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x08;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取软硬件版本号*/
void pH_DpH07_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x08;
	rs485_usart.tx_buf[3] = 0x7c;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}

/*获取国标和美标*/
void pH_DpH07_rs485_GetStander(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x08;
	rs485_usart.tx_buf[3] = 0x0a;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Set_Mes_mode);
	
}
void pH_DpH07_rs485_SetStander(PtrToDOProbe ptd, uint16_t Value)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
  Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x08;
	rs485_usart.tx_buf[3]  = 0x0a;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Set_Mes_Time);
}

void pH_DpH07_SetStander(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=0;
	Cal_Value.value_arr[0]=dat[1];
	Cal_Value.value_arr[1]=dat[0];
	setting_SetIs_pH_Group(Cal_Value.value_f == 1 ? 0 : 1);  		
}


// pH 标定   0x11:6.86  0x12:4.01  0x13:9.18      0x21:7.00  0x22:4.00  0x23:10.01
void pH_DpH07_rs485_Cal_pH(PtrToDOProbe ptd,uint16_t Value)
{
	if(ptd == NULL) return;

	uint16_u Cal_Value;
  Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x08;
	rs485_usart.tx_buf[3]  = 0x08;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}



void pH_DpH07_rs485_SetTemp(PtrToDOProbe ptd, uint16_t Value)
{//设置温度
	if(ptd == NULL) return;
	uint16_u Cal_Value;
  Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x08;
	rs485_usart.tx_buf[3]  = 0x0b;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}


void pH_DpH07_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = pH_DpH07_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x08;
	rs485_usart.tx_buf[3] = 0x33;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void pH_DpH07_SetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;

	ptd->SN[0]='L';
	ptd->SN[1]='H';
	ptd->SN[2]='-';
	ptd->SN[3]='D';	
	ptd->SN[4]='p';
	ptd->SN[5]='H';	
	ptd->SN[6]='0';
	ptd->SN[7]='7';	
	ptd->SN[8]='_';
	ptd->SN[9]='p';	
	ptd->SN[10]='H';
	ptd->SN[11]=' ';	
		
}


void pH_DpH07_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{

	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;	
	uint16_u pH_Value;	
	uint16_u Temp_Value;	
	int16_u pH_Vol_Value;	
	if(ptd == NULL) return;
	pH_Value.value_arr[0]=dat[1];
	pH_Value.value_arr[1]=dat[0];
	
	Temp_Value.value_arr[0]=dat[3];
	Temp_Value.value_arr[1]=dat[2];
	
	pH_Vol_Value.value_arr[0]=dat[5];
	pH_Vol_Value.value_arr[1]=dat[4];
	
	ptd->DOmgl.value_f=(float)pH_Value.value_f/100.0;
	ptd->temperature.value_f=(float)Temp_Value.value_f/10.0;
	ptd->pH_Vol.value_f=(float)pH_Vol_Value.value_f/10.0;
	
	ptd->DOpercent.value_f=0.0;
	snprintf(ptd->DOpercent_Vol_arr,  8, "%5.2f", (ptd->pH_Vol.value_f ));


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
		
		snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.2f", (ptd->DOpercent.value_f ));
		snprintf(ptd->DOmgl_arr,       7, "%5.2f", ptd->DOmgl.value_f);
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
			
			
			
			if(setting_GetIsOpen_SlideAvg_pH() && setting_GetSlideAvgTimes_pH()>=2)//开启并且次数最起码为2次
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
				snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
				snprintf(ptd->DOpercent_arr,   7, "%6.2f", DO_Percent_temp);
				snprintf(ptd->DOmgl_arr,       7, "%5.2f", DO_mgl_temp);
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
			
			
		}
	}
}


