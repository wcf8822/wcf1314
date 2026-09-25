#include "DX01.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

/*获取探头Modbus通讯ID*/
void DX01_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 悬浮物和温度  */
void DX01_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}

/*获取 温度校准值  */
void DX01_rs485_GetTemp_CalValue(PtrToDOProbe ptd)
{
	// if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x0b;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetSalinity);
}



/*设置温度校准值  */
void DX01_rs485_SetTemp_CalValue(PtrToDOProbe ptd,uint8_t *dat)
{
	// if(ptd == NULL) return;
	// int16_u DX01_Cal_Temp_value;

	// DX01_Cal_Temp_value.value_arr[0] = dat[1];
	// DX01_Cal_Temp_value.value_arr[1] =	dat[0];

	// ptd->press.value_f = (float)DX01_Cal_Temp_value.value_f / 100;
}

/*获取 自动清洗时间间隔  */
void DX01_rs485_GetClear_time(PtrToDOProbe ptd)
{
	// if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x0c;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_Get_Tur_Status);
}

void DX01_rs485_Get_lvbo_num(PtrToDOProbe ptd)
{
	// if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x6e;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetMesParameter);
}

/*设置自动清洗时间间隔  */
void DX01_rs485_SetClear_time(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	uint16_u DX01_clear_time;

	DX01_clear_time.value_arr[0] = dat[1];
	DX01_clear_time.value_arr[1] = dat[0];

	ptd->Measure_Range.value_f = DX01_clear_time.value_f;

	snprintf(ptd->DOmgl_Vol_arr,       7, "%4d", ptd->Measure_Range.value_f);
}

void DX01_rs485_Set_lvbo_num(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	uint16_u DX01_lvbo_value;

	DX01_lvbo_value.value_arr[0] = dat[1];
	DX01_lvbo_value.value_arr[1] = dat[0];

	ptd->DC17_Mes_Time.value_f = DX01_lvbo_value.value_f;

	// snprintf(ptd->DOmgl_Vol_arr,       7, "%4d", ptd->DC17_Mes_Time.value_f);
}

void DX01_rs485_Write_Avg_num(PtrToDOProbe ptd,uint8_t value)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0]  = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x07;
	rs485_usart.tx_buf[3]  = 0x6e;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = value;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetFullCal);
}




/*获取 平均次数  */
void DX01_rs485_Get_Avg_num(PtrToDOProbe ptd)
{
	// if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x6e;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetPressure);
}

/*设置平均次数  */
void DX01_rs485_Set_Avg_num(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	int16_u DX01_Avgvalue;

	DX01_Avgvalue.value_arr[0] = dat[1];
	DX01_Avgvalue.value_arr[1] = dat[0];

	// ptd->ORP_value.value_f = DX01_Avgvalue.value_f;
}


int16_t xuanfuwu_temp_va;
// 悬浮物 温度校准
void DX01_rs485_Set_Temp_Cal(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;

	xuanfuwu_temp_va =Value + get_CurDo()->ORP_value.value_f;

	if(xuanfuwu_temp_va < 0)
	{
		xuanfuwu_temp_va = -xuanfuwu_temp_va;
		Cal_Value.value_f = 0xffff - xuanfuwu_temp_va + 1;
	}
	else
	{
		Cal_Value.value_f = xuanfuwu_temp_va;
	}
	rs485_usart.tx_buf[0]  = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x07;
	rs485_usart.tx_buf[3]  = 0x0b;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}

void DX01_rs485_Set_Temp_Cal_zero(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	rs485_usart.tx_buf[0]  = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x07;
	rs485_usart.tx_buf[3]  = 0x0b;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x00;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}

// 悬浮物 零点校准
void DX01_rs485_Set_Zero_Cal(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	int16_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x07;
	rs485_usart.tx_buf[3]  = 0x72;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetZeroCal);
}


// 悬浮物 斜率校准
void DX01_rs485_Set_Slp_Cal(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x07;
	rs485_usart.tx_buf[3]  = 0x73;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;

	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];  
	
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
}


void DX01_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0xF5;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x79; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

//立即清洗一次刷子
void DX01_rs485_Set_Clear(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x70;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Set_Mes_mode);
}

//设置自动清洗的时间间隔
void DX01_rs485_Set_Clear_time(PtrToDOProbe ptd,uint16_t value)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = LH_DX01_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x0c;
	rs485_usart.tx_buf[4] = value >> 8;
	rs485_usart.tx_buf[5] = value; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Set_Mes_Time);
}


void DX01_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_mgl_temp = 0.0;
	
	uint32_u DX01_Value;
	int16_u DX01_Temp_Value;
	if(ptd == NULL) return;
	
	DX01_Temp_Value.value_f=0.0;
	DX01_Value.value_f=0.0;

  	DX01_Value.value_arr[0] =	dat[1];
  	DX01_Value.value_arr[1] =	dat[0];
	DX01_Value.value_arr[2] =	dat[3];
	DX01_Value.value_arr[3] =	dat[2];

	DX01_Temp_Value.value_arr[0] = dat[5];
	DX01_Temp_Value.value_arr[1] =	dat[4];

	ptd->DOmgl.value_f = (float)DX01_Value.value_f / 1000;
	ptd->temperature.value_f = (float)DX01_Temp_Value.value_f / 100;
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
		
		if(setting_GetTemp_jingdu() == 0)
		{
			snprintf(ptd->temperature_arr, 6, "%5d", (uint16_t)ptd->temperature.value_f);
		}
		else if(setting_GetTemp_jingdu() == 5)
		{
			snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);
		}
		else
		{
			snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
		}

		if(ptd->DOmgl.value_f >= 1000.00)
		{
			snprintf(ptd->DOmgl_arr,       7, "%6d", (uint16_t)ptd->DOmgl.value_f);
		}
		else if (ptd->DOmgl.value_f >= 100.00)
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.1f", ptd->DOmgl.value_f);
		}
		else
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.2f", ptd->DOmgl.value_f);
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
			
			if(setting_GetIsOpen_SlideAvg_TSS() && setting_GetSlideAvgTimes_TSS()>=2)//开启并且次数最起码为2次
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
				if(setting_GetTemp_jingdu() == 0)
				{
					snprintf(ptd->temperature_arr, 6, "%5d", (uint16_t)temperature_temp);
				}
				else if(setting_GetTemp_jingdu() == 5)
				{
					snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
				}
				else
				{
					snprintf(ptd->temperature_arr, 6, "%5.2f", temperature_temp);
				}

				if(DO_mgl_temp >= 1000.00)
				{
					snprintf(ptd->DOmgl_arr,       7, "%6d", (uint16_t)DO_mgl_temp);
				}
				else if (DO_mgl_temp >= 100.00)
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
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;	
		}
	}
}


void DX01_SetSHWVersion(PtrToDOProbe ptd)
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

uint8_t flag;
void DX01_SetTemp_Cal_value(PtrToDOProbe ptd ,uint8_t *dat)
{
	if(ptd == NULL) return;

	ptd->ORP_value.value_arr[0] = dat[1];
	ptd->ORP_value.value_arr[1] = dat[0];

	flag = dat[0] & 0x80;
	//正数
	if(flag == 0)
	{

	}
	else
	{
		ptd->ORP_value.value_f = 0xffff - ptd->ORP_value.value_f + 1;
		ptd->ORP_value.value_f = -ptd->ORP_value.value_f ;
	}
}


void DX01_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	if(len == 0){
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='X';
		ptd->SN[5]='0';	
		ptd->SN[6]='1';
		ptd->SN[7]='-';	
		ptd->SN[8]='T';
		ptd->SN[9]='S';	
		ptd->SN[10]='S';
		ptd->SN[11]=' ';	

	}else{
		for(uint8_t i =0; i<len; i++)
		{
			ptd->SN[i] = *(buff + i);
		}
	}		
}