#include "NH3N_DN02.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"


/*获取探头Modbus通讯ID*/
void NH3N_DN02_rs485_GetModbusId(void)
{	
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x61;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x10;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetSentType(DO_SendType_GetModbusId);
}

/*获取 NH3 pH 温度*/
void NH3N_DN02_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x61;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x10;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}


/*获取sn*/
void NH3N_DN02_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x62;
	rs485_usart.tx_buf[3] = 0x02;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x08;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取软硬件版本号*/
void NH3N_DN02_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x62;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}



//昇辉氨氮 1标定
void NH3N_DN02_rs485_Frist(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x63;
	rs485_usart.tx_buf[3]  = 0x06;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[2];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[3];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);

	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

//昇辉氨氮 2标定
void NH3N_DN02_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x63;
	rs485_usart.tx_buf[3]  = 0x08;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[2];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[3];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);

	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}


//昇辉氨氮 pH 4.01 标定
void NH3N_DN02_pH_rs485_Frist(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x63;
	rs485_usart.tx_buf[3]  = 0x00;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[2];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[3];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);

	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

//昇辉氨氮 pH 6.86 标定
void NH3N_DN02_pH_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x63;
	rs485_usart.tx_buf[3]  = 0x02;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[2];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[3];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

//昇辉氨氮 pH 9.18 标定
void NH3N_DN02_pH_rs485_Third(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x63;
	rs485_usart.tx_buf[3]  = 0x04;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[2];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[3];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}


void NH3N_DN02_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x63;
	rs485_usart.tx_buf[3] = 0x0e;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = ptd->temperature.value_arr[0];
	rs485_usart.tx_buf[8]  = ptd->temperature.value_arr[1];
	rs485_usart.tx_buf[9]  = ptd->temperature.value_arr[2];
	rs485_usart.tx_buf[10] = ptd->temperature.value_arr[3];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}

void NH3N_DN02_rs485_ClearCal(PtrToDOProbe ptd)
{//发送聚光溶解氧清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x63;
	rs485_usart.tx_buf[3] = 0x12;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	rs485_usart.tx_buf[6] = 0x02;	
	rs485_usart.tx_buf[7] = 0x01;
	rs485_usart.tx_buf[8] = 0x00;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 11);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void NH3N_DN02_rs485_Cleanse(PtrToDOProbe ptd)
{// 开启刷子  清洗
	if(ptd == NULL) return;
	
	rs485_usart.tx_buf[0] = NH3N_DN02_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x63;
	rs485_usart.tx_buf[3] = 0x11;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	rs485_usart.tx_buf[6] = 0x02;	
	rs485_usart.tx_buf[7] = 0x01;
	rs485_usart.tx_buf[8] = 0x00;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 11);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}


void NH3N_DN02_SetSN(PtrToDOProbe ptd, uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	for(uint8_t i =0; i<len; i++)
	{
		ptd->SN[i] = *(buff + i);
	}	
}
void NH3N_DN02_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	ptd->HWV[0] = (*data) + '0';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = (*(data+1))+'0';
	ptd->HWV[3] = '\0';
	
	ptd->SWV[0] = (*(data+2))+'0';
	ptd->SWV[1] = '.';
	ptd->SWV[2] = (*(data+3))+'0';
	ptd->SWV[3] = '\0';
	
}

void NH3N_DN02_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;

	
	if(ptd == NULL) return;
	
  ptd->DOmgl.value_arr[0] =	dat[0];
  ptd->DOmgl.value_arr[1] =	dat[1];
	ptd->DOmgl.value_arr[2] =	dat[2];
	ptd->DOmgl.value_arr[3] =	dat[3];


	
  ptd->temperature.value_arr[0] =	dat[4];
  ptd->temperature.value_arr[1] =	dat[5];
	ptd->temperature.value_arr[2] =	dat[6];
	ptd->temperature.value_arr[3] =	dat[7];	
	
  ptd->DOpercent.value_arr[0] =	dat[8];
  ptd->DOpercent.value_arr[1] =	dat[9];
	ptd->DOpercent.value_arr[2] =	dat[10];
	ptd->DOpercent.value_arr[3] =	dat[11];
	
  ptd->pH_Vol.value_arr[0] =	dat[24];
  ptd->pH_Vol.value_arr[1] =	dat[25];
	ptd->pH_Vol.value_arr[2] =	dat[26];
	ptd->pH_Vol.value_arr[3] =	dat[27];

  ptd->NH4_Vol.value_arr[0] =	dat[28];
  ptd->NH4_Vol.value_arr[1] =	dat[29];
	ptd->NH4_Vol.value_arr[2] =	dat[30];
	ptd->NH4_Vol.value_arr[3] =	dat[31];

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
	
	snprintf(ptd->DOpercent_Vol_arr,  8, "%5.2f", (ptd->pH_Vol.value_f*1000 ));
	snprintf(ptd->DOmgl_Vol_arr,       8, "%5.2f", ptd->NH4_Vol.value_f*1000);
		
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
			
			
			
			if(setting_GetIsOpen_SlideAvg_NH4() && setting_GetSlideAvgTimes_NH4()>=2)//开启并且次数最起码为2次
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
