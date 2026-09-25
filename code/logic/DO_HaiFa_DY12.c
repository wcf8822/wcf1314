#include "DO_HaiFa_DY12.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

uint8_t Sal_Flag = 0,Press_Flag = 0,Zero_Flag = 0,Full_Flag = 0;

int16_t SalValue = 0,PresssValue = 0, Zero_Value = 0,Full_Value = 0;

//获取设置盐度标志
uint8_t Get_DY12_Set_Sal(void)
{
	return Sal_Flag;
}

//获取设置盐度数值
int16_t Get_DY12_Set_Sal_Value(void)
{
	return SalValue;
}

//获取设置气压标志
uint8_t Get_DY12_Set_Press(void)
{
	return Press_Flag;
}

//获取设置气压数值
int16_t Get_DY12_Set_PressValue(void)
{
	return PresssValue;
}


//获取设置零氧校准标志
uint8_t Get_DY12_Set_Zero_Cal(void)
{
	return Zero_Flag;
}

//获取设置零氧校准数值
int16_t Get_DY12_Set_Zero_Cal_Value(void)
{
	return Zero_Value;
}

//获取设置满氧校准标志
uint8_t Get_DY12_Set_Full_Cal(void)
{
	return Full_Flag;
}

//获取设置满氧校准数值
int16_t Get_DY12_Set_Full_Cal_Value(void)
{
	return Full_Value;
}


//设置盐度标志
void Set_DY12_Sal_Flag(uint8_t value)
{
	Sal_Flag = value;
}

//设置气压标志
void Set_DY12_Press_Flag(uint8_t value)
{
	Press_Flag = value;
}

//设置零氧校准标志
void Set_DY12_Zero_Flag(uint8_t value)
{
	Zero_Flag = value;
}

//设置满氧校准标志
void Set_DY12_Full_Flag(uint8_t value)
{
	Full_Flag = value;
}


//设置盐度数值
void Set_DY12_Sal_Value(int16_t value)
{
	SalValue = value;
	if(SalValue >= 10000)
	{
		SalValue = 10000;
	}
}

//设置气压数值
void Set_DY12_Press_Value(int16_t value)
{
	PresssValue = value;
}

//设置零氧校准数值
void Set_DY12_Zero_Value(int16_t value)
{
	Zero_Value = value;
}

//设置满氧校准数值
void Set_DY12_Full_Value(int16_t value)
{
	Full_Value = value;
}

/*获取探头Modbus通讯ID*/
void DO_HaiFa_DY12_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x0c;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	// rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	// rs485_usart.tx_buf[1] = 0x03;
	// rs485_usart.tx_buf[2] = 0x00;
	// rs485_usart.tx_buf[3] = 0x03;
	// rs485_usart.tx_buf[4] = 0x00;
	// rs485_usart.tx_buf[5] = 0x08;

	// SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetSentType(DO_SendType_GetModbusId);
}

/*获取溶解氧数值及其他测量数值  */
void DO_HaiFa_DY12_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x03;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x08;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}

/*设置sn号和软硬件版本*/
void DO_HaiFa_DY12_rs485_SetSN_SH(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	ptd->SN[0]='H';
	ptd->SN[1]='F';
	ptd->SN[2]='-';
	ptd->SN[3]='D';	
	ptd->SN[4]='Y';
	ptd->SN[5]='1';	
	ptd->SN[6]='2';
	ptd->SN[7]='_';	
	ptd->SN[8]='D';
	ptd->SN[9]='O';	
	ptd->SN[10]=' ';
	ptd->SN[11]=' ';		

	ptd->HWV[0] = '1';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = '0';
	ptd->HWV[3] = '\0';

	ptd->SWV[0] = '1';
	ptd->SWV[1] = '.';
	ptd->SWV[2] = '0';
	ptd->SWV[3] = '\0';
}

/*设置参数允许修改*/
void DO_HaiFa_DY12_rs485_Set_Cmd_open(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x28;
	rs485_usart.tx_buf[4] = 0x5a;
	rs485_usart.tx_buf[5] = 0x5a;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Start);
}

/*禁止参数修改*/
void DO_HaiFa_DY12_rs485_Set_Cmd_close(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x28;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x00;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Stop);
}

/*设置盐度数值-千分单位*/
void DO_HaiFa_DY12_rs485_Set_Sal(PtrToDOProbe ptd,int16_t value)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x13;
	rs485_usart.tx_buf[4] = value >> 8;
	rs485_usart.tx_buf[5] = value;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetSalinity);
}

/*设置压力-Kpa*/
void DO_HaiFa_DY12_rs485_Set_Press(PtrToDOProbe ptd,int16_t value)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x12;
	rs485_usart.tx_buf[4] = value >> 8;
	rs485_usart.tx_buf[5] = value;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetPressure);
}

/*恢复出厂设置*/
void DO_HaiFa_DY12_rs485_ClearCal(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0xff;
	rs485_usart.tx_buf[5] = 0xff; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

/*零点和满氧校准*/
void DO_HaiFa_DY12_rs485_Set_Cal(PtrToDOProbe ptd,uint8_t value)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x01;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = value;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetFullCal);
}

/*退出校准*/
void DO_HaiFa_DY12_rs485_Set_Break(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF_DY12_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x01;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x00;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetZeroCal);
}


void DO_HaiFa_DY12_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
	uint16_u DO_mg_value;

	if(ptd == NULL) return;
	
	//溶解氧mg 
	DO_mg_value.value_arr[0] =	dat[1];
	DO_mg_value.value_arr[1] =	dat[0];
	ptd->DOmgl.value_f = (float)DO_mg_value.value_f/100;
	//---
	//饱和溶解氧
	//饱和度
	DO_mg_value.value_arr[0] =	dat[7];
	DO_mg_value.value_arr[1] =	dat[6];
	ptd->DOpercent.value_f = (float)DO_mg_value.value_f/10;
	//---
	//盐度
	DO_mg_value.value_arr[0] =	dat[11];
	DO_mg_value.value_arr[1] =	dat[10];
	ptd->sal.value_f = (float)DO_mg_value.value_f/100;
	//压力
	DO_mg_value.value_arr[0] =	dat[13];
	DO_mg_value.value_arr[1] =	dat[12];
	ptd->press.value_f = (float)DO_mg_value.value_f/100;
	//温度
	DO_mg_value.value_arr[0] =	dat[15];
	DO_mg_value.value_arr[1] =	dat[14];
	ptd->temperature.value_f = (float)DO_mg_value.value_f/100;					
									
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);
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


