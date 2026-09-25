#include "EC_DE26.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"
#include "interfacial.h"


/*获取探头Modbus通讯ID*/
void EC_DE26_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0A;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 电导率  温度 */
void EC_DE26_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0A;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}



/*获取sn*/
void EC_DE26_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xc0;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0f;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取tds,温度系数*/
void EC_DE26_rs485_Get_tds_temp_value(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0x22;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetMesParameter);
}

/*获取软硬件版本号*/
void EC_DE26_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xd0;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0f;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
}

// 电导率电极设置TDS系数
void EC_DE26_SetTds_value(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x06;
	rs485_usart.tx_buf[3]  = 0x22;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetSalinity);
}

// 电导率电极设置温度补偿系数
void EC_DE26_SetTemp_value(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x06;
	rs485_usart.tx_buf[3]  = 0x26;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetPressure);
}

// 电导率单点标定 1413μS/cm
void EC_DE26_rs485_Frist(PtrToDOProbe ptd,float Value,uint8_t modbus)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = modbus;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x06;
	rs485_usart.tx_buf[3]  = 0x20;
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

//电导率零点 标定
void EC_DE26_rs485_Zero(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x06;
	rs485_usart.tx_buf[3]  = 0x28;
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


void EC_DE26_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0x24;
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


//设置运行模式    测量模式（0x00） 校准模式（0x10） 设置模式（0x30）
void EC_DE26_rs485_SetMode(PtrToDOProbe ptd,uint16_t Value)
{
	if(ptd == NULL) return;
	
	uint16_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xf4;
	rs485_usart.tx_buf[4] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5] = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void EC_DE26_rs485_ClearCal(PtrToDOProbe ptd)
{//恢复出厂设置
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_DE26_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xf5;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x79; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}



void EC_DE26_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len)
{
	// if(ptd == NULL) return;
	if(len == 0){
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='E';
		ptd->SN[5]='2';	
		ptd->SN[6]='6';
		ptd->SN[7]='_';	
		ptd->SN[8]='E';
		ptd->SN[9]='C';	
		ptd->SN[10]=' ';
		ptd->SN[11]=' ';	

	}else{
		for(uint8_t i =0; i<len; i++)
		{
			ptd->SN[i] = *(buff + i);
		}
	}		
}
void EC_DE26_SetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	ptd->HWV[0] = '1';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = '0';
	ptd->HWV[3] = '\0';
	
	ptd->SWV[0] = '1';
	ptd->SWV[1] = '.';
	ptd->SWV[2] = '0';
	ptd->SWV[3] = '\0';

}

void EC_DE26_Settds_temp_xishu(PtrToDOProbe ptd,uint8_t *dat)
{
	//tds系数
	ptd->compensate_b.value_arr[0] =	dat[1];
  	ptd->compensate_b.value_arr[1] =	dat[0];
	ptd->compensate_b.value_arr[2] =	dat[3];
	ptd->compensate_b.value_arr[3] =	dat[2];

	snprintf(ptd->tocmgl_Vol_arr, 6, "%5.3f", ptd->compensate_b.value_f);

	//温度系数
	ptd->compensate_k.value_arr[0] =	dat[9];
	ptd->compensate_k.value_arr[1] =	dat[8];
	ptd->compensate_k.value_arr[2] =	dat[11];
	ptd->compensate_k.value_arr[3] =	dat[10];
 
	ptd->compensate_k.value_f = ptd->compensate_k.value_f * 100;
	snprintf(ptd->DOmgl_Vol_arr, 6, "%4.2f", ptd->compensate_k.value_f);
}

void EC_DE26_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
	float unit_data = 1.0;
  static unsigned char EC_unit_Flag=0;	
	
	if(ptd == NULL) return;
	
	//电导率
  ptd->DOmgl.value_arr[0] =	dat[1];
  ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];

  //TDS 
  ptd->DOpercent.value_arr[0] =	dat[9];
  ptd->DOpercent.value_arr[1] =	dat[8];
	ptd->DOpercent.value_arr[2] =	dat[11];
	ptd->DOpercent.value_arr[3] =	dat[10];

  //盐度	
  ptd->pH_Vol.value_arr[0] =	dat[13];
  ptd->pH_Vol.value_arr[1] =	dat[12];
	ptd->pH_Vol.value_arr[2] =	dat[15];
	ptd->pH_Vol.value_arr[3] =	dat[14];
	
  //温度	
  ptd->temperature.value_arr[0] =	dat[17];
  ptd->temperature.value_arr[1] =	dat[16];
	ptd->temperature.value_arr[2] =	dat[19];
	ptd->temperature.value_arr[3] =	dat[18];	
	
	// if(setting_GetSal_Uni() == 5)
	// {
	// 	unit_data = 10;
	// }
	// else
	// {
		unit_data = 1;
	// }

	// snprintf(ptd->DOpercent_Vol_arr,  7, "%5.2f", (ptd->pH_Vol.value_f));
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
		// snprintf(ptd->DOpercent_arr,   7, "%5.2f", (ptd->DOpercent.value_f ));
		//snprintf(ptd->DOmgl_arr,       7, "%5.2f", ptd->DOmgl.value_f);
		
		if(ptd->DOmgl.value_f >= 1000000)//1000ms ---
		{
			snprintf(ptd->DOmgl_arr,       7, "%5d",1000);
			snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)ptd->DOpercent.value_f / 1000);	
		}
		else if(ptd->DOmgl.value_f >= 100000.0 && ptd->DOmgl.value_f < 1000000.0)//100.0ms - 999.9ms
		{
			snprintf(ptd->DOmgl_arr,       7, "%5.1f",ptd->DOmgl.value_f / 1000.0);		
			snprintf(ptd->DOpercent_arr,       7, "%5.1f",ptd->DOpercent.value_f / 1000.0);		
		}
		else if(ptd->DOmgl.value_f >= 10000.0 && ptd->DOmgl.value_f < 100000.0)//10.00ms - 99.99ms
		{
			snprintf(ptd->DOmgl_arr,       7, "%5.2f",ptd->DOmgl.value_f / 1000.0);		
			snprintf(ptd->DOpercent_arr,       7, "%5.2f",ptd->DOpercent.value_f / 1000.0);		
		}
		else if(ptd->DOmgl.value_f >= 1000.0 && ptd->DOmgl.value_f < 10000.0)//1000 - 9999 无小数
		{
			snprintf(ptd->DOmgl_arr,       7, "%5d",(unsigned int)ptd->DOmgl.value_f);	
			snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)ptd->DOpercent.value_f);	
		}
		else if(ptd->DOmgl.value_f >= 100.0 && ptd->DOmgl.value_f < 1000.0)//100.0 - 999.9 一位小数
		{
			snprintf(ptd->DOmgl_arr,       7, "%5.1f",ptd->DOmgl.value_f);	
			snprintf(ptd->DOpercent_arr,       7, "%5.1f",ptd->DOpercent.value_f);
		}
		else if(ptd->DOmgl.value_f >= 0.0 && ptd->DOmgl.value_f < 100.0)//0.00 - 99.99 两位小数
		{
			snprintf(ptd->DOmgl_arr,       7, "%5.2f",ptd->DOmgl.value_f);
			snprintf(ptd->DOpercent_arr,       7, "%5.2f",ptd->DOpercent.value_f);
		}
		//tds
		// if(ptd->DOpercent.value_f >= 10000)//1000ms ---
		// {
		// 	snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)ptd->DOpercent.value_f);			
		// }
		// else if(ptd->DOpercent.value_f >= 1000.0 && ptd->DOpercent.value_f < 10000.0)//1000 - 9999 无小数
		// {
		// 	snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)ptd->DOpercent.value_f);		
		// }
		// else if(ptd->DOpercent.value_f >= 100.0 && ptd->DOpercent.value_f < 1000.0)//100.0 - 999.9 一位小数
		// {
		// 	snprintf(ptd->DOpercent_arr,       7, "%5.1f",ptd->DOpercent.value_f);	
		// }
		// else if(ptd->DOpercent.value_f >= 0.0 && ptd->DOpercent.value_f < 100.0)//0.00 - 99.99 两位小数
		// {
		// 	snprintf(ptd->DOpercent_arr,       7, "%5.2f",ptd->DOpercent.value_f);
		// }

		//盐度
		if(ptd->pH_Vol.value_f >= 10000)//1000ms ---
		{
			snprintf(ptd->DOpercent_Vol_arr,       7, "%5d",(unsigned int)ptd->pH_Vol.value_f / unit_data);			
		}
		else if(ptd->pH_Vol.value_f >= 1000.0 && ptd->pH_Vol.value_f < 10000.0)//1000 - 9999 无小数
		{
			snprintf(ptd->DOpercent_Vol_arr,       7, "%5d",(unsigned int)ptd->pH_Vol.value_f / unit_data);		
		}
		else if(ptd->pH_Vol.value_f >= 100.0 && ptd->pH_Vol.value_f < 1000.0)//100.0 - 999.9 一位小数
		{
			snprintf(ptd->DOpercent_Vol_arr,       7, "%5.1f",ptd->pH_Vol.value_f / unit_data);	
		}
		else if(ptd->pH_Vol.value_f >= 0.0 && ptd->pH_Vol.value_f < 100.0)//0.00 - 99.99 两位小数
		{
			snprintf(ptd->DOpercent_Vol_arr,       7, "%5.2f",ptd->pH_Vol.value_f / unit_data);
		}
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
			
			
			
			if(setting_GetIsOpen_SlideAvg_EC() && setting_GetSlideAvgTimes_EC()>=2)//开启并且次数最起码为2次
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
				// snprintf(ptd->DOpercent_arr,   7, "%5.2f", DO_Percent_temp);
				//snprintf(ptd->DOmgl_arr,       7, "%5.2f", DO_mgl_temp);
				if(DO_mgl_temp >= 1000000)//1000ms ---
				{
					snprintf(ptd->DOmgl_arr,       7, "%5d",1000);	
					snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)DO_Percent_temp / 1000);				
				}
				else if(DO_mgl_temp >= 100000.0 && DO_mgl_temp < 1000000.0)//100.0ms - 999.9ms
				{
					snprintf(ptd->DOmgl_arr,       7, "%5.1f",DO_mgl_temp / 1000.0);	
					snprintf(ptd->DOpercent_arr,       7, "%5.1f",DO_Percent_temp / 1000.0);				
				}
				else if(DO_mgl_temp >= 10000.0 && DO_mgl_temp < 100000.0)//10.00ms - 99.99ms
				{
					snprintf(ptd->DOmgl_arr,       7, "%5.2f",DO_mgl_temp / 1000.0);	
					snprintf(ptd->DOpercent_arr,       7, "%5.2f",DO_Percent_temp / 1000.0);			
				}
				else if(DO_mgl_temp >= 1000.0 && DO_mgl_temp < 10000.0)//1000us - 9999 无小数
				{
					snprintf(ptd->DOmgl_arr,       7, "%5d",(unsigned int)DO_mgl_temp);	
					snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)DO_Percent_temp);		
				}
				else if(DO_mgl_temp >= 100.0 && DO_mgl_temp < 1000.0)//100.0us - 999.9 一位小数
				{
					snprintf(ptd->DOmgl_arr,       7, "%5.1f",DO_mgl_temp);	
					snprintf(ptd->DOpercent_arr,       7, "%5.1f",DO_Percent_temp);	
				}
				else if(DO_mgl_temp >= 0.0 && DO_mgl_temp < 100.0)//0.00us - 99.99 两位小数
				{
					snprintf(ptd->DOmgl_arr,       7, "%5.2f",DO_mgl_temp);
					snprintf(ptd->DOpercent_arr,       7, "%5.2f",DO_Percent_temp);
				}
				//tds
				// if(DO_Percent_temp >= 10000)//1000ms ---
				// {
				// 	snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)DO_Percent_temp);			
				// }
				// else if(DO_Percent_temp >= 1000.0 && DO_Percent_temp < 10000.0)//1000 - 9999 无小数
				// {
				// 	snprintf(ptd->DOpercent_arr,       7, "%5d",(unsigned int)DO_Percent_temp);		
				// }
				// else if(DO_Percent_temp >= 100.0 && DO_Percent_temp < 1000.0)//100.0 - 999.9 一位小数
				// {
				// 	snprintf(ptd->DOpercent_arr,       7, "%5.1f",DO_Percent_temp);	
				// }
				// else if(DO_Percent_temp >= 0.0 && DO_Percent_temp < 100.0)//0.00 - 99.99 两位小数
				// {
				// 	snprintf(ptd->DOpercent_arr,       7, "%5.2f",DO_Percent_temp);
				// }

				//盐度
				if(ptd->pH_Vol.value_f >= 10000)//1000ms ---
				{
					snprintf(ptd->DOpercent_Vol_arr,       7, "%5d",(unsigned int)ptd->pH_Vol.value_f / unit_data);			
				}
				else if(ptd->pH_Vol.value_f >= 1000.0 && ptd->pH_Vol.value_f < 10000.0)//1000 - 9999 无小数
				{
					snprintf(ptd->DOpercent_Vol_arr,       7, "%5d",(unsigned int)ptd->pH_Vol.value_f / unit_data);		
				}
				else if(ptd->pH_Vol.value_f >= 100.0 && ptd->pH_Vol.value_f < 1000.0)//100.0 - 999.9 一位小数
				{
					snprintf(ptd->DOpercent_Vol_arr,       7, "%5.1f",ptd->pH_Vol.value_f / unit_data);	
				}
				else if(ptd->pH_Vol.value_f >= 0.0 && ptd->pH_Vol.value_f < 100.0)//0.00 - 99.99 两位小数
				{
					snprintf(ptd->DOpercent_Vol_arr,       7, "%5.2f",ptd->pH_Vol.value_f / unit_data);
				}
			}
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
		}
	}
	rs485_SetIsChangeSenesor();
}
