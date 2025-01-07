#include "NH3N_shenghui.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"
/*获取探头Modbus通讯ID*/
void NH3N_shenghui_rs485_GetModbusId(void)
{	
	rs485_usart.tx_buf[0] = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0A;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);

	
	rs485_SetSentType(DO_SendType_GetModbusId);
}

/*获取 NH3 pH 温度*/
void NH3N_shenghui_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0A;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}

/*获取sn*/
void NH3N_shenghui_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_shenghui_ModbusID;
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
void NH3N_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x45;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}



//昇辉氨氮 1标定
void NH3N_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x72;
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

//聚光氨氮 2标定
void NH3N_shenghui_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x76;
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


//氨氮 pH 4.01 标定
void NH3N_shenghui_pH_rs485_Frist(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x62;
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

//聚光氨氮 pH 6.86 标定
void NH3N_shenghui_pH_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x66;
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

//聚光氨氮 pH 9.18 标定
void NH3N_shenghui_pH_rs485_Third(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x6A;
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


void NH3N_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x7A;
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

void NH3N_shenghui_rs485_ClearCal(PtrToDOProbe ptd)
{//发送聚光溶解氧清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = NH3N_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x51;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

void shenghui_SetSN(PtrToDOProbe ptd, uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	ptd->SN[0]=buff[1];
	ptd->SN[1]=buff[0];
	ptd->SN[2]=buff[3];
	ptd->SN[3]=buff[2];	
	ptd->SN[4]=buff[5];
	ptd->SN[5]=buff[4];	
	ptd->SN[6]=buff[7];
	ptd->SN[7]=buff[6];	
	ptd->SN[8]=buff[9];
	ptd->SN[9]=buff[8];	
	ptd->SN[10]=buff[11];
	ptd->SN[11]=buff[10];	
	
  ptd->manufacturer = manufacturer_hyphive;//生产厂商是海发

}
void shenghui_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	ptd->HWV[0] =  '1';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = '0';
	ptd->HWV[3] = '\0';	
	
	ptd->SWV[2] = (*(data));
	ptd->SWV[1] = '.';
	ptd->SWV[0] = (*(data+1));
	ptd->SWV[3] = '\0';	
}


void NH3N_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
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
	
  ptd->DOpercent.value_arr[0] =	dat[9];
  ptd->DOpercent.value_arr[1] =	dat[8];
	ptd->DOpercent.value_arr[2] =	dat[11];
	ptd->DOpercent.value_arr[3] =	dat[10];
	
  ptd->pH_Vol.value_arr[0] =	dat[13];
  ptd->pH_Vol.value_arr[1] =	dat[12];
	ptd->pH_Vol.value_arr[2] =	dat[15];
	ptd->pH_Vol.value_arr[3] =	dat[14];

  ptd->NH4_Vol.value_arr[0] =	dat[17];
  ptd->NH4_Vol.value_arr[1] =	dat[16];
	ptd->NH4_Vol.value_arr[2] =	dat[19];
	ptd->NH4_Vol.value_arr[3] =	dat[18];

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
		
		snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
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
				snprintf(ptd->temperature_arr, 6, "%5.2f", temperature_temp);
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

