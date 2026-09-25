#include "DO59.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"


/*获取探头Modbus通讯ID*/
void DO59_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x04;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 %   mg/L  温度   */
void DO59_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x04;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}




void DO59_rs485_GetSalinity(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x04;
	rs485_usart.tx_buf[3] = 0x06;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();	
	
	
	rs485_SetSentType(DO_SendType_GetSalinity);
	
}


void DO59_rs485_SetSalinity(PtrToDOProbe ptd, float sal)
{
	if(ptd == NULL) return;
	ptd->sal.value_f=sal;
	uint16_u Cal_Value;
	Cal_Value.value_f=sal * 100;

	rs485_usart.tx_buf[0]  = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x04;
	rs485_usart.tx_buf[3]  = 0x06;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];

	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetSalinity);
}


void DO59_rs485_GetPressure(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x04;
	rs485_usart.tx_buf[3] = 0x07;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();	
	
	rs485_SetSentType(DO_SendType_GetPressure);
	
}

void DO59_rs485_SetPressure(PtrToDOProbe ptd, float press)
{
	if(ptd == NULL) return;
	ptd->press.value_f = press;
	
	uint32_u Cal_Value;
	Cal_Value.value_f=press*1000;
	
	rs485_usart.tx_buf[0]  = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x04;
	rs485_usart.tx_buf[3]  = 0x07;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[2];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetPressure);
}



//  温度校准
void DO59_rs485_Set_Temp_Cal(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	int16_u Cal_Value;
	Cal_Value.value_f=Value * 10;
	rs485_usart.tx_buf[0]  = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x04;
	rs485_usart.tx_buf[3]  = 0x05;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}



// 零氧校准
void DO59_rs485_Set_Zero_Cal(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;

	rs485_usart.tx_buf[0]  = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x04;
	rs485_usart.tx_buf[3]  = 0x0A;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetZeroCal);
}


//  满氧校准
void DO59_rs485_Set_Slp_Cal(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	
	rs485_usart.tx_buf[0]  = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x04;
	rs485_usart.tx_buf[3]  = 0x0B;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetFullCal);
}



void DO59_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_DO59_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x04;
	rs485_usart.tx_buf[3] = 0x33;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

void DO59_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_mgl_temp = 0.0;
	float DO_Percent_temp = 0.0;
	uint16_u value1,value2,value3;

	if(ptd == NULL) return;
	
	
	value1.value_arr[0]=dat[1];
	value1.value_arr[1]=dat[0];
	
	value2.value_arr[0]=dat[3];
	value2.value_arr[1]=dat[2];

	value3.value_arr[0]=dat[5];
	value3.value_arr[1]=dat[4];
	
	ptd->DOpercent.value_f = value1.value_f;
	ptd->DOmgl.value_f = value2.value_f;
	ptd->temperature.value_f = value3.value_f;
	
	ptd->DOpercent.value_f = ptd->DOpercent.value_f/10.0;
	ptd->DOmgl.value_f = ptd->DOmgl.value_f/100.0;
	ptd->temperature.value_f = ptd->temperature.value_f/10.0;
	
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
		snprintf(ptd->DOpercent_arr,   7, "%6.1f", (ptd->DOpercent.value_f ));
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
			
			
			
			if(setting_GetIsOpen_SlideAvg_DO() && setting_GetSlideAvgTimes_DO()>=2)//开启并且次数最起码为2次
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
				snprintf(ptd->DOpercent_arr,   7, "%6.1f", DO_Percent_temp);
				snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
			
			
		}
	}
}


void DO59_SetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	ptd->HWV[0] =  '1';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = '0';
	ptd->HWV[3] = '\0';	

	ptd->SWV[0] =  '1';
	ptd->SWV[1] = '.';
	ptd->SWV[2] = '0';
	ptd->SWV[3] = '\0';	
}





void DO59_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	if(len == 0){
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='O';
		ptd->SN[5]='5';	
		ptd->SN[6]='9';
		ptd->SN[7]='-';	
		ptd->SN[8]='D';
		ptd->SN[9]='O';	
		ptd->SN[10]=' ';
		ptd->SN[11]=' ';	

	}else{
		for(uint8_t i =0; i<len; i++)
		{
			ptd->SN[i] = *(buff + i);
		}
	}		
}


void DO59_SetSalinityArr(PtrToDOProbe ptd, uint8_t* dat)
{
	if(ptd == NULL) return;
	uint16_u value;
	value.value_arr[0] = dat[1];
	value.value_arr[1] = dat[0];
	ptd->sal.value_f = value.value_f;
	ptd->sal.value_f = ptd->sal.value_f/100.0;
}

void DO59_SetPressureArr(PtrToDOProbe ptd, uint8_t* dat)
{
	if(ptd == NULL) return;
	uint32_u value;
	value.value_arr[0] = dat[3];
	value.value_arr[1] = dat[2];
	value.value_arr[2] = dat[1];
	value.value_arr[3] = dat[0];
	
	ptd->press.value_f = value.value_f;
	ptd->press.value_f = ptd->press.value_f/100.0;
}


