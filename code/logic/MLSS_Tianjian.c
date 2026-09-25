#include "MLSS_Tianjian.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

float MLSS_temp_b = 0;

/**
 * @brief setting_MLSS_Temp_B
 * 
 * @return float 
 */
float Getting_MLSS_Temp_B(void)
{
	return MLSS_temp_b;
}

/**
 * @brief Setting_MLSS_Temp_B
 * 
 * @return float 
 */
void Setting_MLSS_Temp_B(float valve)
{
	MLSS_temp_b = valve;
}

/*获取探头Modbus通讯ID*/
void MLSS_Tianjian_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x9c;
	rs485_usart.tx_buf[3] = 0xa5;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetSentType(DO_SendType_GetModbusId);
}

/*获取污泥浓度测量数值  */
void MLSS_Tianjian_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x9c;
	rs485_usart.tx_buf[3] = 0x40;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}

/**
 * @brief 获取SN 软件版本号
 * 
 * @param ptd 
 */
void MLSS_Tianjian_GetSN_Software(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x9c;
	rs485_usart.tx_buf[3] = 0x46;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetSN);
}

/**
 * @brief 设置软硬件版本号
 * 
 * @param ptd 
 */
uint32_u snvalue;
	float_u swvvalue;
void MLSS_Tianjian_SetSHWVersion(PtrToDOProbe ptd,uint8_t* buff)
{
	if(ptd == NULL) return;
	// int16_u snvalue;
	// float_u swvvalue;

	snvalue.value_arr[0] = buff[1];
	snvalue.value_arr[1] = buff[0];
	snvalue.value_arr[2] = buff[3];
	snvalue.value_arr[3] = buff[2];

	swvvalue.value_arr[0] = buff[5];
	swvvalue.value_arr[1] = buff[4];
	swvvalue.value_arr[2] = buff[7];
	swvvalue.value_arr[3] = buff[6];
	

	snprintf(ptd->SN,sizeof(ptd->SN),"%d",snvalue.value_f);
	snprintf(ptd->SWV,sizeof(ptd->SN),"%3.1f",swvvalue.value_f);
	snprintf(ptd->HWV,sizeof(ptd->HWV),"%3.1f",1.0);
}

/**
 * @brief 读取传感器单位
 * 
 * @param ptd 
 */
void MLSS_Tianjian_Get_Unit(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x9c;
	rs485_usart.tx_buf[3] = 0xa8;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_GetSHWVersion);
}

/*单位设置*/
void MLSS_Tianjian_rs485_Set_Unit(PtrToDOProbe ptd ,uint8_t num)
{
	if(ptd == NULL) return;
	// uint8_t add_one;
	// uint8_t add_two;

	// add_one = id>>8;
	// add_two = id;
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9c;
	rs485_usart.tx_buf[3] = 0xa8;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = num;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetSensorCap);
}

/**
 * @brief 恢复出厂标定信息
 * 
 * @param ptd 
 */
void MLSS_Tianjian_Reset_Info(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9d;
	rs485_usart.tx_buf[3] = 0x38;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

/*零点偏移设置*/
void MLSS_Tianjian_rs485_SetZero(PtrToDOProbe ptd, float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x9d;
	rs485_usart.tx_buf[3] = 0x06;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetZeroCal);
}

/*斜率设置*/
void MLSS_Tianjian_rs485_Set_K(PtrToDOProbe ptd, float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x9d;
	rs485_usart.tx_buf[3] = 0x08;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetKB);
}

/*信号输入*/
void MLSS_Tianjian_rs485_Set_Signal(PtrToDOProbe ptd, float Value, float Value2,uint16_t id)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	float_u Cal_Value_two;
	uint8_t add_one;
	uint8_t add_two;

	add_one = id>>8;
	add_two = id;
	Cal_Value.value_f=Value;
	Cal_Value_two.value_f = Value2;

	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = add_one;
	rs485_usart.tx_buf[3] = add_two;

	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	rs485_usart.tx_buf[6] = 0x08;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];

	rs485_usart.tx_buf[11]  = Cal_Value_two.value_arr[1];
	rs485_usart.tx_buf[12]  = Cal_Value_two.value_arr[0];
	rs485_usart.tx_buf[13]  = Cal_Value_two.value_arr[3];
	rs485_usart.tx_buf[14] = Cal_Value_two.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetTemp);
}

/*浓度输入*/
void MLSS_Tianjian_rs485_Set_Signal_two(PtrToDOProbe ptd, float Value ,uint16_t id)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	uint8_t add_one;
	uint8_t add_two;

	add_one = id>>8;
	add_two = id;
	Cal_Value.value_f=Value;
	
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = add_one;
	rs485_usart.tx_buf[3] = add_two;

	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_Set_Mes_mode);
}

/*标定点数设置*/
void MLSS_Tianjian_rs485_Set_Cal_Num(PtrToDOProbe ptd ,uint8_t num)
{
	if(ptd == NULL) return;
	// uint8_t add_one;
	// uint8_t add_two;

	// add_one = id>>8;
	// add_two = id;	
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9d;
	rs485_usart.tx_buf[3] = 0x05;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = num;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetPressure);
}

/*标定点数读取*/
void MLSS_Tianjian_rs485_Get_Cal_Num(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	// uint8_t add_one;
	// uint8_t add_two;

	// add_one = id>>8;
	// add_two = id;	
	rs485_usart.tx_buf[0] = MLSS_Tianjian_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x9d;
	rs485_usart.tx_buf[3] = 0x05;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_GetPressure);
}

/*标定点数设置*/
void MLSS_Tianjian_rs485_Set_Cal_Num1(PtrToDOProbe ptd,uint8_t* buff)
{
	if(ptd == NULL) return;

	ptd->compensate_b.value_f = buff[1];

}
/**
 * @brief 更新测量数据
 * 
 * @param ptd 
 * @param dat 
 */
void MLSS_Tianjian_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float DOpercent_value = 0.0;
	float DO_mgl_temp = 0.0;
	
	if(ptd == NULL) return;

	/*污泥浓度*/
	ptd->DOmgl.value_arr[0] =	dat[1];
	ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];
	// ptd->DOmgl.value_arr[0] =	0xe1;
	// ptd->DOmgl.value_arr[1] =	0x7a;
	// ptd->DOmgl.value_arr[2] =	0x48;
	// ptd->DOmgl.value_arr[3] =	0x41;
	
	/*信号值*/
  	ptd->DOpercent.value_arr[0] =	dat[5];
 	ptd->DOpercent.value_arr[1] =	dat[4];
	ptd->DOpercent.value_arr[2] =	dat[7];
	ptd->DOpercent.value_arr[3] =	dat[6];

	/*mg*/
	// if(ptd->DOmgl.value_f >= 1000.0f)
	// {
	// 	ptd->DOmgl.value_f = 999;
	// }
	// else if(ptd->DOmgl.value_f <= -1000.0f)
	// {	
	// 	ptd->DOmgl.value_f = -999;
	// }
	
	// /*mg*/
	// if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
	// {
	// 	ptd->DOmgl.value_f = ptd->DOmgl.value_f;
	// }
	// else
	// {
	// 	ptd->DOmgl.value_f = ptd->DOmgl.value_f /100.0f;
	// }

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
		//整数
		if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
		{
			if(ptd->DOmgl.value_f  >= 25000)
			{
				snprintf(ptd->DOpercent_Vol_arr,   8, "%5d", (int)25000);
			}
			else
			{
				snprintf(ptd->DOpercent_Vol_arr,   8, "%5d", (int)ptd->DOmgl.value_f);
			}
		}
		else if(setting_GetMesUnit() == 5)
		{
			if(ptd->DOmgl.value_f  >= 25000)
			{
				snprintf(ptd->DOpercent_Vol_arr,   8, "%5.3f",25000/1000.0f);
			}
			else
			{
				snprintf(ptd->DOpercent_Vol_arr,   8, "%5.3f", ptd->DOmgl.value_f/1000.0f);
			}
		}
		else
                                                                                                           		{
			if(ptd->DOmgl.value_f  >= 25000)
			{
				snprintf(ptd->DOpercent_Vol_arr,   8, "%5.4f",25000/10000.0f);
			}
			else
			{
				snprintf(ptd->DOpercent_Vol_arr,   8, "%5.4f", ptd->DOmgl.value_f/10000.0f);
			}
		}
	}
	else
	{
		ptd->DOmgl_sum += ptd->DOmgl.value_f;
		ptd->DOpercent_sum += ptd->DOpercent.value_f;
		
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;//更新一下上次的值
		
		if(++ptd->update_count >= 3)
		{
			DOpercent_value = ptd->DOpercent_sum / ((float)ptd->update_count);
			DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
			
			if(setting_GetIsOpen_SlideAvg_MLSS() && setting_GetSlideAvgTimes_MLSS()>=2)//开启并且次数最起码为2次
			{
				if(is_FirstFilter)
				{
					is_FirstFilter = 0;
					
					filter_clear(&(ptd->queue_domgl));
					filter_clear(&(ptd->queue_temp));
				}
				
				filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
				filter_inset2arr(&(ptd->queue_temp), DOpercent_value);
				
				DOpercent_value  = filter_get_avg(&(ptd->queue_temp)); 
				DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
			}
			else
			{
				is_FirstFilter = 1;
			}
			
			if(!DO_GetValueLocked(ptd))
			{
				//整数
				if(setting_GetMesUnit() == 0 || setting_GetMesUnit() == 10)
				{
					if(DO_mgl_temp  >= 25000)
					{
						snprintf(ptd->DOpercent_Vol_arr,   8, "%5d", (int)25000);
					}
					else
					{
						snprintf(ptd->DOpercent_Vol_arr,   8, "%5d", (int)DO_mgl_temp);
					}
				}
				else if(setting_GetMesUnit() == 5)
				{
					if(DO_mgl_temp >= 25000)
					{
						snprintf(ptd->DOpercent_Vol_arr,   8, "%5.3f", 25000/1000.0f);
					}
					else
					{
						snprintf(ptd->DOpercent_Vol_arr,   8, "%5.3f", DO_mgl_temp/1000.0f);
					}
				}
				else
				{
					if(DO_mgl_temp >= 25000)
					{
						snprintf(ptd->DOpercent_Vol_arr,   8, "%5.4f", 25000/10000.0f);
					}
					else
					{
						snprintf(ptd->DOpercent_Vol_arr,   8, "%5.4f", DO_mgl_temp/10000.0f);
					}
				}

			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;	
		}
	}
}