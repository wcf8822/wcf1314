#include "TDS_DT49.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"
#include "interfacial.h"


/**
 * @brief 获取电极地址
 * 
 */
void TDS_DT49_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xf2;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/**
 * @brief 获取温度和tds数值
 * 
 */
void TDS_DT49_rs485_GetValue(void)
{
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0x04;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}

/**
 * @brief 获取tds系数
 * 
 */
void TDS_DT49_rs485_Get_tds_temp_value(void)
{
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0x22;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetMesParameter);
}

/**
 * @brief 获取sn编码
 * 
 */
void TDS_DT49_rs485_GetSN(void)
{
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xc0;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x0f;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);	
}


/**
 * @brief 获取软件版本号
 * 
 */
void TDS_DT49_rs485_GetSHWVersion(void)
{
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xd0;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x09;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
}

/**
 * @brief 常数校准
 * 
 * @param ptd 电极信息
 * @param Value 设置标液数值
 */
void TDS_DT49_rs485_Standard(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x06;
	rs485_usart.tx_buf[3]  = 0x46;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetFullCal);
}


/**
 * @brief 设置tds系数
 * 
 * @param ptd 电极信息
 * @param Value 设置的具体数值
 */
void TDS_DT49_SetTds_value(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = TDS_DT49_Modbus;
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

/**
 * @brief 温度校准
 * 
 * @param ptd 电极信息
 * @param temp 需要校准的温度值
 */
void TDS_DT49_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
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


/**
 * @brief 零点标定
 * 
 * @param ptd 电极信息
 * @param Value 设置的具体数值
 */
void TDS_DT49_rs485_Zero(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;

	Cal_Value.value_f=Value;

	rs485_usart.tx_buf[0]  = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x06;
	rs485_usart.tx_buf[3]  = 0x2b;
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



/**
 * @brief 设置传感器的运行模式
 * 
 * @param ptd 电极信息
 * @param Value 0x00测量模式	0x10校准模式	0x30设置模式
 */
void TDS_DT49_rs485_SetMode(PtrToDOProbe ptd,uint16_t Value)
{
	if(ptd == NULL) return;
	
	uint16_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xf4;
	rs485_usart.tx_buf[4] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5] = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

/**
 * @brief 恢复出厂设置
 * 
 * @param ptd 电极信息
 */
void TDS_DT49_rs485_ClearCal(PtrToDOProbe ptd)
{//恢复出厂设置
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = TDS_DT49_Modbus;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x06;
	rs485_usart.tx_buf[3] = 0xf5;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x79; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


/**
 * @brief 设置DT49的sn号
 * 
 * @param ptd 电极信息
 * @param buff 485传入数组,协议获取的sn号
 * @param len sn号长度  0时写成固定的
 */
void TDS_DT49_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	if(ptd->modbus_id == TDS_DT49_Modbus)
	{
		if(len == 0){
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='T';
		ptd->SN[5]='4';	
		ptd->SN[6]='9';
		ptd->SN[7]='_';	
		ptd->SN[8]='T';
		ptd->SN[9]='D';	
		ptd->SN[10]='S';
		ptd->SN[11]=' ';
		}else{
			for(uint8_t i =0; i<len; i++)
			{
				ptd->SN[i] = *(buff + i);
			}
		}	
	}
}

/**
 * @brief 设置版本号
 * 
 * @param ptd 电极信息
 */
void TDS_DS49_SetSHWVersion(PtrToDOProbe ptd)
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

/**
 * @brief 设置tds系数
 * 
 * @param ptd 
 * @param dat 
 */
void TDS_DT49_Settds_temp_xishu(PtrToDOProbe ptd,uint8_t *dat)
{
	//tds系数
	ptd->compensate_b.value_arr[0] =	dat[1];
  	ptd->compensate_b.value_arr[1] =	dat[0];
	ptd->compensate_b.value_arr[2] =	dat[3];
	ptd->compensate_b.value_arr[3] =	dat[2];

	snprintf(ptd->tocmgl_Vol_arr, 6, "%5.3f", ptd->compensate_b.value_f);
}

/**
 * @brief dt49解析
 * 
 * @param ptd 电极信息
 * @param dat 传入数据
 */
void TDS_DT49_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
	float unit_data = 1.0;
	
	if(ptd == NULL) return;
	
	//TDS 
	ptd->DOmgl.value_arr[0] =	dat[1];
	ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];
	
	//温度	
	ptd->temperature.value_arr[0] =	dat[9];
	ptd->temperature.value_arr[1] =	dat[8];
	ptd->temperature.value_arr[2] =	dat[11];
	ptd->temperature.value_arr[3] =	dat[10];	

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
		
		snprintf(ptd->DOmgl_arr,       7, "%6.2f",ptd->DOmgl.value_f);
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
			
			if(setting_GetIsOpen_SlideAvg_TDS() && setting_GetSlideAvgTimes_TDS()>=2)//开启并且次数最起码为2次
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
				snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
			}
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
		}
	}
	rs485_SetIsChangeSenesor();
}