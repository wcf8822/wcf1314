#include "DZ09.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

//浊度连接成功标志位
uint8_t TUR_Connect_Successure;

/*获取探头Modbus通讯ID*/
void DZ09_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x01;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 浊度  */
void DZ09_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x01;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x03;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}

/*获取 温度校准值  */
void DZ09_rs485_GetTemp_CalValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x01;
	rs485_usart.tx_buf[3] = 0x0b;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetSalinity);
}

/*获取sn*/
void DZ09_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x01;
	rs485_usart.tx_buf[3] = 0xC0;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x10;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取软硬件版本号*/
void DZ09_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x01;
	rs485_usart.tx_buf[3] = 0xD0;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);	
}

int16_t value_data;
// 浊度 温度校准
void DZ09_rs485_Set_Temp_Cal(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;

	value_data =Value + get_CurDo()->ORP_value.value_f;

	if(value_data < 0)
	{
		value_data = -value_data;
		Cal_Value.value_f = 0xffff - value_data + 1;
	}
	else
	{
		Cal_Value.value_f = value_data;
	}
	rs485_usart.tx_buf[0]  = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x01;
	rs485_usart.tx_buf[3]  = 0x0b;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	// rs485_usart.tx_buf[4]  = 0x00;
	// rs485_usart.tx_buf[5]  = 0x00;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}


void DZ09_rs485_Set_Temp_Cal_zero(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	rs485_usart.tx_buf[0]  = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x01;
	rs485_usart.tx_buf[3]  = 0x0b;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x00;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}



// 浊度 零点校准
void DZ09_rs485_Set_Zero_Cal(PtrToDOProbe ptd,int16_t Value)
{
	if(ptd == NULL) return;
	int16_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x01;
	rs485_usart.tx_buf[3]  = 0x72;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetZeroCal);
}


// 浊度 斜率校准
void DZ09_rs485_Set_Slp_Cal(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x01;
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


void DZ09_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ZS_DZ09_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x01;
	rs485_usart.tx_buf[3] = 0xF5;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x79; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

void DZ09_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
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
				snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);

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
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;	
		}
	}
}


void DZ09_SetSHWVersion(PtrToDOProbe ptd)
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

uint8_t flagTUR;
void DZ09_SetTemp_Cal_value(PtrToDOProbe ptd ,uint8_t *dat)
{
	if(ptd == NULL) return;

	ptd->ORP_value.value_arr[0] = dat[1];
	ptd->ORP_value.value_arr[1] = dat[0];

	flagTUR = dat[0] & 0x80;
	//正数
	if(flagTUR == 0)
	{

	}
	else
	{
		ptd->ORP_value.value_f = 0xffff - ptd->ORP_value.value_f + 1;
		ptd->ORP_value.value_f = -ptd->ORP_value.value_f ;
	}
}


void DZ09_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	if(len == 0){
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='Z';
		ptd->SN[5]='0';	
		ptd->SN[6]='9';
		ptd->SN[7]='-';	
		ptd->SN[8]='T';
		ptd->SN[9]='U';	
		ptd->SN[10]='R';
		ptd->SN[11]=' ';	

	}else{
		for(uint8_t i =0; i<len; i++)
		{
			ptd->SN[i] = *(buff + i);
		}
	}		
}


/*设置浊度连接标志位*/
void Set_TUR_Connect_FLAG(uint8_t value)
{
	TUR_Connect_Successure = value;
}
/*获取浊度连接标志位*/
uint8_t Get_TUR_Connect_FLAG(void)
{
	return TUR_Connect_Successure;
}


